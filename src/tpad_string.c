/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_string.c , is part of tpad.
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

int str_size(char* string){
/*
 mbstate_t t;
 char *scopy = string;
 memset (&t, '\0', sizeof (t));
 return(mbsrtowcs (NULL,(const char **) &scopy, strlen (scopy), &t));
*/
return(strlen(string));
}
void clear_uchar_array(unsigned char* first, ...){
  /*
	Usage: clear_char_array(anArray,..., NULL);
	--> ALWAYS END LIST WITH NULL
	--> Pass asmany arrays as you like, all will be cleared.
	--> REQUIRES #include <stdarg.h> 	
  */
  unsigned char* str;
  va_list vl;
  str=first;
  va_start(vl,first);

  do {
    if(str) memset(&str[0], 0, sizeof(str));

    str=(unsigned char*) va_arg(vl,unsigned char*);
  } while (str!=NULL);

  va_end(vl);
}

void clear_char_array(gchar first[], ...){
  /*
	Usage: clear_char_array(anArray,..., NULL);
	--> ALWAYS END LIST WITH NULL
	--> Pass asmany arrays as you like, all will be cleared.
	--> REQUIRES #include <stdarg.h> 	
  */
  gchar* str;
  va_list vl;
  str=first;
  va_start(vl,first);

  do {
    if(str) memset(&str[0], 0, sizeof(str));

    str=(gchar*) va_arg(vl,gchar*);
  } while (str!=NULL);

  va_end(vl);
}

gboolean is_separator (gchar cChar){
	static const gchar *token = " -.,;:#+*][{}^'?!\"Â£$%&/\\()=|<>\n\t\r";
	gint i=0;
	for ( i = 0; i < strlen (token); i++ ){
		 if ( token[i] == cChar ) return TRUE;
	}
	return FALSE;
}

gint gtk_text_buffer_get_word_count (GtkTextBuffer *buffer){
	GtkTextIter start, end;
	gchar *cstring=NULL;
	gint i, leng, icount;
	gtk_text_buffer_get_bounds (buffer, &start, &end);
	cstring = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
	leng = strlen (cstring);
	for ( i = (icount = 1) - 1; i < leng; i++ ){
	if ( is_separator (cstring[i]) && !is_separator (i ? cstring[i - 1] : 'h') )icount++;
	}
	if ( is_separator (cstring[i - 1]) )  icount--;
	g_free (cstring);
	return icount;
}