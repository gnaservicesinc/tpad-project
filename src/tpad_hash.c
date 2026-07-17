/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014, 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_hash.c , is part of tpad.
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
extern GtkSourceBuffer *mBuff;

typedef int (*TpadHashUpdate)(void *context,
                              const unsigned char *data,
                              size_t length);

static char *tpad_hash_error(void);
static gboolean tpad_hash_update_file(const char *path,
                                      void *context,
                                      TpadHashUpdate update);
static int tpad_sha512_update(void *context,
                              const unsigned char *data,
                              size_t length);
static int tpad_sha256_update(void *context,
                              const unsigned char *data,
                              size_t length);
static int tpad_md5_update(void *context,
                           const unsigned char *data,
                           size_t length);
static char *tpad_hash_file_sha256(const char *path);
static char *tpad_hash_file_md5(const char *path);
static char *tpad_base64_encode_bytes(const unsigned char *data, size_t length);
static char *tpad_base64_decode_bytes(const unsigned char *data, size_t length);
static gboolean tpad_choose_file_contents(gchar **contents, gsize *length);

static char *tpad_hash_error(void)
{
	/* Public conversion helpers consistently return caller-owned strings. */
	return g_strdup("--\n");
}

char *data_to_hex(const void *pdata, size_t length)
{
	static const char hex[] = "0123456789abcdef";
	const unsigned char *data = (const unsigned char *) pdata;
	char *out;
	size_t n;

	if (data == NULL || length > (G_MAXSIZE - 1) / 2)
		return tpad_hash_error();

	out = g_try_malloc(length * 2 + 1);
	if (out == NULL)
		return tpad_hash_error();

	for (n = 0; n < length; n++) {
		out[n * 2] = hex[data[n] >> 4];
		out[n * 2 + 1] = hex[data[n] & 0x0f];
	}
	out[length * 2] = '\0';

	return out;
}

char *str2sha512(const char *str)
{
	unsigned char digest[64];
	size_t length;

	if (str == NULL)
		return tpad_hash_error();

	length = strlen(str);
	if (mbedtls_sha512_ret((const unsigned char *) str,
	                       length, digest, 0) != 0)
		return tpad_hash_error();

	return data_to_hex(digest, sizeof(digest));
}

char *str2sha256(const char *str)
{
	unsigned char digest[32];
	size_t length;

	if (str == NULL)
		return tpad_hash_error();

	length = strlen(str);
	if (mbedtls_sha256_ret((const unsigned char *) str,
	                       length, digest, 0) != 0)
		return tpad_hash_error();

	return data_to_hex(digest, sizeof(digest));
}

char *str2md5(const char *str)
{
	unsigned char digest[16];
	size_t length;

	if (str == NULL)
		return tpad_hash_error();

	length = strlen(str);
	if (mbedtls_md5_ret((const unsigned char *) str, length, digest) != 0)
		return tpad_hash_error();

	return data_to_hex(digest, sizeof(digest));
}

static gboolean tpad_hash_update_file(const char *path,
                                      void *context,
                                      TpadHashUpdate update)
{
	unsigned char buffer[16384];
	ssize_t bytes_read;
	int fd;

	if (path == NULL || path[0] == '\0' || context == NULL || update == NULL)
		return FALSE;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return FALSE;

	for (;;) {
		bytes_read = read(fd, buffer, sizeof(buffer));
		if (bytes_read > 0) {
			if (update(context, buffer, (size_t) bytes_read) != 0) {
				close(fd);
				return FALSE;
			}
			continue;
		}
		if (bytes_read == 0)
			break;
		if (errno == EINTR)
			continue;

		close(fd);
		return FALSE;
	}

	return close(fd) == 0;
}

static int tpad_sha512_update(void *context,
                              const unsigned char *data,
                              size_t length)
{
	return mbedtls_sha512_update_ret((mbedtls_sha512_context *) context,
	                                 data, length);
}

