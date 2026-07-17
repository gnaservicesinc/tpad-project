/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_file.c , is part of tpad.
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
extern GtkWidget *window;
extern gboolean save_locked;
extern char* tpad_fp;
extern int tpad_fp_state;

static gchar *cRpath=NULL;

int tpad_touch_check_file(char* fp)
{
	gchar *path;
	struct stat status;
	int fd;
	int result = -1;

	path = clean_path(fp);
	if (path == NULL || *path == '\0') {
		g_free(path);
		return -1;
	}

	fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0666);
	if (fd >= 0) {
		(void) fcntl(fd, F_SETFD, FD_CLOEXEC);
		result = close(fd) == 0 ? 1 : -1;
	} else if (errno == EEXIST && stat(path, &status) == 0 &&
	           S_ISREG(status.st_mode)) {
		result = 0;
	}

	g_free(path);
	return result;
}
size_t tpad_get_file_size(char* filepath)
{
	GError *error = NULL;
	gchar *filename;
	GStatBuf status;
	size_t size = 0;

	if (filepath == NULL)
		return 0;

	filename = g_filename_from_utf8(filepath, -1, NULL, NULL, &error);
	if (filename == NULL) {
		if (error != NULL) {
			print(error->message);
			g_error_free(error);
		}
		return 0;
	}

	if (g_stat(filename, &status) == 0 && S_ISREG(status.st_mode) &&
	    status.st_size >= 0 && (guint64) status.st_size <= G_MAXSIZE)
		size = (size_t) status.st_size;

	g_free(filename);
	return size;
}

gchar *get_currentfile_basename(void)
{
	gchar *current = tpad_fp_get_current();
	gchar *basename;

	if (current == NULL)
		return NULL;

	basename = g_path_get_basename(current);
	g_free(current);
	return basename;
}

gchar *get_currentfile_dirname(void)
{
	gchar *current = tpad_fp_get_current();
	gchar *directory;

	if (current == NULL)
		return NULL;

	directory = g_path_get_dirname(current);
	g_free(current);
	return directory;
}

void tpad_copy_file_name_to_clipboard(GtkWidget *caller){
	gchar *current = tpad_fp_get_current();

	(void) caller;
	if (current != NULL){
		gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
		                       current, -1);
		gtk_clipboard_store (gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
	}
	g_free(current);
}

gchar *clean_path(gchar *path){
	GError *error = NULL;
	const gchar *codeset;
	gchar *converted;

	if (path == NULL)
		return NULL;

	(void) g_get_charset(&codeset);
	converted = g_convert(path, -1, "UTF-8", codeset, NULL, NULL,
	                      &error);
	if (converted == NULL) {
		if (error != NULL) {
			print(error->message);
			g_error_free(error);
		}
		return NULL;
	}

	return converted;
}

gchar* link_resolve(gchar* file)
{
	GError *error = NULL;
	gchar *filename;
	gchar *target;
	gchar *utf8_target;

	if (file == NULL || *file == '\0')
		return NULL;

	filename = g_filename_from_utf8(file, -1, NULL, NULL, &error);
	if (filename == NULL) {
		if (error != NULL)
			print(error->message);
		g_clear_error(&error);
		return NULL;
	}

	target = g_file_read_link(filename, &error);
	g_free(filename);
	if (target == NULL) {
		gerror_warn(error != NULL ? error->message : "Unable to resolve link",
		            "tpad_file.c: link_resolve", TRUE, FALSE);
		g_clear_error(&error);
		return NULL;
	}

	utf8_target = g_filename_to_utf8(target, -1, NULL, NULL, &error);
	g_free(target);
	if (utf8_target == NULL) {
		if (error != NULL)
			print(error->message);
		g_clear_error(&error);
	}

	return utf8_target;
}



gint get_file_type(gchar *file)
{
	GError *error = NULL;
	gchar *filename;
	gint type;

	if (file == NULL)
		return _ERROR_FILE_GET_TYPE;

	filename = g_filename_from_utf8(file, -1, NULL, NULL, &error);
	if (filename == NULL) {
		if (error != NULL)
			print(error->message);
		g_clear_error(&error);
		return _ERROR_FILE_GET_TYPE;
	}

	if (g_file_test(filename, G_FILE_TEST_IS_SYMLINK))
		type = _FILE_IS_A_LINK;
	else if (g_file_test(filename, G_FILE_TEST_IS_DIR))
		type = _FILE_IS_A_DIRECTORY;
	else if (g_file_test(filename, G_FILE_TEST_IS_EXECUTABLE))
		type = _FILE_IS_A_EXE_FILE;
	else if (g_file_test(filename, G_FILE_TEST_IS_REGULAR))
		type = _FILE_IS_A_REGRULAR_FILE;
	else if (g_file_test(filename, G_FILE_TEST_EXISTS))
		type = _FILE_IS_OF_UNKOWN_TYPE;
	else
		type = _ERROR_FILE_GET_TYPE;

	g_free(filename);
	return type;

/*
	struct stat buf;
	if (g_stat(file, &buf) != 0) {
			 print(g_strconcat("ERROR  Getting Information on file:  ",file,"\n",NULL));
			return(ERR);
          }
	else{
			if (S_ISLNK(buf.st_mode)) return(LNK);
		    else if(S_ISDIR(buf.st_mode)) return(DIR);
			else if(S_ISREG(buf.st_mode)) return(REG);
			else return(ERR);
	}
*/
}

gchar* getcRpath(void){
	if(cRpath!=NULL) return(g_strdup(cRpath));
	else return((gchar*)NULL);
}
 gchar* check_file(gchar *afile)
{
	/* File-type dispatch below was disabled long ago.  Keep the active
	 * behavior, but return one owned, validated conversion. */
	return clean_path(afile);
		/* Bypassing */
	/* Removed Code Start
switch(get_file_type(file))
	{

	case _FILE_IS_A_LINK:
			return(check_file(link_resolve( g_strdup(file))));
	break;
	case _FILE_IS_A_EXE_FILE:
		return((gchar*) NULL);
	break;

	case _FILE_IS_OF_UNKOWN_TYPE:
		return((gchar*) g_strdup(file));
	break;
	case _FILE_IS_A_DIRECTORY:

			cRpath=g_strdup(file);

			//tpad_file_handel_dir_open(file);
			return((gchar*) NULL);
			break;
	case _FILE_IS_A_REGRULAR_FILE:
				return(g_strdup(file));
			break;
	case _ERROR_FILE_GET_TYPE:
						// Holding spot for now
			return(g_strdup(file));
			break;
	default:
							// Holding spot for now
			return(g_strdup(file));
			break;

	}
	 Removed Code End */

}
