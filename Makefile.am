## Process this file with automake to produce Makefile.in
## Created by Anjuta
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
SUBDIRS = src po icon man script debian m4

tpaddocdir = ${prefix}/share/doc/tpad
tpaddoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = $(tpaddoc_DATA) \
	$(INTLTOOL_FILES) \
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
	po/.intltool-merge-cache


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(tpaddocdir)
