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

gboolean tpad_string_is_text_data(const gchar *data, gsize length)
{
	return data != NULL && memchr(data, '\0', length) == NULL;
}

int str_size(char* string){
/*
 mbstate_t t;
 char *scopy = string;
 memset (&t, '\0', sizeof (t));
 return(mbsrtowcs (NULL,(const char **) &scopy, strlen (scopy), &t));
*/
return(strlen(string));
}
gint gtk_text_buffer_get_word_count (GtkTextBuffer *buffer){
	GtkTextIter iter;
	gint count = 0;

	if (buffer == NULL)
		return 0;

	gtk_text_buffer_get_start_iter(buffer, &iter);
	while (!gtk_text_iter_is_end(&iter)) {
		if (gtk_text_iter_starts_word(&iter))
			count++;
		if (!gtk_text_iter_forward_char(&iter))
			break;
	}

	return count;
}
