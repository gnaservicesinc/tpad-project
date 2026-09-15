/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013, 2014, 2015, 2016, 2017 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_file.c , is part of tpad.
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
//extern gchar *content;

static void tpad_do_file_open_dialog(void);


void open_file(void){
	 if(save_modified()) tpad_do_file_open_dialog();
}




static void tpad_do_file_open_dialog(void){
	GListModel *files = tpad_file_dialog_open_multiple(
		GTK_WINDOW(window), gettext("Open File"));
	gchar *current = tpad_fp_get_current();
	gboolean have_current = current != NULL;

	if (files == NULL) {
		g_free(current);
		return;
	}
	save_locked = FALSE;
	for (guint index = 0; index < g_list_model_get_n_items(files); index++) {
		GFile *file = g_list_model_get_item(files, index);
		gchar *filename = g_file_get_path(file);
		gchar *utf8_filename;

		g_object_unref(file);
		if (filename == NULL) {
			gerror_warn(_CAN_NOT_READ_FILE,
			            gettext("Only local files can be opened."), TRUE, FALSE);
			continue;
		}
		utf8_filename = tpad_filename_to_utf8(filename);
		g_free(filename);
		if (utf8_filename == NULL)
			continue;

		if (have_current)
			(void) new_thread_tpad(utf8_filename);
		else if (show_file(utf8_filename) == 0)
			have_current = TRUE;
		g_free(utf8_filename);
	}

	g_free(current);
	g_object_unref(files);
  }
