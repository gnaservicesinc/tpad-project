#!/usr/bin/env bash

# Exercise normal editor workflows under an isolated X server and config file.

set -euo pipefail

if (( $# == 0 )); then
	printf 'Usage: %s COMMAND [ARG ...]\n' "${0##*/}" >&2
	exit 2
fi

for tool in timeout xvfb-run xdotool grep cp rm; do
	if ! command -v "$tool" >/dev/null 2>&1; then
		printf 'error: %s is required for the GUI workflow test\n' "$tool" >&2
		exit 1
	fi
done

# The single-quoted program is intentionally expanded by the inner Bash.
# shellcheck disable=SC2016
timeout 180s xvfb-run -a bash -c '
	set -euo pipefail
	command=("$@")
	test_dir=$(mktemp -d "${TMPDIR:-/tmp}/tpad-gui-workflows.XXXXXX")
	tracked_pids=()

	cleanup() {
		local pid
		set +e
		for pid in "${tracked_pids[@]}"; do
			kill "$pid" 2>/dev/null
		done
		if [[ -n $test_dir && -d $test_dir ]]; then
			rm -r -- "$test_dir"
		fi
	}
	trap cleanup EXIT

	export LC_ALL=C.UTF-8
	export GDK_SCALE=1
	export TPAD_CONFIG_FILE="$test_dir/tpad.cfg"

	wait_for_window() {
		local search_kind=$1
		local pattern=$2
		local attempt

		found_window=
		for ((attempt = 0; attempt < 300; attempt++)); do
			found_window=$(xdotool search --onlyvisible "$search_kind" "$pattern" \
				2>/dev/null | head -n 1 || true)
			if [[ -n $found_window ]]; then
				return 0
			fi
			sleep 0.1
		done
		return 1
	}

	wait_for_window_gone() {
		local target=$1
		local attempt

		for ((attempt = 0; attempt < 300; attempt++)); do
			if ! xdotool getwindowname "$target" >/dev/null 2>&1; then
				return 0
			fi
			sleep 0.1
		done
		return 1
	}

	track_window_process() {
		local target=$1
		local pid

		pid=$(xdotool getwindowpid "$target" 2>/dev/null || true)
		if [[ $pid =~ ^[0-9]+$ ]]; then
			tracked_pids+=("$pid")
		fi
	}

	send_key_to_window() {
		local target=$1
		shift
		xdotool windowfocus --sync "$target"
		xdotool key --clearmodifiers "$@"
	}

	fixture="$test_dir/open-save.txt"
	printf "%s\n" "alpha tpad omega" "second tpad line" > "$fixture"
	"${command[@]}" "$fixture" &
	launcher_pid=$!
	if ! wait_for_window --class tpad; then
		printf "error: tpad did not open the workflow fixture\n" >&2
		exit 1
	fi
	main_window=$found_window
	main_pid=$(xdotool getwindowpid "$main_window")
	track_window_process "$main_window"
	wait "$launcher_pid" || true

	# Replace all occurrences through the actual dialog, then save the file.
	send_key_to_window "$main_window" ctrl+h
	if ! wait_for_window --name "^Find / Find & Replace Text$"; then
		printf "error: Ctrl+H did not open Find / Replace\n" >&2
		exit 1
	fi
	find_window=$found_window
	xdotool windowfocus --sync "$find_window"
	xdotool key --clearmodifiers ctrl+a
	xdotool type --clearmodifiers --delay 5 tpad
	xdotool key --clearmodifiers Tab ctrl+a
	xdotool type --clearmodifiers --delay 5 releasepad
	xdotool key --clearmodifiers Tab Tab Tab space
	send_key_to_window "$find_window" Escape
	if ! wait_for_window_gone "$find_window"; then
		printf "error: Escape did not close Find / Replace\n" >&2
		exit 1
	fi
	send_key_to_window "$main_window" ctrl+s
	for ((attempt = 0; attempt < 100; attempt++)); do
		if grep -Fq releasepad "$fixture"; then
			break
		fi
		sleep 0.1
	done
	if ! grep -Fq releasepad "$fixture" || grep -Fq tpad "$fixture"; then
		printf "error: Find / Replace result was not saved\n" >&2
		exit 1
	fi
	printf "Open, Find / Replace, and Save workflow passed.\n"

	# Exercise the real Preferences action twice. The conventional Ctrl+comma
	# accelerator avoids fragile assumptions about menu geometry.
	send_key_to_window "$main_window" ctrl+comma
	if ! wait_for_window --name "^Display UI Settings$"; then
		printf "error: Ctrl+comma did not open Preferences\n" >&2
		exit 1
	fi
	pref_window=$found_window
	send_key_to_window "$main_window" ctrl+comma
	sleep 0.2
	mapfile -t preference_windows < <(
		xdotool search --onlyvisible --name "^Display UI Settings$" \
			2>/dev/null || true
	)
	if (( ${#preference_windows[@]} != 1 )) ||
	   [[ ${preference_windows[0]} != "$pref_window" ]]; then
		printf "error: Preferences is not a single reusable window\n" >&2
		exit 1
	fi
	printf "Preferences workflow passed.\n"
	send_key_to_window "$pref_window" Escape
	if ! wait_for_window_gone "$pref_window"; then
		printf "error: Escape did not close Preferences\n" >&2
		exit 1
	fi

	# Deleting a tracked file must be treated as an external change, never as
	# permission to silently recreate it. Restore the unchanged snapshot only
	# after observing and cancelling the warning, then save normally.
	snapshot="$test_dir/pre-delete-snapshot.txt"
	cp -- "$fixture" "$snapshot"
	xdotool mousemove --window "$main_window" 300 300 click 1
	send_key_to_window "$main_window" ctrl+End
	xdotool key --clearmodifiers Return
	xdotool type --clearmodifiers --delay 5 "post-delete edit"
	rm -- "$fixture"
	send_key_to_window "$main_window" ctrl+s
	change_dialog=
	for ((attempt = 0; attempt < 300; attempt++)); do
		mapfile -t visible_windows < <(
			xdotool search --onlyvisible --class tpad 2>/dev/null || true
		)
		for candidate in "${visible_windows[@]}"; do
			candidate_pid=$(xdotool getwindowpid "$candidate" 2>/dev/null || true)
			if [[ $candidate != "$main_window" &&
			      $candidate_pid == "$main_pid" ]]; then
				change_dialog=$candidate
				break
			fi
		done
		[[ -n $change_dialog ]] && break
		sleep 0.1
	done
	if [[ -z $change_dialog || -e $fixture ]]; then
		printf "error: deleted file was saved without an external-change warning\n" >&2
		exit 1
	fi
	send_key_to_window "$change_dialog" Escape
	if ! wait_for_window_gone "$change_dialog"; then
		printf "error: external-change warning did not close\n" >&2
		exit 1
	fi
	cp -- "$snapshot" "$fixture"
	send_key_to_window "$main_window" ctrl+s
	for ((attempt = 0; attempt < 100; attempt++)); do
		grep -Fq "post-delete edit" "$fixture" && break
		sleep 0.1
	done
	if ! grep -Fq "post-delete edit" "$fixture"; then
		printf "error: document did not save after the tracked file returned\n" >&2
		exit 1
	fi
	printf "External deletion guard workflow passed.\n"

	send_key_to_window "$main_window" ctrl+q
	if ! wait_for_window_gone "$main_window"; then
		printf "error: first workflow window did not close\n" >&2
		exit 1
	fi

	# A multi-file invocation must create one visible editor window per file.
	first_file="$test_dir/first.txt"
	second_file="$test_dir/café.txt"
	printf "%s\n" first > "$first_file"
	printf "%s\n" second > "$second_file"
	"${command[@]}" "$first_file" "$second_file" &
	launcher_pid=$!
	windows=()
	for ((attempt = 0; attempt < 300; attempt++)); do
		mapfile -t windows < <(xdotool search --onlyvisible --class tpad \
			2>/dev/null || true)
		if (( ${#windows[@]} == 2 )); then
			break
		fi
		sleep 0.1
	done
	if (( ${#windows[@]} != 2 )); then
		printf "error: multi-file launch created %d windows, expected 2\n" \
			"${#windows[@]}" >&2
		exit 1
	fi
	wait "$launcher_pid" || true
	for main_window in "${windows[@]}"; do
		track_window_process "$main_window"
		send_key_to_window "$main_window" ctrl+q
		if ! wait_for_window_gone "$main_window"; then
			printf "error: multi-file window did not close\n" >&2
			exit 1
		fi
	done
	printf "Multi-file workflow passed.\n"
' tpad-gui-workflows "$@"
