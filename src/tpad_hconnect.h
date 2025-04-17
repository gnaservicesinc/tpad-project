/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_hconnect.h , is part of tpad.
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

#ifndef TPAD_HCONNECT_H
#define  TPAD_HCONNECT_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "tpad_headers.h"
void h512_connector(GtkWidget *caller);
void h256_connector(GtkWidget *caller);
void hmd5_connector(GtkWidget *caller);
void base64_connector(GtkWidget *caller);
void h512_file_connector(GtkWidget *caller);
void h256_file_connector(GtkWidget *caller);
void hmd5_file_connector(GtkWidget *caller);
void base64_file_connector(GtkWidget *caller);
void fbase64_connector(GtkWidget *caller);
void fbase64_file_connector(GtkWidget *caller);
#endif