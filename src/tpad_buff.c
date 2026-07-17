/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014-2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_buff.c , is part of tpad.
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

extern GtkWidget *window;
extern GtkSourceBuffer *mBuff;
extern GtkSourceView *view;

unsigned int tpad_buff_add(void){
	return 1;
}
GtkSourceView *tpad_buff_get_view(unsigned int key){
	(void) key;
	return view;
}

GtkSourceBuffer  *tpad_buff_get_buff(unsigned int key){
	(void) key;
	tpad_clean_mbuff();
	return mBuff;
}
void set_buffer(void){

	if (content == NULL)
		return;

		    gtk_source_buffer_begin_not_undoable_action(GTK_SOURCE_BUFFER(mBuff));
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(mBuff),content,-1);
        gtk_source_buffer_end_not_undoable_action(GTK_SOURCE_BUFFER(mBuff));
        gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(mBuff),FALSE);
        GtkTextIter iter;
        gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(mBuff),&iter);
        gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(mBuff),&iter);
	g_clear_pointer(&content, g_free);
	tpad_clean_mbuff();
}

gboolean tpad_clean_mbuff(void) {
GtkTextIter start,end;

gtk_text_buffer_get_bounds( GTK_TEXT_BUFFER(mBuff),&start,&end);

	gchar *data_to_convert = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),
	                                                 &start, &end, FALSE);
	gchar *converted;

if (data_to_convert == NULL) return(FALSE);

	converted = tpad_codeset_convert_to_curent_local_from_utf8(
	                    data_to_convert, strlen(data_to_convert));
	if (converted == NULL) {
		g_free(data_to_convert);
		return FALSE;
	}

gtk_source_buffer_begin_not_undoable_action(GTK_SOURCE_BUFFER(mBuff));

gtk_text_buffer_set_text(GTK_TEXT_BUFFER(mBuff), converted, -1);
	 gtk_source_buffer_end_not_undoable_action(GTK_SOURCE_BUFFER(mBuff));
	g_free(converted);
	g_free(data_to_convert);
	return(TRUE);
}
