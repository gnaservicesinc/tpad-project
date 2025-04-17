/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_id.c , is part of tpad.
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
gchar* tpad_id_get_id() {
/*
	sd_id128_t id;
	sd_id128_get_machine_app_specific(APP_ID_TPAD, &id);
	assert(sd_id128_is_null(id));
	gchar* rid = (gchar*) g_strdup_vprintf(SD_ID128_FORMAT_STR,SD_ID128_FORMAT_VAL(id));

return( (gchar*) rid);
*/
#define BUFF_SIZE_MIN 1
        sd_id128_t id;
	sd_id128_get_machine_app_specific(APP_ID_TPAD,&id);

int check=0;
size_t buffer_size = (size_t) BUFF_SIZE_MIN;

int inotdone=1;
char* buff = (char*) NULL;
char* LargerBuffer =NULL;

while (inotdone)
{
LargerBuffer = (char*) realloc(buff,buffer_size * sizeof(char));

if ( LargerBuffer != NULL){
	buff = LargerBuffer;
}
	else {
		free(buff);
		exit(1);
		}

check = snprintf(buff,buffer_size,SD_ID128_FORMAT_STR,SD_ID128_FORMAT_VAL(id));

//check = g_vsnprintf(buff,buffer_size,SD_ID128_FORMAT_STR,SD_ID128_FORMAT_VAL(id));

if (check >=0 && check < buffer_size) {
	inotdone = 0;
	break;
}
else if (check >= buffer_size){
	buffer_size = check + 1;
	continue;
}

else if (check < 0) {
	fprintf(stderr,"FAILED -- Encoding Error!\nERROR DETECTED IN tpad_id.c \nsnprintf returned less than zero in function tpad_id_get_id().\nPlease file a bug report at https://bugs.launchpad.net/tpad-project/+filebug");	 exit(1);
}

else exit(1);

}
return (buff);
//return (g_strdup(buff));

}