static int tpad_sha256_update(void *context,
                              const unsigned char *data,
                              size_t length)
{
	return mbedtls_sha256_update_ret((mbedtls_sha256_context *) context,
	                                 data, length);
}

static int tpad_md5_update(void *context,
                           const unsigned char *data,
                           size_t length)
{
	return mbedtls_md5_update_ret((mbedtls_md5_context *) context,
	                              data, length);
}

char *tpad_hash_file_sha512(const char *path)
{
	mbedtls_sha512_context context;
	unsigned char digest[64];
	char *result = NULL;

	mbedtls_sha512_init(&context);
	if (mbedtls_sha512_starts_ret(&context, 0) == 0 &&
	    tpad_hash_update_file(path, &context, tpad_sha512_update) &&
	    mbedtls_sha512_finish_ret(&context, digest) == 0)
		result = data_to_hex(digest, sizeof(digest));
	mbedtls_sha512_free(&context);

	return result != NULL ? result : tpad_hash_error();
}

static char *tpad_hash_file_sha256(const char *path)
{
	mbedtls_sha256_context context;
	unsigned char digest[32];
	char *result = NULL;

	mbedtls_sha256_init(&context);
	if (mbedtls_sha256_starts_ret(&context, 0) == 0 &&
	    tpad_hash_update_file(path, &context, tpad_sha256_update) &&
	    mbedtls_sha256_finish_ret(&context, digest) == 0)
		result = data_to_hex(digest, sizeof(digest));
	mbedtls_sha256_free(&context);

	return result != NULL ? result : tpad_hash_error();
}

static char *tpad_hash_file_md5(const char *path)
{
	mbedtls_md5_context context;
	unsigned char digest[16];
	char *result = NULL;

	mbedtls_md5_init(&context);
	if (mbedtls_md5_starts_ret(&context) == 0 &&
	    tpad_hash_update_file(path, &context, tpad_md5_update) &&
	    mbedtls_md5_finish_ret(&context, digest) == 0)
		result = data_to_hex(digest, sizeof(digest));
	mbedtls_md5_free(&context);

	return result != NULL ? result : tpad_hash_error();
}

char *curBin2sha512(void)
{
	return tpad_hash_file_sha512("/proc/self/exe");
}

char *curTxtbuff2sha512(void)
{
	GtkTextIter start, end;
	gchar *text;
	char *result;

	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(mBuff), &start, &end);
	text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),
	                                &start, &end, TRUE);
	result = str2sha512(text);
	g_free(text);
	return result;
}

char *curFile2sha512(void)
{
	gchar *path = tpad_fp_get_current();
	char *result;

	if (path == NULL)
		return tpad_hash_error();

	result = tpad_hash_file_sha512(path);
	g_free(path);
	return result;
}

char *tpad_hash_read_in_file(const char *fp)
{
	gchar *contents = NULL;
	GError *error = NULL;

	if (fp == NULL || fp[0] == '\0' ||
	    !g_file_get_contents(fp, &contents, NULL, &error)) {
		if (error != NULL)
			g_error_free(error);
		return tpad_hash_error();
	}

	return contents;
}

static char *tpad_base64_encode_bytes(const unsigned char *data, size_t length)
{
	unsigned char *encoded;
	size_t required = 0;
	size_t written = 0;
	int status;

	if (data == NULL && length != 0)
		return tpad_hash_error();
	if (length == 0)
		return g_strdup("");

	status = mbedtls_base64_encode(NULL, 0, &required, data, length);
	if (status != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL || required == 0 ||
	    required == G_MAXSIZE)
		return tpad_hash_error();

	encoded = g_try_malloc(required);
	if (encoded == NULL)
		return tpad_hash_error();

	status = mbedtls_base64_encode(encoded, required, &written, data, length);
	if (status != 0) {
		g_free(encoded);
		return tpad_hash_error();
	}
	/* mbedtls includes room for and writes the terminator. */
	encoded[written] = '\0';
	return (char *) encoded;
}

