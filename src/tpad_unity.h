/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_unity.h , is part of tpad.
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
/ ********************************************************************************/

#ifndef TPAD_UNITY_H
#define  TPAD_UNITY_H
#include <config.h>
#include "tpad_headers.h"
/*
#ifdef HAVE_LIBUNITY
void ui_unity_init(void);
int ui_unity_urgent_on (void);
int ui_unity_urgent_off (void);
void ui_unity_destroy (void);
#else
*/
void ui_unity_init(void);
int ui_unity_urgent_on (void);
int ui_unity_urgent_off (void);
void ui_unity_destroy (void);


//#endif  /* HAVE_LIBUNITY */


#endif