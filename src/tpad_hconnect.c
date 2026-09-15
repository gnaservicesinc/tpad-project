/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_hconnect.c , is part of tpad.
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

typedef gchar *(*TpadTextTransform)(const gchar *text);
typedef gchar *(*TpadFileTransform)(void);

static void transform_primary_clipboard(TpadTextTransform transform)
{
	gchar *clipboard_text;
	gchar *result;

	clipboard_text = tpad_clipboard_read_primary_text();
	if (clipboard_text == NULL)
		return;
	result = transform(clipboard_text);
	g_free(clipboard_text);
	if (result != NULL && strlen(result) > 2)
		tpad_clipboard_set_text(result);
	g_free(result);
}

static void transform_file_to_clipboard(TpadFileTransform transform)
{
	gchar *result = transform();

	if (result != NULL)
		tpad_clipboard_set_text(result);
	g_free(result);
}

void h512_connector(GtkWidget *caller)
{
	(void) caller;
	transform_primary_clipboard(str2sha512);
}

void h256_connector(GtkWidget *caller)
{
	(void) caller;
	transform_primary_clipboard(str2sha256);
}

void hmd5_connector(GtkWidget *caller)
{
	(void) caller;
	transform_primary_clipboard(str2md5);
}

void base64_connector(GtkWidget *caller)
{
	(void) caller;
	transform_primary_clipboard(str2base64);
}

void fbase64_connector(GtkWidget *caller)
{
	(void) caller;
	transform_primary_clipboard(strFrombase64);
}

void base64_file_connector(GtkWidget *caller)
{
	(void) caller;
	transform_file_to_clipboard(file2base64);
}

void fbase64_file_connector(GtkWidget *caller)
{
	(void) caller;
	transform_file_to_clipboard(filefrombase64);
}

void h512_file_connector(GtkWidget *caller)
{
	(void) caller;
	transform_file_to_clipboard(file2sha512);
}

void h256_file_connector(GtkWidget *caller)
{
	(void) caller;
	transform_file_to_clipboard(file2sha256);
}

void hmd5_file_connector(GtkWidget *caller)
{
	(void) caller;
	transform_file_to_clipboard(file2md5);
}
