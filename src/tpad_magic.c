/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_magic.c , is part of tpad.
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


int check_magic(gchar* file) {

return(1);/*
		#ifdef HAVE_LIBMAGIC
		if(cfg_use_magic() == 0) return(1);
	
		if((unsigned int) tpad_get_file_size(file) == 0) return(1);
		int ret =0;
		char *actual_file = (char*) file;
		const char *magic_full;
		magic_t magic_cookie;
		magic_cookie = magic_open(MAGIC_MIME); 
	if (magic_cookie == NULL) {
	        return(2);
		    }
	if (magic_load(magic_cookie, NULL) != 0) {
	        magic_close(magic_cookie);
	        return(3);
	    }
	magic_full = magic_file(magic_cookie, actual_file);
	char* magic_string= (char*) magic_full;
	// [DEBUGGING]
	//	fprintf(stdout,"FILE MAGIC: %s\n",magic_string);
	//

	if(tpad_magic_test(magic_string,"inode/symlink") ==0){
		return( check_magic( link_resolve(actual_file)) );
	}

	if(tpad_magic_test(magic_string,"text") == 0 ) ret =1;
	else ret =0;
	return(ret);
#endif
	#ifndef HAVE_LIBMAGIC
	return(1);
#endif
*/
}

gint tpad_magic_test (gchar file_mime_string[], gchar check_mime_string[]){
return(1);	
/*
#ifdef HAVE_LIBMAGIC
	if(cfg_use_magic() == 0) return(1);

	unsigned int slen=strlen(check_mime_string);
	gchar beg_string[slen];
	strncpy ( beg_string, file_mime_string, (int)slen );
	if(strncmp(beg_string,check_mime_string,slen) == 0) return(0);
	else return(1);
#endif
#ifndef HAVE_LIBMAGIC
	return(1);
#endif
*/
}



