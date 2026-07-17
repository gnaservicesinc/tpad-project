#!/usr/bin/env bash

# Reproducible release checks for tpad.  Every LLVM command is explicitly
# versioned so this script cannot pull in or accidentally select another LLVM.

set -euo pipefail

usage() {
	printf 'Usage: %s [all|build|analyzer|asan|tsan|tysan|valgrind|fuzz|extras]\n' \
		"${0##*/}" >&2
}

audit_mode=${1:-all}
case $audit_mode in
	all|build|analyzer|asan|tsan|tysan|valgrind|fuzz|extras) ;;
	*) usage; exit 2 ;;
esac

repo_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
audit_stamp=$(date +%Y%m%d-%H%M%S)
audit_output=${TPAD_AUDIT_OUTPUT:-"$repo_dir/BUILDLOGS/release-audit-$audit_stamp"}
audit_jobs=$(getconf _NPROCESSORS_ONLN 2>/dev/null || printf '1')

if [[ -e $audit_output ]]; then
	printf 'error: audit output already exists: %s\n' "$audit_output" >&2
	exit 1
fi
mkdir -p -- "$audit_output"

for tool in clang-23 clang++-23 llvm-symbolizer-23 make pkg-config; do
	if ! command -v "$tool" >/dev/null 2>&1; then
		printf 'error: required tool is missing: %s\n' "$tool" >&2
		exit 1
	fi
done

configure_build() {
	local build_name=$1
	local compile_flags=$2
	local link_flags=$3
	local build_dir="$audit_output/build-$build_name"

	mkdir -p -- "$build_dir"
	(
		cd -- "$build_dir"
		CC=/usr/bin/clang-23 \
		CXX=/usr/bin/clang++-23 \
		CFLAGS="$compile_flags" \
		LDFLAGS="$link_flags" \
		"$repo_dir/configure"
	) 2>&1 | tee "$audit_output/configure-$build_name.log" >&2
	printf '%s\n' "$build_dir"
}

build_and_check() {
	local build_dir=$1
	local log_name=$2

	make -C "$build_dir" -j"$audit_jobs" V=1 2>&1 |
		tee "$audit_output/build-$log_name.log"
	make -C "$build_dir" -j"$audit_jobs" check 2>&1 |
		tee "$audit_output/check-$log_name.log"
}

run_gui() {
	local log_name=$1
	shift
	"$repo_dir/tools/gui-smoke.sh" "$@" 2>&1 |
		tee "$audit_output/gui-$log_name.log"
}

run_print_export() {
	local log_name=$1
	shift
	"$repo_dir/tools/print-export-smoke.sh" \
		"$audit_output/print-$log_name.pdf" "$@" 2>&1 |
		tee "$audit_output/print-$log_name.log"
}

run_workflows() {
	local log_name=$1
	shift
	"$repo_dir/tools/gui-workflows-smoke.sh" "$@" 2>&1 |
		tee "$audit_output/workflows-$log_name.log"
}

fail_on_pattern() {
	local search_dir=$1
	local pattern=$2
	local summary=$3

	grep -R -E "$pattern" "$search_dir" > "$audit_output/$summary" 2>/dev/null || true
	if [[ -s $audit_output/$summary ]]; then
		printf 'error: diagnostics matched %s\n' "$pattern" >&2
		cat "$audit_output/$summary" >&2
		return 1
	fi
}

run_build() {
	local flags
	local build_dir

	flags='-O2 -g3'
	build_dir=$(configure_build release "$flags" '')
	build_and_check "$build_dir" release
	run_gui release "$build_dir/src/tpad" "$repo_dir/README.md"
	run_workflows release "$build_dir/src/tpad"
	run_print_export release "$build_dir/tests/test_print_export"
}

run_analyzer() {
	local build_dir

	if ! command -v scan-build-23 >/dev/null 2>&1; then
		printf 'error: scan-build-23 is required\n' >&2
		return 1
	fi
	build_dir=$(configure_build analyzer '-O1 -g3' '')
	scan-build-23 \
		--use-analyzer=/usr/bin/clang-23 \
		--use-cc=/usr/bin/clang-23 \
		--use-c++=/usr/bin/clang++-23 \
		--status-bugs --keep-empty --show-description \
		--html-title='tpad clang-23 release audit' \
		-o "$audit_output/analyzer" \
		make -C "$build_dir" -j"$audit_jobs" check V=1 2>&1 |
		tee "$audit_output/analyzer.log"
}

