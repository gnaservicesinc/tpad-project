/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014, 2015, 2016 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_find_and_replace.c , is part of tpad.
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
extern gboolean searchCase;
extern gboolean doCOVT;
extern unsigned int sdone;
extern GtkWidget *findentry,*replaceentry;
extern GtkTextIter match_start,match_end;

int opt_find_replace(void){
	    const gchar *old;
	    const gchar *new;
		gchar *old_converted = NULL;
		gchar *new_converted = NULL;
		gchar *source_text;

		if(doCOVT){
			old_converted = g_strcompress(gtk_entry_get_text(GTK_ENTRY(findentry)));
			new_converted = g_strcompress(gtk_entry_get_text(GTK_ENTRY(replaceentry)));
			old = old_converted;
			new = new_converted;
		}
		else {
			new=( const gchar *)gtk_entry_get_text(GTK_ENTRY(replaceentry));
			old=( const gchar *)gtk_entry_get_text(GTK_ENTRY(findentry));
			}

		if (old == NULL || old[0] == '\0') {
			g_free(old_converted);
			g_free(new_converted);
			return 0;
		}

		GtkTextMark *mark_start;
		GtkTextMark *mark_end;
		GtkTextIter start,end;
		gint hadsele=0;

	if(gtk_text_buffer_get_has_selection (GTK_TEXT_BUFFER(mBuff))){
		if(!gtk_text_buffer_get_selection_bounds( GTK_TEXT_BUFFER(mBuff),&start,&end)) {
			gtk_text_buffer_get_bounds( GTK_TEXT_BUFFER(mBuff),&start,&end);
			hadsele=0;
			}
		else hadsele=1;

	}
	else{
	 gtk_text_buffer_get_bounds( GTK_TEXT_BUFFER(mBuff),&start,&end);
	hadsele=0;
	}

		source_text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),
		                                         &start, &end, FALSE);
		content = tpad_replace_str_full(source_text, old, new, searchCase);
		g_free(source_text);
		g_free(old_converted);
		g_free(new_converted);

		if(content==NULL) return(0);

	gtk_text_buffer_begin_user_action(GTK_TEXT_BUFFER(mBuff));
	if(hadsele){

			mark_start = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(mBuff),
			                                         NULL, &start, TRUE);
			mark_end = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(mBuff),
			                                       NULL, &end, FALSE);
			gtk_text_buffer_delete (GTK_TEXT_BUFFER(mBuff), &start, &end);
			gtk_text_buffer_insert (GTK_TEXT_BUFFER(mBuff), &start, content, -1);
			gtk_text_buffer_end_user_action (GTK_TEXT_BUFFER(mBuff));
			gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(mBuff),&start);
					GtkTextIter mstart, mend;
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(mBuff),&mstart,mark_start);
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(mBuff),&mend,mark_end);
			gtk_text_buffer_select_range (GTK_TEXT_BUFFER(mBuff), &mstart, &mend);
			gtk_text_buffer_delete_mark (GTK_TEXT_BUFFER(mBuff), mark_start);
			gtk_text_buffer_delete_mark (GTK_TEXT_BUFFER(mBuff), mark_end);
	}
	else{
	gtk_text_buffer_delete(GTK_TEXT_BUFFER(mBuff), &start, &end);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(mBuff), &start, content, -1);
	gtk_text_buffer_end_user_action (GTK_TEXT_BUFFER(mBuff));
	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(mBuff),TRUE);

        GtkTextIter iter;
        gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(mBuff),&iter);
        gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(mBuff),&iter);
	}
	g_free(content);
	content = NULL;
	return(1);
}



char *tpad_replace_str_full(const char *str, const char *old, const char *new,
                           gboolean case_sensitive)
{
	GError *error = NULL;
	GRegex *regex;
	GRegexCompileFlags flags = G_REGEX_OPTIMIZE;
	gchar *escaped;
	gchar *result;

	if (str == NULL || old == NULL || new == NULL)
		return NULL;
	if (old[0] == '\0')
		return g_strdup(str);

	if (!case_sensitive)
		flags = (GRegexCompileFlags) (flags | G_REGEX_CASELESS);
	escaped = g_regex_escape_string(old, -1);
	regex = g_regex_new(escaped, flags, 0, &error);
	g_free(escaped);
	if (regex == NULL) {
		g_clear_error(&error);
		return NULL;
	}

	result = g_regex_replace_literal(regex, str, -1, 0, new, 0, &error);
	g_regex_unref(regex);
	if (error != NULL) {
		g_clear_error(&error);
		g_free(result);
		return NULL;
	}
	return result;
}

char *tpad_replace_str(const char *str, const char *old, const char *new)
{
	return tpad_replace_str_full(str, old, new, TRUE);
}
