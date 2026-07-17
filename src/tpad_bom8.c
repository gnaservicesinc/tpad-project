/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2019 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_bom8.c , is part of tpad.
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
gint tpad_string_has_bom8_len(const gchar *string, gsize length)
{
	const guchar *bytes = (const guchar *) string;

	if (bytes == NULL || length < 3)
		return 0;

	return bytes[0] == 0xef && bytes[1] == 0xbb && bytes[2] == 0xbf;
}

gint tpad_string_has_bom8(gchar **string)
{
	if (string == NULL || *string == NULL)
		return 0;

	return tpad_string_has_bom8_len(*string, strlen(*string));
}

gchar *tpad_utf8_bom_decode(const gchar *bytes, gsize length)
{
	const gchar *payload;
	gsize payload_length;

	if (!tpad_string_has_bom8_len(bytes, length))
		return NULL;
	payload = bytes + 3;
	payload_length = length - 3;
	if (payload_length > G_MAXSSIZE ||
	    !g_utf8_validate(payload, (gssize) payload_length, NULL))
		return NULL;
	return g_strndup(payload, payload_length);
}

gchar *tpad_utf8_bom_encode(const gchar *utf8, gsize length,
	                         gsize *output_length)
{
	const gchar *payload = utf8;
	gsize payload_length = length;
	gchar *output;

	if (output_length == NULL || utf8 == NULL)
		return NULL;
	if (tpad_string_has_bom8_len(utf8, length)) {
		payload += 3;
		payload_length -= 3;
	}
	if (payload_length > G_MAXSSIZE ||
	    !g_utf8_validate(payload, (gssize) payload_length, NULL) ||
	    payload_length > G_MAXSIZE - 4)
		return NULL;

	*output_length = payload_length + 3;
	output = g_malloc(*output_length + 1);
	output[0] = (gchar) 0xef;
	output[1] = (gchar) 0xbb;
	output[2] = (gchar) 0xbf;
	if (payload_length != 0)
		memcpy(output + 3, payload, payload_length);
	output[*output_length] = '\0';
	return output;
}
