/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2016 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_codeset.c , is part of tpad.
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

static char* tp_codeset_converter(const gchar * String, GIConv converter, gsize length);



gchar* tpad_codeset_convert_to_utf8_from_current_local( const gchar * StringToConvert, gsize length) {
	

	GIConv converter = (GIConv) g_iconv_open("UTF-8", g_get_codeset());

	return( tp_codeset_converter(StringToConvert, converter, length) );

}

gchar* tpad_codeset_convert_to_curent_local_from_utf8( const gchar * StringToConvert, gsize length) {

	GIConv converter = (GIConv) g_iconv_open(g_get_codeset(),"UTF-8");
	return( tp_codeset_converter(StringToConvert, converter, length) );


}


static char* tp_codeset_converter(const gchar * String, GIConv converter, gsize length) {
	gsize *bytes_written=0;
        GError *err = NULL;

	static char* ConvertedString = NULL;
	ConvertedString = (const char*)  g_convert_with_iconv( String, length,converter,NULL,bytes_written,&err);

	if (err != NULL){
		gerror_warn(err->message,_CONVERT_FAILED,TRUE,TRUE);
		g_error_free (err);	
	}
	else return( ConvertedString );

}
