/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_doc.c , is part of tpad.
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

extern GtkSourceBuffer *mBuff;

void low_caps_document(void)
{
	(void) mod_doc(_METHOD_DOWNCAP);
}

void up_caps_document(void)
{
	(void) mod_doc(_METHOD_UPCAP);
}

void rev_document(void)
{
	(void) mod_doc(_METHOD_REVERSE);
}

void hex_document(void)
{
	(void) mod_doc(_METHOD_HEX);
}

gint mod_doc(int method)
{
	GtkTextIter start;
	GtkTextIter end;
	GtkTextIter selection_start;
	GtkTextMark *start_mark = NULL;
	gchar *source;
	gchar *modified = NULL;
	gboolean had_selection;

	if (method < 0 || method > _METHOD_MAX) {
		gerror_warn(_ERROR_MOD_DOC_METHOD_VAL, (gchar *) __func__, TRUE, FALSE);
		return -1;
	}

	had_selection = gtk_text_buffer_get_selection_bounds(
	                        GTK_TEXT_BUFFER(mBuff), &start, &end);
	if (!had_selection)
		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(mBuff), &start, &end);
	source = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),
	                                  &start, &end, FALSE);

	switch (method) {
	case _METHOD_REVERSE:
		modified = g_utf8_strreverse(source, -1);
		break;
	case _METHOD_UPCAP:
		modified = g_utf8_strup(source, -1);
		break;
	case _METHOD_DOWNCAP:
		modified = g_utf8_strdown(source, -1);
		break;
	default:
		break;
	}
	g_free(source);

	if (modified == NULL)
		return -1;

	gtk_text_buffer_begin_user_action(GTK_TEXT_BUFFER(mBuff));
	if (had_selection) {
		start_mark = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(mBuff),
		                                         NULL, &start, TRUE);
		gtk_text_buffer_delete(GTK_TEXT_BUFFER(mBuff), &start, &end);
		gtk_text_buffer_insert(GTK_TEXT_BUFFER(mBuff), &start, modified, -1);
		gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(mBuff),
		                                 &selection_start, start_mark);
		gtk_text_buffer_select_range(GTK_TEXT_BUFFER(mBuff),
		                             &selection_start, &start);
		gtk_text_buffer_delete_mark(GTK_TEXT_BUFFER(mBuff), start_mark);
	} else {
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(mBuff), modified, -1);
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(mBuff), &start);
		gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(mBuff), &start);
	}
	gtk_text_buffer_end_user_action(GTK_TEXT_BUFFER(mBuff));
	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(mBuff), TRUE);
	g_free(modified);
	return 1;
}
