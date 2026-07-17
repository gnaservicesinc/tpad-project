/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014, 2015, 2016, 2017 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_show_file.c , is part of tpad.
 *
 *   tpad is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *   tpad is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with tpad.  If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************************/
#include "tpad_headers.h"
extern int setModified;
extern int madetemp;
extern gchar* origfile;
extern GtkSourceBuffer *mBuff;
extern GtkSourceView *view;
extern gchar *content;
extern int tpad_wach_thread_set;
extern GtkSpellChecker* doc_spelling;
extern GdkDisplay* Display;

static void clear_open_guard_state(void)
{
	tpad_open_guard_cleanup();
	madetemp = 0;
	g_clear_pointer(&origfile, g_free);
}

int show_file(gchar *nfile){
	gchar *newfile;
	gchar *unknown_contents = NULL;
	gchar *converted;
	gsize length;
	GError *err = NULL;
	gint bom_detected;
	gboolean guard_tracked = FALSE;

	if (tpad_touch_check_file(nfile) < 0)
		return 1;

	newfile = check_file(nfile);
	if (newfile == NULL)
		return 1;

	/* Read and validate the candidate completely before changing the current
	 * path, guard, buffer, title, or BOM preference. */
	if (!g_file_get_contents(newfile, &unknown_contents, &length, &err)) {
		gerror_warn(err != NULL ? err->message : _CAN_NOT_READ_FILE,
		            _CAN_NOT_READ_FILE, TRUE, FALSE);
		g_clear_error(&err);
		g_free(newfile);
		return 1;
	}
	if (!tpad_string_is_text_data(unknown_contents, length)) {
		gerror_warn(_BINARY_FILE_UNSUPPORTED, _CAN_NOT_READ_FILE,
		            TRUE, FALSE);
		g_free(unknown_contents);
		g_free(newfile);
		return 1;
	}

	bom_detected = tpad_string_has_bom8_len(unknown_contents, length);
	if (bom_detected)
		converted = tpad_utf8_bom_decode(unknown_contents, length);
	else
		converted = tpad_codeset_convert_to_utf8_from_current_local(
		                unknown_contents, length);
	if (converted == NULL) {
		if (bom_detected)
			gerror_warn(_CONVERT_FAILED, _CONVERT_FAILED, TRUE, FALSE);
		g_free(unknown_contents);
		g_free(newfile);
		return 1;
	}
	if (cfg_use_open_guard() && !tpad_open_guard_check_path(newfile)) {
		g_free(converted);
		g_free(unknown_contents);
		g_free(newfile);
		return 1;
	}

	/* Commit the document transition only after every fallible preflight step
	 * above has succeeded. */
	clear_open_guard_state();
	tpad_fp_set(newfile);
	if (cfg_use_open_guard())
		guard_tracked = tpad_open_guard_track_path(newfile);
	madetemp = guard_tracked ? 1 : 0;
	if (guard_tracked)
		origfile = g_strdup(newfile);
	cfg_set_use_ut8bom(bom_detected);
	g_clear_pointer(&content, g_free);
	gtk_source_buffer_begin_not_undoable_action(GTK_SOURCE_BUFFER(mBuff));
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(mBuff), converted, -1);
	gtk_source_buffer_end_not_undoable_action(GTK_SOURCE_BUFFER(mBuff));
	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(mBuff),FALSE);
	GtkTextIter iter;
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(mBuff),&iter);
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(mBuff),&iter);

	set_title();
	set_language();
	tpad_control_store_hash_of_current_file_set();
	g_free(unknown_contents);
	g_free(converted);
	g_free(newfile);

	return 0;
}
