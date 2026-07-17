/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013, 2014, 2015, 2016, 2017 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_new_file.c , is part of tpad.
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
extern gboolean save_locked;
extern GtkSourceBuffer *mBuff;
extern GtkSourceView *view;
extern gchar *origfile;
extern int madetemp;
void new_file(void){

    if(save_modified()) {
        gtk_source_buffer_begin_not_undoable_action(GTK_SOURCE_BUFFER(mBuff));
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(mBuff),"",-1);
        gtk_source_buffer_end_not_undoable_action(GTK_SOURCE_BUFFER(mBuff));
	        gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(mBuff),FALSE);
		tpad_open_guard_cleanup();
		madetemp = 0;
		g_clear_pointer(&origfile, g_free);
		tpad_fp_cleanup();
		tpad_control_store_hash_of_current_file_set();
		cfg_set_use_ut8bom(FALSE);
		set_language();
	        gtk_window_set_title(GTK_WINDOW(window),_FALLBACK_SAVE_FILE_NAME);
		save_locked=FALSE;
    }

}