run_asan() {
	local flags
	local build_dir
	local asan_options

	flags='-O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=address,undefined -fno-sanitize-recover=undefined'
	build_dir=$(configure_build asan "$flags" '-fsanitize=address,undefined')
	build_and_check "$build_dir" asan
	DEBUGINFOD_URLS='' \
	ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer-23 \
	ASAN_OPTIONS='detect_leaks=1:halt_on_error=1:symbolize=1' \
	LSAN_OPTIONS="exitcode=23:report_objects=1:log_path=$audit_output/lsan-unit" \
	UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1:log_path=$audit_output/ubsan-unit" \
		"$build_dir/tests/test_core" 2>&1 | tee "$audit_output/unit-asan.log"
	asan_options="detect_leaks=0:halt_on_error=1:strict_string_checks=1:check_initialization_order=1:detect_stack_use_after_return=1:symbolize=1:suppressions=$repo_dir/tools/asan-system.supp:log_path=$audit_output/asan"
	run_gui asan env \
		DEBUGINFOD_URLS= \
		ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer-23 \
		ASAN_OPTIONS="$asan_options" \
		UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1:log_path=$audit_output/ubsan" \
		"$build_dir/src/tpad" "$repo_dir/README.md"
	run_workflows asan env \
		DEBUGINFOD_URLS= \
		ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer-23 \
		ASAN_OPTIONS="$asan_options" \
		UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1:log_path=$audit_output/ubsan" \
		"$build_dir/src/tpad"
	run_print_export asan env \
		DEBUGINFOD_URLS= \
		ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer-23 \
		ASAN_OPTIONS="$asan_options" \
		UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1:log_path=$audit_output/ubsan" \
		"$build_dir/tests/test_print_export"
	fail_on_pattern "$audit_output" \
		'ERROR: (AddressSanitizer|LeakSanitizer)|runtime error:' \
		'asan-findings.txt'
}

run_tsan() {
	local flags
	local build_dir

	flags='-O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=thread'
	build_dir=$(configure_build tsan "$flags" '-fsanitize=thread')
	build_and_check "$build_dir" tsan
	fail_on_pattern "$audit_output" \
		'(WARNING|ERROR): ThreadSanitizer' 'tsan-findings.txt'
}

run_tysan() {
	local flags
	local build_dir

	flags='-O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=type'
	build_dir=$(configure_build tysan "$flags" '-fsanitize=type')
	build_and_check "$build_dir" tysan
	run_gui tysan env \
		DEBUGINFOD_URLS= \
		TYSAN_OPTIONS="print_stacktrace=1:halt_on_error=1:log_path=$audit_output/tysan" \
		"$build_dir/src/tpad" "$repo_dir/README.md"
	run_workflows tysan env \
		DEBUGINFOD_URLS= \
		TYSAN_OPTIONS="print_stacktrace=1:halt_on_error=1:log_path=$audit_output/tysan" \
		"$build_dir/src/tpad"
	run_print_export tysan env \
		DEBUGINFOD_URLS= \
		TYSAN_OPTIONS="print_stacktrace=1:halt_on_error=1:log_path=$audit_output/tysan" \
		"$build_dir/tests/test_print_export"
	fail_on_pattern "$audit_output" \
		'ERROR: TypeSanitizer' 'tysan-findings.txt'
}

run_valgrind() {
	local build_dir
	local suppression
	local -a gui_memcheck_options

	if ! command -v valgrind >/dev/null 2>&1; then
		printf 'error: valgrind is required\n' >&2
		return 1
	fi
	for suppression in \
		/usr/share/glib-2.0/valgrind/glib.supp \
		/usr/share/gtk-3.0/valgrind/gtk.supp; do
		if [[ ! -r $suppression ]]; then
			printf 'error: required Valgrind suppression is missing: %s\n' \
				"$suppression" >&2
			return 1
		fi
	done
	build_dir=$(configure_build valgrind '-O1 -g3 -fno-omit-frame-pointer' '')
	build_and_check "$build_dir" valgrind
	valgrind \
		--leak-check=full \
		--show-leak-kinds=definite,indirect \
		--errors-for-leak-kinds=definite,indirect \
		--track-origins=yes \
		--track-fds=yes \
		--error-exitcode=101 \
		--log-file="$audit_output/valgrind-core.log" \
		"$build_dir/tests/test_core"

	gui_memcheck_options=(
		--leak-check=full
		'--show-leak-kinds=definite,indirect'
		'--errors-for-leak-kinds=none'
		--track-origins=yes
		--trace-children=no
		--error-exitcode=101
		--suppressions=/usr/share/glib-2.0/valgrind/glib.supp
		--suppressions=/usr/share/gtk-3.0/valgrind/gtk.supp
	)
	run_gui valgrind env \
		NO_AT_BRIDGE=1 G_SLICE=always-malloc G_DEBUG=gc-friendly \
		valgrind "${gui_memcheck_options[@]}" \
		--log-file="$audit_output/valgrind.%p.log" \
		"$build_dir/src/tpad" "$repo_dir/README.md"
	run_workflows valgrind env \
		NO_AT_BRIDGE=1 G_SLICE=always-malloc G_DEBUG=gc-friendly \
		valgrind "${gui_memcheck_options[@]}" \
		--log-file="$audit_output/valgrind-workflows.%p.log" \
		"$build_dir/src/tpad"
	run_print_export valgrind env \
		NO_AT_BRIDGE=1 G_SLICE=always-malloc G_DEBUG=gc-friendly \
		valgrind "${gui_memcheck_options[@]}" \
		--log-file="$audit_output/valgrind-print.%p.log" \
		"$build_dir/tests/test_print_export"
	fail_on_pattern "$audit_output/valgrind-core.log" \
		'definitely lost: [1-9]|indirectly lost: [1-9]|ERROR SUMMARY: [1-9]' \
		'valgrind-core-findings.txt'
	fail_on_pattern "$audit_output" 'ERROR SUMMARY: [1-9]' \
		'valgrind-findings.txt'
}

