/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2016 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_codeset.c , is part of tpad.
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

static gchar *tp_codeset_converter(const gchar *string, gsize length,
                                   const gchar *destination_codeset,
                                   const gchar *source_codeset);



gchar *tpad_codeset_convert_to_utf8_from_current_local(
        const gchar *string, gsize length)
{
	const gchar *codeset;

	(void) g_get_charset(&codeset);
	return tp_codeset_converter(string, length, "UTF-8", codeset);

}

gchar *tpad_codeset_convert_to_curent_local_from_utf8(
        const gchar *string, gsize length)
{
	const gchar *codeset;

	(void) g_get_charset(&codeset);
	return tp_codeset_converter(string, length, codeset, "UTF-8");

}

static gchar *tp_codeset_converter(const gchar *string, gsize length,
                                   const gchar *destination_codeset,
                                   const gchar *source_codeset)
{
	GError *error = NULL;
	gchar *converted;

	if (string == NULL)
		return NULL;

	converted = g_convert(string, length, destination_codeset,
	                      source_codeset, NULL, NULL, &error);
	if (converted == NULL) {
		gerror_warn(error != NULL ? error->message : _CONVERT_FAILED,
		            _CONVERT_FAILED, TRUE, FALSE);
		g_clear_error(&error);
	}

	return converted;
}