static char *tpad_base64_decode_bytes(const unsigned char *data, size_t length)
{
	unsigned char *decoded;
	size_t required = 0;
	size_t written = 0;
	int status;

	if (data == NULL && length != 0)
		return tpad_hash_error();
	if (length == 0)
		return g_strdup("");

	status = mbedtls_base64_decode(NULL, 0, &required, data, length);
	if (status != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL)
		return tpad_hash_error();
	if (required == G_MAXSIZE)
		return tpad_hash_error();

	decoded = g_try_malloc(required + 1);
	if (decoded == NULL)
		return tpad_hash_error();

	status = mbedtls_base64_decode(decoded, required, &written, data, length);
	if (status != 0) {
		g_free(decoded);
		return tpad_hash_error();
	}
	decoded[written] = '\0';
	return (char *) decoded;
}

char *strFrombase64(const char *str)
{
	if (str == NULL)
		return tpad_hash_error();
	return tpad_base64_decode_bytes((const unsigned char *) str, strlen(str));
}

char *str2base64(const char *str)
{
	if (str == NULL)
		return tpad_hash_error();
	return tpad_base64_encode_bytes((const unsigned char *) str, strlen(str));
}

char *tpad_hash_get_file(void)
{
	GtkWidget *dialog;
	gchar *current_path;
	gchar *selected_path = NULL;
	gint response;

	dialog = gtk_file_chooser_dialog_new("File hash to clipboard",
	                                     GTK_WINDOW(window),
	                                     GTK_FILE_CHOOSER_ACTION_OPEN,
	                                     "Cancel", GTK_RESPONSE_CANCEL,
	                                     "Select File", GTK_RESPONSE_ACCEPT,
	                                     NULL);
	gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dialog), TRUE);
	gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(dialog), TRUE);

	current_path = tpad_fp_get_current();
	if (current_path != NULL && current_path[0] != '\0')
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), current_path);
	g_free(current_path);

	response = gtk_dialog_run(GTK_DIALOG(dialog));
	if (response == GTK_RESPONSE_ACCEPT)
		selected_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

	gtk_widget_destroy(dialog);
	return selected_path;
}

static gboolean tpad_choose_file_contents(gchar **contents, gsize *length)
{
	gchar *path;
	GError *error = NULL;
	gboolean success;

	if (contents == NULL || length == NULL)
		return FALSE;
	*contents = NULL;
	*length = 0;

	path = tpad_hash_get_file();
	if (path == NULL)
		return FALSE;

	success = g_file_get_contents(path, contents, length, &error);
	g_free(path);
	if (error != NULL)
		g_error_free(error);

	return success;
}

char *file2sha512(void)
{
	gchar *path = tpad_hash_get_file();
	char *result;

	if (path == NULL)
		return tpad_hash_error();
	result = tpad_hash_file_sha512(path);
	g_free(path);
	return result;
}

char *file2sha256(void)
{
	gchar *path = tpad_hash_get_file();
	char *result;

	if (path == NULL)
		return tpad_hash_error();
	result = tpad_hash_file_sha256(path);
	g_free(path);
	return result;
}

char *file2md5(void)
{
	gchar *path = tpad_hash_get_file();
	char *result;

	if (path == NULL)
		return tpad_hash_error();
	result = tpad_hash_file_md5(path);
	g_free(path);
	return result;
}

char *file2base64(void)
{
	gchar *contents;
	gsize length;
	char *result;

	if (!tpad_choose_file_contents(&contents, &length))
		return tpad_hash_error();
	result = tpad_base64_encode_bytes((const unsigned char *) contents, length);
	g_free(contents);
	return result;
}

char *filefrombase64(void)
{
	gchar *contents;
	gsize length;
	char *result;

	if (!tpad_choose_file_contents(&contents, &length))
		return tpad_hash_error();
	result = tpad_base64_decode_bytes((const unsigned char *) contents, length);
	g_free(contents);
	return result;
}

char *tpad_hash_choose_file_and_get_contents(void)
{
	gchar *contents;
	gsize length;

	if (!tpad_choose_file_contents(&contents, &length))
		return NULL;
	return contents;
}
