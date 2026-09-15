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


void tpad_set_language_highlighting(gboolean enabled){
	cfg_set_show_lang((int) enabled);
	gtk_source_buffer_set_highlight_matching_brackets(mBuff, enabled);
	set_language();
	cfg_save();
}

void set_language(void){
	if(cfg_lang()){
				GtkSourceLanguageManager *lm;
				lm=gtk_source_language_manager_get_default();
				char* currentfile=(char*)tpad_fp_get_current();
					if(currentfile!=NULL){
					gboolean result_uncertain;
					gchar *content_type;
					content_type = g_content_type_guess (currentfile, NULL, 0, &result_uncertain);
					if (result_uncertain){
						g_free (content_type);
						content_type = NULL;
						}
					GtkSourceLanguage *lang = NULL;
					lang = gtk_source_language_manager_guess_language (lm, currentfile, content_type);
					gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(mBuff), lang);
					gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER(mBuff), (gboolean) cfg_lang());
					if(cfg_lang()) (lang) ? gtk_source_buffer_set_highlight_syntax(GTK_SOURCE_BUFFER(mBuff),TRUE) : gtk_source_buffer_set_highlight_syntax(GTK_SOURCE_BUFFER(mBuff),FALSE);
					if(content_type) g_free (content_type);
						g_free(currentfile);
				}
				else {
					gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(mBuff), NULL);
					gtk_source_buffer_set_highlight_syntax(GTK_SOURCE_BUFFER(mBuff), FALSE);
				}
		}
	else {
		gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(mBuff),NULL);
		gtk_source_buffer_set_highlight_syntax(GTK_SOURCE_BUFFER(mBuff),FALSE); 
	     }
	if (cfg_undo() >= 0 && cfg_undo() <= UNDO_MAX)
		gtk_text_buffer_set_max_undo_levels(GTK_TEXT_BUFFER(mBuff),
		                                    (guint) cfg_undo());
	else {
		/* GTK 4 uses zero, rather than -1, for unlimited undo. */
		gtk_text_buffer_set_max_undo_levels(GTK_TEXT_BUFFER(mBuff), 0);
		cfg_set_undo(0);
	}
		(cfg_line_wrap()) ? gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view),GTK_WRAP_WORD) : gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view),GTK_WRAP_NONE);
}
