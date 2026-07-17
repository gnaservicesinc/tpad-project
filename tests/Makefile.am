AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_builddir) \
	$(TPAD_CFLAGS) \
	$(SYSTEMD_CFLAGS)

AM_CFLAGS = \
	-std=gnu99 \
	-Wall \
	-Wextra \
	-Werror \
	-Wformat=2 \
	-Wstrict-prototypes \
	-Wmissing-prototypes \
	-Wshadow \
	-Wpointer-arith \
	-Wwrite-strings

check_PROGRAMS = test_core test_print_export
TESTS = test_core

test_core_SOURCES = \
	test_core.c \
	test_support.c \
	$(top_srcdir)/src/tpad_find_and_replace.c \
	$(top_srcdir)/src/tpad_string.c \
	$(top_srcdir)/src/tpad_bom8.c \
	$(top_srcdir)/src/tpad_copy.c \
	$(top_srcdir)/src/tpad_open_guard.c \
	$(top_srcdir)/src/tpad_hash.c \
	$(top_srcdir)/src/base64.c \
	$(top_srcdir)/src/md5.c \
	$(top_srcdir)/src/sha256.c \
	$(top_srcdir)/src/sha512.c \
	$(top_srcdir)/src/platform_util.c

test_core_LDADD = $(TPAD_LIBS) $(SYSTEMD_LIBS)

test_print_export_SOURCES = \
	test_print_export.c \
	test_support.c \
	$(top_srcdir)/src/tpad_print.c

test_print_export_LDADD = $(TPAD_LIBS) $(SYSTEMD_LIBS)

EXTRA_DIST = fuzz_core.c
