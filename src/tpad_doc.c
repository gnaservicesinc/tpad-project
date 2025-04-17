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
extern gchar *content;
extern 	gchar* unknownContents;

// Get text buffer contents -> ensure / convert contents to UTF 8 -> modify contents
// -> ensure / convert modifyed contents to UTF 8 -> modify / set text buffer -> place_cursor

void low_caps_document(){
	mod_doc(_METHOD_DOWNCAP);
}

void up_caps_document(){
	mod_doc(_METHOD_UPCAP);
}

void rev_document(){
	mod_doc(_METHOD_REVERSE);
}

void hex_document(){
	mod_doc(_METHOD_HEX);
}

gint mod_doc(int method){
	//gsize length;
	gchar* unknownContents;
	unknownContents=NULL;
	  
	int hex;
	GError *error = NULL;	
	if (method >= 0 && method <= _METHOD_MAX){
		GtkTextMark *  mark_start = (GtkTextMark *) gtk_text_mark_new ("mStart",TRUE);
		GtkTextMark *  mark_end = (GtkTextMark *) gtk_text_mark_new ("mEnd",FALSE);
	GtkTextIter start,end;
	gchar *RevBuff;
	if(gtk_text_buffer_get_has_selection (GTK_TEXT_BUFFER(mBuff)))
		{
		if(gtk_text_buffer_get_selection_bounds( GTK_TEXT_BUFFER(mBuff),&start,&end))
			{
			gtk_text_buffer_begin_user_action(GTK_TEXT_BUFFER(mBuff));	
			RevBuff=NULL;
			
			gchar* cUnknown = (gchar*) g_strdup(gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),&start,&end,FALSE));
				

			gchar* temp_buffer = (gchar*) g_strdup(g_convert(cUnknown,(gsize) strlen (cUnknown), "UTF-8", g_get_codeset(),NULL, NULL, &error));
		   if (error != NULL)
       		{
				gerror_warn(error->message,_ERROR_STR_REV,TRUE,TRUE);
   				g_error_free (error);
				return(-1);
       		}
				
			switch(method)
			{
			case _METHOD_REVERSE: 
					RevBuff = g_strdup( g_strreverse(temp_buffer) );
			break;
			
			case _METHOD_UPCAP:
				RevBuff=g_ascii_strup(gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),&start,&end,FALSE),-1);
			break;

			case _METHOD_DOWNCAP:
				RevBuff=g_ascii_strdown (gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),&start,&end,FALSE),-1);
			break;

			case _METHOD_HEX:
			
			hex=0;
			
			RevBuff = g_strdup(gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),&start,&end,FALSE));
			gchar* ptrTemp = (gchar*) g_strdup( (gchar*) RevBuff );

			break;
			default:
				gerror_warn(_ERROR_MOD_DOC_METHOD_VAL,(gchar*)__func__,TRUE,FALSE);
			break;
			
			}

				if(RevBuff == NULL) return(2);
				
			content=g_strdup(g_convert(RevBuff,(gsize) strlen (RevBuff), "UTF-8", g_get_codeset(),NULL, NULL, &error));
		   if (error != NULL)
       		{
				gerror_warn(error->message,_ERROR_STR_REV,TRUE,TRUE);
   				g_error_free (error);
				return(-1);
       		}
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
	}
	else {
	
	gtk_text_buffer_begin_user_action(GTK_TEXT_BUFFER(mBuff));
	gtk_text_buffer_get_bounds( GTK_TEXT_BUFFER(mBuff),&start,&end);
		gchar* cUnknown = (gchar*) g_strdup(gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),&start,&end,FALSE));                                     
		gchar* temp_buffer = (gchar*) g_strdup(g_convert(cUnknown,(gsize) strlen (cUnknown), "UTF-8", g_get_codeset(),NULL, NULL, &error));
		   if (error != NULL)
       		{
				gerror_warn(error->message,_ERROR_STR_REV,TRUE,TRUE);
   				g_error_free (error);
				return(-1);
       		}
		
			switch(method)
			{
			case _METHOD_REVERSE: 	

					
				unknownContents=g_strdup(g_strreverse(temp_buffer));
			break;
			
			case _METHOD_UPCAP:
				unknownContents=g_ascii_strup(temp_buffer,-1);
			break;

			case _METHOD_DOWNCAP:
				unknownContents=g_ascii_strdown (temp_buffer,-1);
			break;

			default:
				gerror_warn(_ERROR_MOD_DOC_METHOD_VAL,(gchar*)__func__,TRUE,FALSE);
			break;
			
			}

		    if(unknownContents == NULL) return(2);

			content=NULL;
			content=g_strdup(g_convert(unknownContents, strlen(unknownContents), "UTF-8", g_get_codeset(),NULL, NULL, &error));

		   if (error != NULL)
       		{
				gerror_warn(error->message,_ERROR_STR_REV,TRUE,TRUE);
   				g_error_free (error);
				return(-1);
       		}

	// Set Buffer
		GtkTextIter iter;
		
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(mBuff),content,-1);
		g_free(content);
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(mBuff),&iter); 
		gtk_text_buffer_end_user_action (GTK_TEXT_BUFFER(mBuff));
        gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(mBuff),TRUE);
     
       
        gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(mBuff),&iter);
		return(1);
	}
	}
}