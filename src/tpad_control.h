/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_control.h , is part of tpad.
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

#ifndef TPAD_CONTROL_H
#define  TPAD_CONTROL_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "tpad_headers.h"

short tpad_control_store_hash_of_current_file_set();
short tpad_control_compare_stored_file_hash_to_current_ondisk_file_hash();


#endif