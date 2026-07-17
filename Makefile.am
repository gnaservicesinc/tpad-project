## Process this file with automake to produce Makefile.in
## Created by Anjuta
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
SUBDIRS = src tests po icon man script debian m4

tpaddocdir = ${prefix}/share/doc/tpad
tpaddoc_DATA = \
	README.md\
	COPYING\
	debian/copyright\
	AUTHORS

mbedtlslicensedir = $(tpaddocdir)/mbedtls
mbedtlslicense_DATA = src/mbedtls/LICENSE


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = $(tpaddoc_DATA) \
	$(INTLTOOL_FILES) \
	RELEASE_TESTING.md \
	AGENTS.md \
	autogen.sh \
	prepare-release.sh \
	tools/asan-system.supp \
	tools/gui-smoke.sh \
	tools/gui-workflows-smoke.sh \
	tools/print-export-smoke.sh \
	tools/release-audit.sh \
	snap/snapcraft.yaml \
	po/de.po \
	po/el.po \
	po/en.po \
	po/en_US.po \
	po/br.po \
	po/en_GB.po \
	po/es.po \
	po/fr.po \
	po/ko.po \
	po/si.po \
	po/fr_FR.po \
	po/template.pot \
	po/tr.po

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache \
	po/.intltool-merge-cache.lock


# Remove doc directory on uninstall
uninstall-local:
	-rm -r "$(DESTDIR)$(tpaddocdir)"
