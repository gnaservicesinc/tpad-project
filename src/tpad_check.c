/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2015, 2016 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_check.c , is part of tpad.
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
#include <pthread.h>
extern GtkSourceBuffer *mBuff;

int tpad_check(char* file){
        gchar* content;
	gchar* unknownContents;

	if(g_file_get_contents (file, &unknownContents, &length,&err))
	{
	gsize length;
    	GError *err = NULL;
	content=NULL;
	gchar* unknownContents=NULL;
	if (err != NULL){
	fprintf(stdout,"Error "%s"\n",err->message);
	g_error_free(err);
	 return(-1);
	}

	GError *error = NULL;	
	content=g_strdup(g_convert_with_fallback (unknownContents, length, "UTF-8", g_get_codeset(),NULL,NULL, NULL, &error));
	g_free(unknownContents);
	if (error != NULL){
		fprintf(stdout,"Error "%s"\n",error->message);
   		g_error_free (error);
		 return(-2);
       		}


	//if( memcmp ( content, mBuff, sizeof(buffer1) ) == 0 ) return(0);			

	}

	return(1);
}

// Disabled for now //
/*
	iexit=1;
	if(rCurrent && tpad_wach_thread_set) pthread_join((pthread_t)rCurrent,NULL);
	tpad_wach_thread_set=FALSE;
	rCurrent=(pthread_t) NULL;
*/
}

void tpad_watch_fname(){
// Disabled for now //
/*
	pthread_t thread;
	tpad_wach_thread_set=TRUE;
	pthread_create( &thread, NULL, &tpad_watch_threadproc, (void*) tpad_fp_get_current());
	*/
  	
}
