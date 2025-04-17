/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_file.h , is part of tpad.
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
#ifndef TPAD_FILE_H
#define TPAD_FILE_H
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "tpad_headers.h"
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

#define DIR 1
#define LNK 2
#define ERR 3
#define REG 4
int tpad_touch_check_file(char* fp);
gchar *get_currentfile_basename();
gchar *get_currentfile_dirname();
size_t tpad_get_file_size(char* filepath);
gchar* getcRpath(void);
gchar* clean_path(gchar *path);
gchar* link_resolve(gchar* file);
gint get_file_type( gchar *file);
gchar* check_file(gchar *afile);
void tpad_copy_file_name_to_clipboard(GtkWidget *caller);
#endif