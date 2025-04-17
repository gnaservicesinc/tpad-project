/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013, 2014 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_new_file.h , is part of tpad.
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
#ifndef TPAD_NEW_FILE_H
#define TPAD_NEW_FILE_H
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <dirent.h>
#include <gdk/gdk.h>
#include <glib/gstdio.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <libintl.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include "tpad_headers.h"
void new_file();
#endif