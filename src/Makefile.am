## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(TPAD_FRONTEND_CPPFLAGS) \
	$(TPAD_CFLAGS) \
	$(SYSTEMD_CFLAGS)

AM_CFLAGS = -Wall -Wextra


bin_PROGRAMS = tpad

tpad_SOURCES = \
	tpad_backend.h \
	error_warn.h \
	main.c \
	main.h \
	tpad_main.h \
	tpad_id.c \
	tpad_id.h \
	malloc.c \
	malloc.h \
	tpad_config.c \
	tpad_config.h \
	tpad_quit.h \
	tpad_bom8.c \
	tpad_bom8.h \
	tpad_codeset.c \
	tpad_codeset.h \
	tpad_doc.h \
	tpad_copy.c \
	tpad_copy.h \
	tpad_frequency_analysis.h \
	tpad_control.c \
	tpad_control.h \
	tpad_reload.c \
	tpad_reload.h \
	tpad_unity.c \
	tpad_unity.h \
	tpad_find.h \
	tpad_find_and_replace.h \
	tpad_show_file.h \
	tpad_buff.h \
	tpad_title.c \
	tpad_title.h \
	tpad_hconnect.h \
	tpad_about.h \
	tpad_history.h \
	tpad_font.h \
	tpad_file.h \
	tpad_new_file.h \
	tpad_open_file.h \
	tpad_fp.c \
	tpad_fp.h \
	tpad_headers.h \
	tpad_logo.h \
	tpad_open_guard.c \
	tpad_open_guard.h \
	tpad_recent.c \
	tpad_recent.h \
	tpad_pref.h \
	tpad_print.h \
	tpad_save.h \
	tpad_tree.c \
	tpad_tree.h \
	tpad_spawn.c \
	tpad_string.c \
	tpad_string.h \
	tpad_textstats.h \
	tpad_hash.h \
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

if USE_GTK3
tpad_SOURCES += \
	gtk3/def.h \
	gtk3/error_warn.c \
	gtk3/include.h \
	gtk3/tpad_about.c \
	gtk3/tpad_buff.c \
	gtk3/tpad_doc.c \
	gtk3/tpad_file.c \
	gtk3/tpad_find.c \
	gtk3/tpad_find_and_replace.c \
	gtk3/tpad_font.c \
	gtk3/tpad_frequency_analysis.c \
	gtk3/tpad_hash.c \
	gtk3/tpad_hconnect.c \
	gtk3/tpad_headers.h \
	gtk3/tpad_history.c \
	gtk3/tpad_lang.c \
	gtk3/tpad_lang.h \
	gtk3/tpad_main.c \
	gtk3/tpad_new_file.c \
	gtk3/tpad_open_file.c \
	gtk3/tpad_pref.c \
	gtk3/tpad_print.c \
	gtk3/tpad_quit.c \
	gtk3/tpad_save.c \
	gtk3/tpad_show_file.c \
	gtk3/tpad_spawn.h \
	gtk3/tpad_spelling.c \
	gtk3/tpad_spelling.h \
	gtk3/tpad_textstats.c \
	gtk3/tpad_toggle.c \
	gtk3/tpad_toggle.h \
	gtk3/tpad_ui.c \
	gtk3/tpad_ui.h
else
tpad_SOURCES += \
	def.h \
	error_warn.c \
	include.h \
	tpad_gtk.c \
	tpad_gtk.h \
	tpad_about.c \
	tpad_buff.c \
	tpad_doc.c \
	tpad_file.c \
	tpad_find.c \
	tpad_find_and_replace.c \
	tpad_font.c \
	tpad_frequency_analysis.c \
	tpad_hash.c \
	tpad_hconnect.c \
	tpad_history.c \
	tpad_lang.c \
	tpad_lang.h \
	tpad_main.c \
	tpad_new_file.c \
	tpad_open_file.c \
	tpad_pref.c \
	tpad_print.c \
	tpad_quit.c \
	tpad_save.c \
	tpad_show_file.c \
	tpad_spawn.h \
	tpad_spelling.c \
	tpad_spelling.h \
	tpad_textstats.c \
	tpad_toggle.c \
	tpad_toggle.h \
	tpad_ui.c \
	tpad_ui.h
endif

tpad_CFLAGS = \
	-D_XOPEN_SOURCE=700 \
	-std=gnu11 \
	-Wall \
	-Wextra \
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
