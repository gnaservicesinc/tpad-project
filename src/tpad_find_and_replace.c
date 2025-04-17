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

int opt_find_replace(){
    const gchar *old;
    const gchar *new;
	gchar* unknownContents;
	unknownContents=NULL;
	int hex;

	if(doCOVT){
		old=(const gchar *) g_strcompress(gtk_entry_get_text(GTK_ENTRY(findentry)));
		new=(const gchar *) g_strcompress(gtk_entry_get_text(GTK_ENTRY(replaceentry)));
	}
	else {
		new=( const gchar *)gtk_entry_get_text(GTK_ENTRY(replaceentry));
		old=( const gchar *)gtk_entry_get_text(GTK_ENTRY(findentry));
		}


	GtkTextMark *  mark_start = (GtkTextMark *) gtk_text_mark_new ("mStart",TRUE);
	GtkTextMark *  mark_end = (GtkTextMark *) gtk_text_mark_new ("mEnd",FALSE);
	GtkTextIter start,end;
	gchar *RevBuff;
	gint failchck=0;
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
	
	 content=tpad_replace_str((const char*)gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),&start,&end,FALSE),(const char*)old,(const char*)new); 

	if(content==NULL) return(0);

	gtk_text_buffer_begin_user_action(GTK_TEXT_BUFFER(mBuff));	
	if(hadsele){

			gtk_text_buffer_add_mark  (GTK_TEXT_BUFFER(mBuff),mark_start,&start);
			gtk_text_buffer_add_mark  (GTK_TEXT_BUFFER(mBuff),mark_end,&end);
			gtk_text_buffer_delete (GTK_TEXT_BUFFER(mBuff), &start, &end);
	 		gtk_text_buffer_insert (GTK_TEXT_BUFFER(mBuff), &start, g_strdup(content), -1);
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

	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(mBuff),content,-1);
	gtk_text_buffer_end_user_action (GTK_TEXT_BUFFER(mBuff));
	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(mBuff),TRUE);

        GtkTextIter iter;
        gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(mBuff),&iter);
        gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(mBuff),&iter);  
	}  
	g_free(content);
	return(1);
}



char *tpad_replace_str(const char *str, const char *old, const char *new)
{
	if(str == NULL || old == NULL || new == NULL) return((char *)str);

	char *ret, *r;
	const char *p, *q;
	size_t oldlen = (size_t) strlen(old);
	size_t count, retlen, newlen = (size_t) strlen(new);

	if(newlen >  PTRDIFF_MAX ||  oldlen > PTRDIFF_MAX) return(str); 

	int samesize = (oldlen == newlen);

	if (!samesize) {
		for (count = 0, p = str; (q = strstr(p, old)) != NULL; p = q + oldlen)
			count++;

		retlen = p - str + strlen(p) + count * (newlen - oldlen);
	} else
		retlen = strlen(str);

	if ( (ret = (char*) calloc(retlen + 1,sizeof(char)) ) == NULL) return NULL;

	r = ret, p = str;
	while (TRUE) {
		if (!samesize && !count--)
			break;

		if ((q = strstr(p, old)) == NULL)
			break;

		ptrdiff_t l = q - p;
		memcpy(r, p, l);
		r += l;
		memcpy(r, new, newlen);
		r += newlen;
		p = q + oldlen;
	}
	strcpy(r, p);

	return( (char *) ret);
}
