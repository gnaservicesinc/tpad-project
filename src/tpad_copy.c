/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2015-2026 Andrew Smith (GNA SERVICES INC)
 *     <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_copy.c, is part of tpad.
 *
 *   tpad is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   tpad is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with tpad. If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************************/
#include "tpad_headers.h"

extern gchar *path_temp_file;

static gboolean temp_copy_created = FALSE;

static gboolean copy_to_new_file(const gchar *source_path,
	const gchar *destination_path)
{
	GFile *source;
	GFile *destination;
	GFileInputStream *input = NULL;
	GFileOutputStream *output = NULL;
	GError *error = NULL;
	gboolean copied = FALSE;

	source = g_file_new_for_path(source_path);
	destination = g_file_new_for_path(destination_path);
	input = g_file_read(source, NULL, &error);
	if (input != NULL) {
		/* Exclusive, private creation avoids overwriting a guard file and
		 * prevents another user from reading the temporary copy. */
		output = g_file_create(destination, G_FILE_CREATE_PRIVATE, NULL,
		                       &error);
	}
	if (output != NULL) {
		copied = g_output_stream_splice(
		        G_OUTPUT_STREAM(output), G_INPUT_STREAM(input),
		        G_OUTPUT_STREAM_SPLICE_CLOSE_SOURCE |
		        G_OUTPUT_STREAM_SPLICE_CLOSE_TARGET,
		        NULL, &error) >= 0;
		if (!copied)
			(void) g_file_delete(destination, NULL, NULL);
	}

	g_clear_error(&error);
	g_clear_object(&output);
	g_clear_object(&input);
	g_object_unref(destination);
	g_object_unref(source);
	return copied;
}

gint tpad_copy(char *source, char *dest)
{
	gboolean copied;

	if (source == NULL || dest == NULL)
		return -1;

	/* Callers provide filesystem paths; GFile performs the platform-specific
	 * handling directly, without a lossy locale/UTF-8 round trip. */
	copied = copy_to_new_file(source, dest);
	temp_copy_created = copied;
	return copied ? 0 : -1;
}

void *tpad_cp_remove_temp(void)
{
	if (temp_copy_created && path_temp_file != NULL &&
	    strlen(path_temp_file) > 1) {
		(void) g_unlink(path_temp_file);
	}
	temp_copy_created = FALSE;
	return NULL;
}
