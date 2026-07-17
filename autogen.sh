#!/bin/sh

# Regenerate tpad's Autotools build system and, unless NOCONFIGURE is set,
# configure the source tree with any arguments supplied by the caller.

set -eu

srcdir=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
cd -- "$srcdir"

if [ ! -f configure.ac ]; then
	printf 'error: %s does not contain configure.ac\n' "$srcdir" >&2
	exit 1
fi

required_tools='autoconf autoheader automake aclocal glib-gettextize intltoolize libtoolize'
for tool in $required_tools; do
	if ! command -v "$tool" >/dev/null 2>&1; then
		printf 'error: required build tool is missing: %s\n' "$tool" >&2
		exit 1
	fi
done

if [ -n "${GNOME2_DIR:-}" ]; then
	ACLOCAL_FLAGS="-I $GNOME2_DIR/share/aclocal ${ACLOCAL_FLAGS:-}"
	LD_LIBRARY_PATH="$GNOME2_DIR/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
	PATH="$GNOME2_DIR/bin:$PATH"
	export ACLOCAL_FLAGS LD_LIBRARY_PATH PATH
fi

printf 'processing %s\n' "$srcdir"
printf 'Running glib-gettextize... Ignore non-fatal advisory messages.\n'
printf 'no\n' | glib-gettextize --force --copy

printf 'Running intltoolize...\n'
intltoolize --copy --force --automake

if [ -z "${NO_LIBTOOLIZE:-}" ]; then
	printf 'Running libtoolize...\n'
	libtoolize --force --copy
fi

printf 'Running aclocal %s...\n' "${ACLOCAL_FLAGS:-}"
# ACLOCAL_FLAGS is intentionally a caller-provided list of command arguments.
# shellcheck disable=SC2086
aclocal ${ACLOCAL_FLAGS:-}

printf 'Running autoheader...\n'
autoheader

printf 'Running automake --gnu...\n'
automake --add-missing --copy --gnu

# Automake emits trailing blanks in a handful of generated variable rules.
# Normalize them so regeneration stays byte-for-byte clean in Git.
for generated_makefile in Makefile.in */Makefile.in; do
	if [ -f "$generated_makefile" ]; then
		sed -i 's/[[:blank:]]\+$//' "$generated_makefile"
	fi
done

printf 'Running autoconf...\n'
autoconf

if [ -z "${NOCONFIGURE:-}" ]; then
	printf 'Running %s/configure...\n' "$srcdir"
	"$srcdir/configure" "$@"
	printf 'Configuration complete; run make to compile.\n'
else
	printf 'Skipping configure because NOCONFIGURE is set.\n'
fi
