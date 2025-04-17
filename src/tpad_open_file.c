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

gchar* unknownContents=NULL;


static void tpad_do_file_open_dialog();


void open_file(){  
	 if(save_modified()) tpad_do_file_open_dialog();
}




static void tpad_do_file_open_dialog(){
	gint response;
	
	GtkWidget *dialog=NULL;
  	gchar *tempFileName=NULL;
   
        dialog = gtk_file_chooser_dialog_new("Open File",GTK_WINDOW(window),GTK_FILE_CHOOSER_ACTION_OPEN,"Cancel",GTK_RESPONSE_CANCEL,"Open",GTK_RESPONSE_ACCEPT,NULL);
		gtk_file_chooser_set_show_hidden (GTK_FILE_CHOOSER(dialog),TRUE);

	gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dialog),TRUE);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog),TRUE);
	gtk_file_chooser_set_create_folders(GTK_FILE_CHOOSER(dialog),TRUE);


        response = gtk_dialog_run(GTK_DIALOG(dialog));

        if(response==GTK_RESPONSE_ACCEPT)
	{
	save_locked=FALSE;

	// Not actually using URIs. Set local only so changing this function to the filenames one for simplicity. 
	GSList * ReturnedURIS=(GSList *)gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER(dialog));

	gint iFileN=0;

	for(iFileN=0; tempFileName = (gchar*) g_slist_nth_data(ReturnedURIS,iFileN); iFileN++) {
		if(tpad_fp_get_current()) new_thread_tpad((gchar*) g_strdup(tempFileName));	
		else show_file((gchar*)g_strdup(tempFileName));	
	} // Close File List Loop.

		
	} // Close Response Accepted.
  // Close/Destroy dialog box
  if(dialog) gtk_widget_destroy(GTK_WIDGET(dialog));
  }

