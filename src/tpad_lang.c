/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_lang.c , is part of tpad.
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
extern GtkSourceView *view;


void toggle_show_lang(GtkWidget *caller){
    cfg_set_show_lang((int)gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(caller)));
	gtk_source_buffer_set_highlight_matching_brackets (mBuff, gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(caller)));
	set_language();
	cfg_save();
}

void set_language(){
	if(cfg_lang()){
			GtkSourceLanguageManager *lm;
			lm=gtk_source_language_manager_new();
			char* currentfile=(char*)tpad_fp_get_current();
			if(currentfile!=NULL){
				long unsigned int i= 0,pos= 0;
				size_t mlewn = sizeof(currentfile)/sizeof(currentfile[1]);
				size_t blew = strlen(currentfile) + 1;

				if(blew > mlewn) mlewn = blew;

				 char fil[mlewn];

    			for(i=0; currentfile[i]; i++){
        			if(currentfile[i]=='\\') fil[pos++]='\\';
        			fil[pos++]=currentfile[i];
    				}
    			fil[pos]=0;
				gboolean result_uncertain;
				gchar *content_type;
				content_type = g_content_type_guess (fil, NULL, 0, &result_uncertain);
				if (result_uncertain){
					g_free (content_type);
					content_type = NULL;
					}
				GtkSourceLanguage *lang = NULL;
				lang = gtk_source_language_manager_guess_language (lm, fil, content_type);
				gtk_source_buffer_set_language (GTK_SOURCE_BUFFER(mBuff),GTK_SOURCE_LANGUAGE(lang));
				gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER(mBuff), (gboolean) cfg_lang());
				if(cfg_lang()) (lang) ? gtk_source_buffer_set_highlight_syntax(GTK_SOURCE_BUFFER(mBuff),TRUE) : gtk_source_buffer_set_highlight_syntax(GTK_SOURCE_BUFFER(mBuff),FALSE);	
				if(content_type) g_free (content_type);
		}
	}
	else {
		gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(mBuff),NULL);
		gtk_source_buffer_set_highlight_syntax(GTK_SOURCE_BUFFER(mBuff),FALSE); 
	     }
	if(cfg_undo() >= 1 && cfg_undo() <= UNDO_MAX) gtk_source_buffer_set_max_undo_levels ((GtkSourceBuffer *)mBuff,(gint)cfg_undo());
	else {
		gtk_source_buffer_set_max_undo_levels ((GtkSourceBuffer *)mBuff,(gint)-1);
		cfg_set_undo(0);
	}
		(cfg_line_wrap()) ? gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view),GTK_WRAP_WORD) : gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view),GTK_WRAP_NONE);
}