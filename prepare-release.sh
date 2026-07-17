#!/usr/bin/env bash

# Prepare a tpad release by updating every authoritative version field,
# prepending a Debian changelog entry, regenerating Autotools output, and
# returning the source tree to a clean (unconfigured) build state.

set -euo pipefail

usage() {
	printf 'Usage: %s VERSION [CHANGE ...]\n' "${0##*/}" >&2
	printf 'Example: %s 7.1.0.0 "Add system printing" "Refresh Snap packaging"\n' \
		"${0##*/}" >&2
}

if (( $# < 1 )); then
	usage
	exit 2
fi

release_version=$1
shift

if [[ ! $release_version =~ ^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
	printf 'error: VERSION must contain four numeric components (for example, 7.1.0.0)\n' >&2
	exit 2
fi

package_version=$release_version
script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
cd -- "$script_dir"

for required_file in configure.ac debian/changelog snap/snapcraft.yaml autogen.sh; do
	if [[ ! -f $required_file ]]; then
		printf 'error: expected %s in %s\n' "$required_file" "$script_dir" >&2
		exit 1
	fi
done

if (( $# == 0 )); then
	set -- "Prepare release ${release_version}."
fi

current_version=$(dpkg-parsechangelog -S Version)
if [[ $current_version != "$package_version" ]]; then
	maintainer=$(sed -n 's/^ -- \(.*>\)  [A-Z][a-z][a-z], .*/\1/p' debian/changelog | head -n 1)
	distribution=$(sed -n '1s/^tpad ([^)]*) \([^;[:space:]]*\);.*/\1/p' debian/changelog)
	if [[ -z $maintainer || -z $distribution ]]; then
		printf 'error: unable to read maintainer or distribution from debian/changelog\n' >&2
		exit 1
	fi

	release_stamp=$(LC_ALL=C date -R)
	changelog_tmp=$(mktemp "${TMPDIR:-/tmp}/tpad-changelog.XXXXXX")
	trap 'rm -f -- "$changelog_tmp"' EXIT
	chmod --reference=debian/changelog "$changelog_tmp"

	{
		printf 'tpad (%s) %s; urgency=high\n\n' "$package_version" "$distribution"
		for change in "$@"; do
			printf '  * %s\n' "$change"
		done
		printf '\n -- %s  %s\n\n' "$maintainer" "$release_stamp"
		awk 'BEGIN { copying = 0 } /^tpad \(/ { copying = 1 } copying { print }' debian/changelog
	} > "$changelog_tmp"
	mv -- "$changelog_tmp" debian/changelog
	trap - EXIT
else
	printf 'Changelog already starts with %s; resuming regeneration.\n' \
		"$package_version"
fi

TPAD_RELEASE_VERSION=$package_version perl -0pi -e '
	BEGIN { $changed = 0 }
	$changed += s/^AC_INIT\(tpad, [^,]+,/AC_INIT(tpad, $ENV{TPAD_RELEASE_VERSION},/m;
	END { die "error: expected one version field in configure.ac\n" if $changed != 1 }
' configure.ac

TPAD_RELEASE_VERSION=$package_version perl -0pi -e '
	BEGIN { $changed = 0 }
	$changed += s/^version:.*$/version: '\''$ENV{TPAD_RELEASE_VERSION}'\''/m;
	END { die "error: expected one version field in snap\/snapcraft.yaml\n" if $changed != 1 }
' snap/snapcraft.yaml

printf 'Updated tpad to %s in Debian, Autotools, and Snap metadata.\n' \
	"$release_version"
printf 'Regenerating Autotools files...\n'
env -u NOCONFIGURE ./autogen.sh
make distclean
printf 'Release metadata regenerated and the source tree is distclean.\n'
