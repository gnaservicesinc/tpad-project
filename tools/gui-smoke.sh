#!/usr/bin/env bash

# Run a tpad command inside Xvfb, exercise the print dialog, and close the
# editor cleanly so leak detectors and sanitizers get normal exit handling.

set -euo pipefail

if (( $# == 0 )); then
	printf 'Usage: %s COMMAND [ARG ...]\n' "${0##*/}" >&2
	exit 2
fi

for tool in xvfb-run xdotool; do
	if ! command -v "$tool" >/dev/null 2>&1; then
		printf 'error: %s is required for the GUI smoke test\n' "$tool" >&2
		exit 1
	fi
done

# The single-quoted program is intentionally expanded by the inner Bash.
# shellcheck disable=SC2016
xvfb-run -a bash -c '
	set -euo pipefail
	test_dir=$(mktemp -d)
	trap '\''rm -rf -- "$test_dir"'\'' EXIT
	export TPAD_CONFIG_FILE="$test_dir/tpad.cfg"
	export TPAD_RECENT_FILE="$test_dir/tpad.recent"
	# A bare Xvfb session has no portal UI, even when the D-Bus service is
	# activatable. Force GTKs native print dialog for this UI-level check.
	export GDK_DEBUG=no-portals
	export G_DEBUG="${G_DEBUG:-fatal-criticals}"
	"$@" &
	launcher_pid=$!

	window_id=
	for ((attempt = 0; attempt < 300; attempt++)); do
		window_id=$(xdotool search --onlyvisible --class tpad 2>/dev/null | head -n 1 || true)
		if [[ -n $window_id ]]; then
			break
		fi
		sleep 0.1
	done

	if [[ -z $window_id ]]; then
		printf "error: tpad did not create a visible window\n" >&2
		wait "$launcher_pid" || true
		exit 1
	fi

	# Bare Xvfb has no window manager to establish focus.  Sending a key with
	# xdotool --window uses a synthetic X event, which GTK may ignore.  Give
	# the target real X input focus first, then send the key normally.
	send_key_to_window() {
		local target=$1
		local key=$2
		xdotool windowfocus --sync "$target"
		xdotool key --clearmodifiers "$key"
	}
	wait_for_window_gone() {
		local target=$1
		for ((attempt = 0; attempt < 300; attempt++)); do
			if ! xdotool getwindowname "$target" >/dev/null 2>&1; then
				return 0
			fi
			sleep 0.1
		done
		return 1
	}

	# Exercise cancellation through the GTK 4 async file-dialog helper.
	send_key_to_window "$window_id" ctrl+o
	open_id=
	for ((attempt = 0; attempt < 300; attempt++)); do
		open_id=$(xdotool search --onlyvisible --name "^Open File" 2>/dev/null | head -n 1 || true)
		if [[ -n $open_id ]]; then
			break
		fi
		sleep 0.1
	done
	if [[ -z $open_id ]]; then
		printf "error: Ctrl+O did not open the file dialog\n" >&2
		exit 1
	fi
	send_key_to_window "$open_id" Escape
	if ! wait_for_window_gone "$open_id"; then
		printf "error: file dialog did not close after Escape\n" >&2
		exit 1
	fi

	# Verify that the File -> Print action reaches GTKs system print dialog.
	send_key_to_window "$window_id" ctrl+p
	print_id=
	for ((attempt = 0; attempt < 300; attempt++)); do
		print_id=$(xdotool search --onlyvisible --name "^Print" 2>/dev/null | head -n 1 || true)
		if [[ -n $print_id ]]; then
			break
		fi
		sleep 0.1
	done
	if [[ -z $print_id ]]; then
		printf "error: Ctrl+P did not open the print dialog\n" >&2
		exit 1
	fi
	send_key_to_window "$print_id" Escape

	wait_for_window_gone "$print_id" || true

	send_key_to_window "$window_id" ctrl+q
	for ((attempt = 0; attempt < 300; attempt++)); do
		if ! xdotool getwindowname "$window_id" >/dev/null 2>&1; then
			break
		fi
		sleep 0.1
	done

	if xdotool getwindowname "$window_id" >/dev/null 2>&1; then
		printf "error: tpad did not close after Ctrl+Q\n" >&2
		exit 1
	fi

	wait "$launcher_pid" || true
' tpad-gui-smoke "$@"
