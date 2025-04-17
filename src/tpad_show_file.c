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
extern gchar *unknownContents;
extern gchar* origfile;
extern GtkSourceBuffer *mBuff;
extern GtkSourceView *view;
extern gchar *content;
extern int tpad_wach_thread_set;
extern GtkSpellChecker* doc_spelling;
extern GdkDisplay* Display;

int show_file(gchar *nfile){
	tpad_touch_check_file(nfile);

	static gchar *newfile=NULL;


	newfile= g_strdup(check_file(nfile));

	if (newfile == NULL) return(1);

	tpad_fp_set(newfile);
	tpad_control_store_hash_of_current_file_set();
	//tpad_control_on_open();

	if(cfg_use_open_guard()){
		//fprintf(stdout,"Trying to set tpad fp to newfile. newfile == %s\n",newfile);
		//tpad_fp_set( (void*) newfile);
		//fprintf(stdout,"Set tpad fp to newfile, tpad_fp_get returns %s\n",tpad_fp_get_current());
		int openguard_debug = (int) tpad_open_guard_check_ok();
		//fprintf(stdout,"Open guard returned %i\n",openguard_debug);
		if(!openguard_debug) {

		return(1);

		}
		else{
			madetemp=1;
			origfile=g_strdup((gchar*) newfile);
			}
	}

	gsize length;
        GError *err = NULL;
	content=NULL;
	unknownContents=NULL;
	if(g_file_get_contents (newfile, &unknownContents, &length,&err))
	{
		if (err != NULL){
			gerror_warn(err->message,_CAN_NOT_READ_FILE,TRUE,TRUE);
			g_error_free (err);

		}
		else {
		// If utmbom option is not already set (forced by preference
		int ibomdetected = (int) tpad_string_has_bom8(&unknownContents);
		cfg_set_use_ut8bom(ibomdetected);
		gtk_source_buffer_begin_not_undoable_action(GTK_SOURCE_BUFFER(mBuff));
		cfg_set_use_ut8bom (tpad_string_has_bom8(&unknownContents));	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(mBuff),tpad_codeset_convert_to_utf8_from_current_local(unknownContents, length),-1);
	 gtk_source_buffer_end_not_undoable_action(GTK_SOURCE_BUFFER(mBuff));
	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(mBuff),FALSE);
        GtkTextIter iter;
        gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(mBuff),&iter);
        gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(mBuff),&iter);

		g_free(unknownContents);
		content = NULL;
		//set_buffer();
		set_title();
		set_language();
		tpad_control_store_hash_of_current_file_set();
		//if(cfg_spell()) toggle_spelling();

		}
   	}
}
