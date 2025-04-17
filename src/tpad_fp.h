/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013, 2014, 2015 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_fp.h , is part of tpad.
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
#ifndef _TPAD_FP_H
#define _TPAD_FP_H
#include "tpad_headers.h"
/*
#ifndef _TPAD_FP_EXTERN
#define _TPAD_FP_EXTERN

#endif
*/
#ifndef _TPAD_FP_INIT_MEMSIZE
#define _TPAD_FP_INIT_MEMSIZE 1024
#endif

#ifndef _TPAD_FP_STATE_UNITIALIZED
#define _TPAD_FP_STATE_UNITIALIZED 0
#endif

#ifndef _TPAD_FP_STATE_INITIALIZED
#define _TPAD_FP_STATE_INITIALIZED 1
#endif

#ifndef _TPAD_FP_STATE_SET
#define _TPAD_FP_STATE_SET 2
#endif

#ifndef _TPAD_FP_STATE_FREE
#define _TPAD_FP_STATE_FREE 3
#endif

void tpad_fp_init();
void tpad_fp_set(gchar* fp);
gchar* tpad_fp_get_current();
void tpad_fp_cleanup();

#endif