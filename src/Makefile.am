## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(TPAD_CFLAGS) \
	$(SYSTEMD_CFLAGS)

AM_CFLAGS = -Wall -Wextra


bin_PROGRAMS = tpad

tpad_SOURCES = \
	def.h \
	error_warn.c \
	error_warn.h \
	include.h \
	main.c \
	main.h \
	tpad_main.c \
	tpad_main.h \
	tpad_id.c \
	tpad_id.h \
	malloc.c \
	malloc.h \
	tpad_config.c \
	tpad_config.h \
	tpad_quit.c \
	tpad_quit.h \
	tpad_bom8.c \
	tpad_bom8.h \
	tpad_codeset.c \
	tpad_codeset.h \
	tpad_doc.c \
	tpad_doc.h \
	tpad_copy.c \
	tpad_copy.h \
	tpad_frequency_analysis.h \
	tpad_frequency_analysis.c \
	tpad_control.c \
	tpad_control.h \
	tpad_ui.c \
	tpad_ui.h \
	tpad_reload.c \
	tpad_reload.h \
	tpad_unity.c \
	tpad_unity.h \
	tpad_find.c \
	tpad_find.h \
	tpad_find_and_replace.c \
	tpad_find_and_replace.h \
	tpad_show_file.c \
	tpad_show_file.h \
	tpad_buff.c \
	tpad_buff.h \
	tpad_title.c \
	tpad_title.h \
	tpad_toggle.c \
	tpad_toggle.h \
	tpad_hconnect.c \
	tpad_hconnect.h \
	tpad_about.c \
	tpad_about.h \
	tpad_history.c \
	tpad_history.h \
	tpad_font.c \
	tpad_font.h \
	tpad_lang.c \
	tpad_lang.h \
	tpad_spelling.c \
	tpad_spelling.h \
	tpad_file.c \
	tpad_file.h \
	tpad_new_file.c \
	tpad_new_file.h \
	tpad_open_file.c \
	tpad_open_file.h \
	tpad_fp.c \
	tpad_fp.h \
	tpad_headers.h \
	tpad_logo.h \
	tpad_open_guard.c \
	tpad_open_guard.h \
	tpad_pref.c \
	tpad_pref.h \
	tpad_print.c \
	tpad_print.h \
	tpad_save.c \
	tpad_save.h \
	tpad_tree.c \
	tpad_tree.h \
	tpad_spawn.c \
	tpad_spawn.h \
	tpad_string.c \
	tpad_string.h \
	tpad_textstats.c \
	tpad_textstats.h \
	tpad_hash.h \
	tpad_hash.c \
	mbedtls/check_config.h \
	mbedtls/config.h \
	mbedtls/base64.h \
	mbedtls/platform.h \
	mbedtls/md5.h \
	mbedtls/sha256.h \
	mbedtls/sha512.h \
	mbedtls/platform_util.h \
	mbedtls/platform_time.h \
	base64.c \
	md5.c \
	sha256.c \
	sha512.c \
	platform_util.c

tpad_CFLAGS = \
	-D_XOPEN_SOURCE=700 \
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

tpad_LDADD = $(TPAD_LIBS) $(SYSTEMD_LIBS)

EXTRA_DIST = \
	mbedtls/LICENSE \
	mbedtls/README.tpad \
	mbedtls/config.h.orig.tar.xz \
	tpad_watch.c \
	tpad_watch.h \
	tpad_modbal.c \
	tpad_modbal.h \
	tpad_magic.c \
	tpad_magic.h
