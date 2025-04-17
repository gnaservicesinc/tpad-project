/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013, 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_save.h , is part of tpad.
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
#ifndef _FALLBACK_SAVE_FILE_NAME
#define _FALLBACK_SAVE_FILE_NAME gettext("Untitled")
#endif
#ifndef _TPAD_SAVE_
#define _TPAD_SAVE_
#include "include.h"
gint save_modified();
void save_as();
void save_file();
gint save_watched();
gint tpad_save_ondisk_change_detected();
#endif