#include "tpad_headers.h"

#include <stdint.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	gchar *first;
	gchar *result;

	if (size == 0)
		return 0;

	first = g_strndup((const gchar *) data + 1, size - 1);
	switch (data[0] % 4) {
	case 0:
		(void) tpad_string_is_text_data((const gchar *) data, size);
		result = str2sha512(first);
		g_free(result);
		result = str2sha256(first);
		g_free(result);
		result = str2md5(first);
		g_free(result);
		result = data_to_hex(data, size);
		g_free(result);
		break;
	case 1: {
		gchar *decoded;

		result = str2base64(first);
		decoded = strFrombase64(result);
		if (g_strcmp0(first, decoded) != 0)
			abort();
		g_free(decoded);
		g_free(result);
		break;
	}
	case 2: {
		size_t payload = size - 1;
		size_t first_length = payload / 3;
		size_t second_length = payload / 3;
		gchar *needle = g_strndup((const gchar *) data + 1 + first_length,
		                          second_length);
		gchar *replacement = g_strndup(
			(const gchar *) data + 1 + first_length + second_length,
			payload - first_length - second_length);

		result = tpad_replace_str(first, needle, replacement);
		g_free(result);
		g_free(replacement);
		g_free(needle);
		break;
	}
	default:
		if (tpad_string_has_bom8_len((const gchar *) data, size)) {
			gsize encoded_length;
			gchar *decoded = tpad_utf8_bom_decode((const gchar *) data,
			                                      size);
			if (decoded != NULL) {
				gchar *encoded = tpad_utf8_bom_encode(
				        decoded, strlen(decoded), &encoded_length);
				g_free(encoded);
				g_free(decoded);
			}
		}
		break;
	}
	g_free(first);
	return 0;
}
