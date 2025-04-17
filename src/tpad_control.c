/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *s
 *   This file, tpad_control.c , is part of tpad.
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
extern gchar* FILE_HASH_ON_LAST_OPEN_OR_SAVE;
extern gchar* FILE_NAME_ON_LAST_OPEN_OR_SAVE;
extern short FILE_HASH_STORED;
gchar* FILE_NAME_ON_LAST_OPEN_OR_SAVE =  (char*) NULL;
gchar* FILE_HASH_ON_LAST_OPEN_OR_SAVE =  (char*) NULL;
short FILE_HASH_STORED = 0;




short tpad_control_store_hash_of_current_file_set() {
	if (tpad_fp_get_current() != NULL && access(tpad_fp_get_current(), R_OK|W_OK ) != -1) {
		gchar* temp_FILE_HASH_ON_LAST_OPEN_OR_SAVE = (gchar*) curFile2sha512();
		gchar* temp_FILE_NAME_ON_LAST_OPEN_OR_SAVE = (gchar*) tpad_fp_get_current();

		if(temp_FILE_HASH_ON_LAST_OPEN_OR_SAVE != NULL && temp_FILE_NAME_ON_LAST_OPEN_OR_SAVE != NULL) {
			FILE_HASH_ON_LAST_OPEN_OR_SAVE = (gchar*) g_strdup(temp_FILE_HASH_ON_LAST_OPEN_OR_SAVE);
			FILE_NAME_ON_LAST_OPEN_OR_SAVE = (gchar*) g_strdup(temp_FILE_NAME_ON_LAST_OPEN_OR_SAVE);
			FILE_HASH_STORED=1;
		}

		g_free(temp_FILE_NAME_ON_LAST_OPEN_OR_SAVE);
		g_free(temp_FILE_HASH_ON_LAST_OPEN_OR_SAVE);

		

	
	}
	else {
		FILE_NAME_ON_LAST_OPEN_OR_SAVE = (char*) NULL;
		FILE_HASH_ON_LAST_OPEN_OR_SAVE = (char*) NULL;

		FILE_HASH_STORED=0;
	}
   return( (short) FILE_HASH_STORED);

}

short tpad_control_compare_stored_file_hash_to_current_ondisk_file_hash() {
	//Returns 1 if they do not match... implies that file may have been changed while open. 
	// Returns 0 if change was detected, no hash stored, no file path set, or on error. 
	// In short, unless a mismatch is detected this function returns 0.

	int rets=0;

	//IF we don't have a stored hash to compare, 
	// We must be doing a first save so we need to return 0 
	// to allow a first save to occur.  We can not yet has the file as it has not been written to yet. 


	if (!FILE_HASH_STORED) return 0;



	
	if (FILE_HASH_STORED && tpad_fp_get_current() != NULL) {

		size_t a = (size_t) strlen(FILE_NAME_ON_LAST_OPEN_OR_SAVE);
		size_t b = (size_t) strlen(tpad_fp_get_current());
		
		if (a != b)  return(0); // If not the same len we can assume the filename has changed. Else lets compare the stored file name with the current one and check that we can read and write to the file path.
		if ( strncmp((const char*) FILE_NAME_ON_LAST_OPEN_OR_SAVE,(const char*) tpad_fp_get_current(), a ) == 0 && access(tpad_fp_get_current(), R_OK|W_OK ) != -1) {
			gchar* tmpHash = (gchar*) curFile2sha512();
			if (rets = strncmp((const char*) FILE_HASH_ON_LAST_OPEN_OR_SAVE, (const char*) tmpHash, 128) != 0) {
		printf("\n\nFILE_HASH_ON_LAST_OPEN_OR_SAVE\t=\t%s\ntmpHash=\t=\t%s\nFILE_NAME_ON_LAST_OPEN_OR_SAVE\t=\t%s\nrets\t=\t%i\n\n",FILE_HASH_ON_LAST_OPEN_OR_SAVE,tmpHash,FILE_NAME_ON_LAST_OPEN_OR_SAVE,rets);
			g_free(tmpHash);				
			return(1); // Hash of on disk file has changed since storing the hash. Return 1. 
			}
		g_free(tmpHash);
		}
		
  	}
	
return(0);

}


