/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013, 2014, 2015 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_open_guard.c , is part of tpad.
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
#ifndef _FP_BUFFER_SIZE
#define _FP_BUFFER_SIZE 2056
#endif
#ifndef _FILE_COPY_BUFFER
#define _FILE_COPY_BUFFER  8192
#endif
extern gchar* path_temp_file;
extern int ihaveTemp;


gchar* path_temp_file = NULL;
int ihaveTemp=0;
static gint open_guard_check_file (gchar* FilePathToCheck);
static void tpad_open_guard_make_temp();
static int tpad_openguard_found(char* fp);
static gint open_guard_check_file (gchar* FilePathToCheck){
	if(access(FilePathToCheck, R_OK ) != -1){
	tpad_openguard_found(FilePathToCheck);
	 return(1);
	}
	else return(0);
}

static int tpad_openguard_found(char* fp){
if (!gerror_openguard_popup(tpad_fp_get_current(), fp)){
	new_thread_tpad(NULL);
	quit_program();
	//exit(1);
	} 
	
 return(1);
}

int tpad_open_guard_check_ok(){

	if(tpad_fp_get_current() == NULL) {
	return(1);
	}
	ihaveTemp=0;
 	path_temp_file=(gchar*) NULL;
	
	path_temp_file = (gchar*) g_strconcat((gchar*) g_strdup(tpad_fp_get_current()),"~",NULL);
	if (path_temp_file == NULL) {
	return(1);

		}
	int ichk=0;
	ichk += open_guard_check_file((gchar*) path_temp_file);
	ichk += open_guard_check_file((gchar*) g_strconcat((gchar*)g_strdup( (gchar*)get_currentfile_dirname() ),"/.",(gchar*) g_strdup(get_currentfile_basename()),".swp",NULL));
	ichk += open_guard_check_file((gchar*) g_strconcat((gchar*)g_strdup( (gchar*)get_currentfile_dirname() ),"/.~lock.",NULL));
	ichk += open_guard_check_file( (gchar*) g_strconcat((gchar*) g_strdup(get_currentfile_basename()),"#",NULL));
	if(ichk){
	return(1);
	}
	else{
		int itpad_copy_return = tpad_copy(tpad_fp_get_current(),path_temp_file);
		if (itpad_copy_return == 0 ) ihaveTemp = 1; // Success! 

		
		if(ihaveTemp) return(0);
		else return(1);
	}
}



