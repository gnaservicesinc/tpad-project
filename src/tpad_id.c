/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_id.c , is part of tpad.
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

#ifndef HAVE_SD_ID128_GET_MACHINE_APP_SPECIFIC
static int tpad_hmac_sha256(const unsigned char key[16],
                            const unsigned char input[16],
                            unsigned char output[32])
{
	mbedtls_sha256_context context;
	unsigned char inner_pad[64];
	unsigned char outer_pad[64];
	unsigned char inner_hash[32];
	size_t i;
	int result = -1;

	memset(inner_pad, 0x36, sizeof(inner_pad));
	memset(outer_pad, 0x5c, sizeof(outer_pad));
	for (i = 0; i < 16; i++) {
		inner_pad[i] ^= key[i];
		outer_pad[i] ^= key[i];
	}

	mbedtls_sha256_init(&context);
	if (mbedtls_sha256_starts_ret(&context, 0) != 0 ||
	    mbedtls_sha256_update_ret(&context, inner_pad, sizeof(inner_pad)) != 0 ||
	    mbedtls_sha256_update_ret(&context, input, 16) != 0 ||
	    mbedtls_sha256_finish_ret(&context, inner_hash) != 0)
		goto cleanup;

	if (mbedtls_sha256_starts_ret(&context, 0) != 0 ||
	    mbedtls_sha256_update_ret(&context, outer_pad, sizeof(outer_pad)) != 0 ||
	    mbedtls_sha256_update_ret(&context, inner_hash, sizeof(inner_hash)) != 0 ||
	    mbedtls_sha256_finish_ret(&context, output) != 0)
		goto cleanup;

	result = 0;

cleanup:
	mbedtls_sha256_free(&context);
	memset(inner_hash, 0, sizeof(inner_hash));
	memset(inner_pad, 0, sizeof(inner_pad));
	memset(outer_pad, 0, sizeof(outer_pad));
	return result;
}

static int tpad_get_app_specific_id(sd_id128_t *result)
{
	sd_id128_t machine;
	sd_id128_t application = APP_ID_TPAD;
	unsigned char digest[32];

	if (sd_id128_get_machine(&machine) < 0 ||
	    tpad_hmac_sha256(machine.bytes, application.bytes, digest) != 0)
		return -1;

	memcpy(result->bytes, digest, sizeof(result->bytes));
	result->bytes[6] = (result->bytes[6] & 0x0f) | 0x40;
	result->bytes[8] = (result->bytes[8] & 0x3f) | 0x80;
	memset(digest, 0, sizeof(digest));
	return 0;
}
#else
static int tpad_get_app_specific_id(sd_id128_t *result)
{
	return sd_id128_get_machine_app_specific(APP_ID_TPAD, result);
}
#endif

gchar *tpad_id_get_id(void)
{
	sd_id128_t id = SD_ID128_NULL;
	/* systemd 204 declares this API with a 33-byte buffer but no size macro. */
	char formatted[33];

	if (tpad_get_app_specific_id(&id) < 0)
		return g_strdup("unavailable");

	return g_strdup(sd_id128_to_string(id, formatted));
}