run_fuzz() {
	local build_dir
	local fuzz_binary
	local fuzz_seconds
	local -a package_cflags
	local -a package_libraries

	build_dir=$(configure_build fuzz '-O1 -g3' '')
	read -r -a package_cflags <<< "$(pkg-config --cflags gtk+-3.0 gtksourceview-3.0 gtkspell3-3.0 glib-2.0 libsystemd)"
	read -r -a package_libraries <<< "$(pkg-config --libs gtk+-3.0 gtksourceview-3.0 gtkspell3-3.0 glib-2.0 libsystemd)"
	fuzz_binary="$build_dir/fuzz_core"

	/usr/bin/clang-23 -std=gnu99 -O1 -g3 \
		-fno-omit-frame-pointer \
		-fsanitize=fuzzer,address,undefined \
		-fsanitize-coverage=trace-cmp,indirect-calls \
		-I"$repo_dir/src" -I"$build_dir" \
		"${package_cflags[@]}" \
		"$repo_dir/tests/fuzz_core.c" \
		"$repo_dir/tests/test_support.c" \
		"$repo_dir/src/tpad_find_and_replace.c" \
		"$repo_dir/src/tpad_string.c" \
		"$repo_dir/src/tpad_bom8.c" \
		"$repo_dir/src/tpad_hash.c" \
		"$repo_dir/src/base64.c" \
		"$repo_dir/src/md5.c" \
		"$repo_dir/src/sha256.c" \
		"$repo_dir/src/sha512.c" \
		"$repo_dir/src/platform_util.c" \
		"${package_libraries[@]}" -lpthread -lm \
		-o "$fuzz_binary" 2>&1 | tee "$audit_output/fuzz-build.log"

	mkdir -p -- "$audit_output/fuzz-corpus" "$audit_output/fuzz-artifacts"
	fuzz_seconds=${TPAD_FUZZ_SECONDS:-60}
	ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer-23 \
	DEBUGINFOD_URLS='' \
	ASAN_OPTIONS='detect_leaks=1:halt_on_error=1' \
	UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1' \
		"$fuzz_binary" "$audit_output/fuzz-corpus" \
		-artifact_prefix="$audit_output/fuzz-artifacts/" \
		-max_len=4096 -timeout=10 -max_total_time="$fuzz_seconds" \
		-print_final_stats=1 2>&1 | tee "$audit_output/fuzz.log"
}

run_extras() {
	local build_dir
	local source_file
	local -a cppcheck_sources=()
	local status=0

	for tool in cppcheck flawfinder shellcheck; do
		if ! command -v "$tool" >/dev/null 2>&1; then
			printf 'error: %s is required for the extras audit\n' "$tool" >&2
			return 1
		fi
	done

	build_dir=$(configure_build extras '-O0 -g3' '')
	while IFS= read -r source_file; do
		cppcheck_sources+=("$repo_dir/src/$source_file")
	done < <(awk '
		/^tpad_SOURCES =/ { sources = 1; next }
		sources && /^[[:space:]]*$/ { exit }
		sources {
			gsub(/\\/, "")
			for (field = 1; field <= NF; field++)
				if ($field ~ /[.]c$/) print $field
		}
	' "$repo_dir/src/Makefile.am")
	cppcheck --enable=warning,performance,portability \
		--inconclusive --std=c99 --suppress=missingIncludeSystem \
		--suppress=unusedFunction --error-exitcode=1 --library=posix \
		-D'GTK_CHECK_VERSION(a,b,c)=1' \
		-DHAVE_CONFIG_H=1 -I"$build_dir" -I"$repo_dir/src" \
		"${cppcheck_sources[@]}" 2>&1 |
		tee "$audit_output/cppcheck.log" || status=1
	flawfinder --minlevel=3 --columns "${cppcheck_sources[@]}" 2>&1 |
		tee "$audit_output/flawfinder.log"
	shellcheck -x "$repo_dir/autogen.sh" "$repo_dir/prepare-release.sh" \
		"$repo_dir/tools/gui-smoke.sh" "$repo_dir/tools/gui-workflows-smoke.sh" \
		"$repo_dir/tools/print-export-smoke.sh" \
		"$repo_dir/tools/release-audit.sh" 2>&1 |
		tee "$audit_output/shellcheck.log" || status=1
	return "$status"
}

if [[ $audit_mode == all ]]; then
	run_build
	run_analyzer
	run_asan
	run_tsan
	run_tysan
	run_valgrind
	run_fuzz
	run_extras
else
	"run_$audit_mode"
fi

printf 'Audit mode %s completed. Logs: %s\n' "$audit_mode" "$audit_output"
