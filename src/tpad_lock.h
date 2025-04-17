/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_lock.h , is part of tpad.
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

#ifndef TPAD_LOCK_H
#define  TPAD_LOCK_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "tpad_headers.h"

#ifndef _S_LOCK_NODE_
#define _S_LOCK_NODE_
struct lock_node
{
  unsigned int key_value;
  unsigned int global_id;
  struct lock_node *left;
  struct lock_node *right;
  unsigned char hash_512[128];
  char filePath[PATH_MAX+1];
  unsigned int pid;
  struct timespec Begin;
  
};
#endif


#endif