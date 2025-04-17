/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013, 2014, 2015, 2016 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_fp.c , is part of tpad.
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

extern gchar* tpad_fp;
extern int tpad_fp_state;
static void* tpad_fp_calloc(size_t init_size);
static void* tpad_fp_recalloc(size_t new_size);

size_t cur = 0L;
gchar* tpad_fp=NULL;
int tpad_fp_state=_TPAD_FP_STATE_UNITIALIZED;

gchar* tpad_fp_get_current(){
	return( (gchar *) g_strdup( (gchar *) tpad_fp) );
}


static void* tpad_fp_calloc(size_t init_size){
}


static void* tpad_fp_recalloc(size_t new_size){
}
void tpad_fp_cleanup(){
	//g_free(tpad_fp);
	tpad_fp=NULL;
}

void tpad_fp_init(){
}
void tpad_fp_set(gchar* fp){
	tpad_fp = (gchar *) g_strdup(fp);
}