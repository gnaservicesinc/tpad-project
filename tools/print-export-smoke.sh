#!/usr/bin/env bash

# Exercise GtkSourcePrintCompositor pagination and page drawing with the
# dedicated integration-test binary under Xvfb.

set -euo pipefail

if (( $# < 2 )); then
	printf 'Usage: %s OUTPUT.pdf COMMAND [ARG ...]\n' "${0##*/}" >&2
	exit 2
fi

output_file=$1
shift

for tool in xvfb-run timeout; do
	if ! command -v "$tool" >/dev/null 2>&1; then
		printf 'error: %s is required for the print export test\n' "$tool" >&2
		exit 1
	fi
done
if [[ $output_file != /* ]]; then
	printf 'error: OUTPUT.pdf must be an absolute path\n' >&2
	exit 2
fi

timeout 60s xvfb-run -a "$@" "$output_file"

if [[ ! -s $output_file ]]; then
	printf 'error: print export did not produce a PDF\n' >&2
	exit 1
fi
if [[ $(LC_ALL=C head -c 5 -- "$output_file") != '%PDF-' ]]; then
	printf 'error: print export does not have a PDF signature\n' >&2
	exit 1
fi
