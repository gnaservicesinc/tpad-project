/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2026 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_recent.c, is part of tpad.
 *
 *   tpad is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 ********************************************************************************/

#include "tpad_headers.h"

#include <glib/gstdio.h>
#include <sys/file.h>

#define TPAD_RECENT_FILE_SUFFIX "/.tpad.recent"
#define TPAD_RECENT_HEADER "# Tpad recent files v1\n"

static gchar *recent_file_path(void)
{
	const gchar *override = g_getenv("TPAD_RECENT_FILE");
	const gchar *home = g_get_home_dir();

	if (override != NULL && override[0] != '\0' &&
	    g_path_is_absolute(override))
		return g_strdup(override);
	if (home == NULL)
		home = ".";
	return g_strconcat(home, TPAD_RECENT_FILE_SUFFIX, NULL);
}

static gchar *normalize_path(const gchar *path)
{
	gchar *filename;
	gchar *resolved;
	gchar *working_directory;
	gchar *utf8_working_directory;
	gchar *absolute;

	if (path == NULL || path[0] == '\0')
		return NULL;
	/* g_canonicalize_filename() requires GLib 2.58, while Tpad still builds
	 * on distributions with GLib 2.40.  realpath() gives existing files the
	 * same canonical treatment; retain an absolute spelling for a stale
	 * legacy entry whose target no longer exists. */
	filename = tpad_filename_from_utf8(path);
	if (filename == NULL)
		return NULL;
	resolved = realpath(filename, NULL);
	g_free(filename);
	if (resolved != NULL) {
		absolute = tpad_filename_to_utf8(resolved);
		free(resolved);
		return absolute;
	}
	if (g_path_is_absolute(path))
		return g_strdup(path);
	working_directory = g_get_current_dir();
	utf8_working_directory = tpad_filename_to_utf8(working_directory);
	g_free(working_directory);
	if (utf8_working_directory == NULL)
		return NULL;
	absolute = g_build_filename(utf8_working_directory, path, NULL);
	g_free(utf8_working_directory);
	return absolute;
}

static gboolean load_from_path(const gchar *list_path, GPtrArray *files,
	                           GError **error)
{
	gchar *contents = NULL;
	gchar **lines;
	GHashTable *seen;
	GError *local_error = NULL;

	if (!g_file_get_contents(list_path, &contents, NULL, &local_error)) {
		if (g_error_matches(local_error, G_FILE_ERROR, G_FILE_ERROR_NOENT)) {
			g_clear_error(&local_error);
			return TRUE;
		}
		g_propagate_error(error, local_error);
		return FALSE;
	}

	seen = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
	lines = g_strsplit(contents, "\n", -1);
	for (gsize index = 0; lines[index] != NULL; index++) {
		gchar *line = g_strstrip(lines[index]);
		gchar *decoded = NULL;
		gchar *canonical;

		if (line[0] == '\0' || line[0] == '#')
			continue;
		if (g_str_has_prefix(line, "file:")) {
			gchar *filename = g_filename_from_uri(line, NULL, NULL);

			if (filename != NULL) {
				decoded = tpad_filename_to_utf8(filename);
				g_free(filename);
			}
		}
		else if (g_path_is_absolute(line))
			decoded = tpad_filename_to_utf8(line);
		if (decoded == NULL)
			continue;
		canonical = normalize_path(decoded);
		g_free(decoded);
		if (canonical == NULL || g_hash_table_contains(seen, canonical)) {
			g_free(canonical);
			continue;
		}
		g_hash_table_add(seen, g_strdup(canonical));
		g_ptr_array_add(files, canonical);
	}

	g_strfreev(lines);
	g_hash_table_unref(seen);
	g_free(contents);
	return TRUE;
}

static gboolean set_file_error(GError **error, const gchar *operation,
	                           const gchar *path, gint error_number)
{
	if (error != NULL && *error == NULL)
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(error_number),
		            "%s '%s': %s", operation, path,
		            g_strerror(error_number));
	return FALSE;
}

static gboolean write_all(gint descriptor, const gchar *data, gsize length)
{
	gsize offset = 0;

	while (offset < length) {
		ssize_t written = write(descriptor, data + offset, length - offset);
		if (written > 0) {
			offset += (gsize) written;
			continue;
		}
		if (written < 0 && errno == EINTR)
			continue;
		if (written == 0)
			errno = EIO;
		return FALSE;
	}
	return TRUE;
}

static gboolean write_to_path(const gchar *list_path, GPtrArray *files,
	                          GError **error)
{
	GString *serialized = g_string_new(TPAD_RECENT_HEADER);
	gchar *temporary = g_strconcat(list_path, ".tmp-XXXXXX", NULL);
	gboolean success = FALSE;
	gboolean temporary_exists = FALSE;
	gint descriptor = -1;

	for (guint index = 0; index < files->len; index++) {
		const gchar *path = g_ptr_array_index(files, index);
		gchar *filename = tpad_filename_from_utf8(path);
		gchar *uri = filename != NULL
			? g_filename_to_uri(filename, NULL, NULL) : NULL;

		g_free(filename);
		if (uri == NULL)
			continue;
		g_string_append(serialized, uri);
		g_string_append_c(serialized, '\n');
		g_free(uri);
	}

	descriptor = g_mkstemp_full(temporary, O_RDWR, 0600);
	if (descriptor < 0) {
		set_file_error(error, "Unable to create recent-files list",
		               list_path, errno);
		goto cleanup;
	}
	temporary_exists = TRUE;
	(void) fcntl(descriptor, F_SETFD, FD_CLOEXEC);
	if (!write_all(descriptor, serialized->str, serialized->len)) {
		set_file_error(error, "Unable to write recent-files list",
		               list_path, errno);
		goto cleanup;
	}
	if (fsync(descriptor) != 0) {
		set_file_error(error, "Unable to synchronize recent-files list",
		               list_path, errno);
		goto cleanup;
	}
	if (close(descriptor) != 0) {
		descriptor = -1;
		set_file_error(error, "Unable to close recent-files list",
		               list_path, errno);
		goto cleanup;
	}
	descriptor = -1;
	if (g_rename(temporary, list_path) != 0) {
		set_file_error(error, "Unable to replace recent-files list",
		               list_path, errno);
		goto cleanup;
	}
	temporary_exists = FALSE;
	success = TRUE;

cleanup:
	if (descriptor >= 0)
		(void) close(descriptor);
	if (temporary_exists)
		(void) g_unlink(temporary);
	g_free(temporary);
	g_string_free(serialized, TRUE);
	return success;
}

static gint lock_recent_file(const gchar *list_path, GError **error)
{
	gchar *lock_path = g_strconcat(list_path, ".lock", NULL);
	gint open_flags = O_CREAT | O_RDWR;
	struct stat lock_status;
	gint stat_result;
	#ifdef O_NOFOLLOW
	open_flags |= O_NOFOLLOW;
	#endif
	gint descriptor = g_open(lock_path, open_flags, 0600);
	gint result;

	if (descriptor < 0) {
		set_file_error(error, "Unable to open recent-files lock",
		               lock_path, errno);
		g_free(lock_path);
		return -1;
	}
	stat_result = fstat(descriptor, &lock_status);
	if (stat_result != 0 || !S_ISREG(lock_status.st_mode)) {
		gint saved_errno = stat_result != 0 ? errno : EINVAL;
		set_file_error(error, "Unsafe recent-files lock",
		               lock_path, saved_errno);
		(void) close(descriptor);
		g_free(lock_path);
		return -1;
	}
	(void) fcntl(descriptor, F_SETFD, FD_CLOEXEC);
	do {
		result = flock(descriptor, LOCK_EX);
	} while (result != 0 && errno == EINTR);
	if (result != 0) {
		set_file_error(error, "Unable to lock recent-files list",
		               lock_path, errno);
		(void) close(descriptor);
		descriptor = -1;
	}
	g_free(lock_path);
	return descriptor;
}

static void unlock_recent_file(gint descriptor)
{
	if (descriptor < 0)
		return;
	(void) flock(descriptor, LOCK_UN);
	(void) close(descriptor);
}

static void apply_limit(GPtrArray *files, guint maximum)
{
	if (maximum == 0)
		return;
	while (files->len > maximum)
		g_ptr_array_remove_index(files, files->len - 1);
}

GPtrArray *tpad_recent_files_load(GError **error)
{
	GPtrArray *files = g_ptr_array_new_with_free_func(g_free);
	gchar *list_path = recent_file_path();

	if (!load_from_path(list_path, files, error))
		g_ptr_array_set_size(files, 0);
	g_free(list_path);
	return files;
}

gboolean tpad_recent_files_add(const gchar *path, gboolean enabled,
	                           guint maximum, GError **error)
{
	gchar *canonical;
	gchar *list_path;
	GPtrArray *files;
	gboolean success = FALSE;
	gint lock_descriptor;

	if (!enabled)
		return TRUE;
	if (maximum > TPAD_RECENT_FILES_MAXIMUM)
		maximum = TPAD_RECENT_FILES_MAXIMUM;
	canonical = normalize_path(path);
	if (canonical == NULL)
		return set_file_error(error, "Invalid recent file", "(empty path)",
		                      EINVAL);
	list_path = recent_file_path();
	lock_descriptor = lock_recent_file(list_path, error);
	if (lock_descriptor < 0)
		goto cleanup_path;
	files = g_ptr_array_new_with_free_func(g_free);
	if (!load_from_path(list_path, files, error))
		goto cleanup_files;
	for (guint index = 0; index < files->len; index++) {
		if (g_strcmp0(g_ptr_array_index(files, index), canonical) == 0) {
			g_ptr_array_remove_index(files, index);
			break;
		}
	}
	g_ptr_array_insert(files, 0, canonical);
	canonical = NULL;
	apply_limit(files, maximum);
	success = write_to_path(list_path, files, error);

cleanup_files:
	g_ptr_array_unref(files);
	unlock_recent_file(lock_descriptor);
cleanup_path:
	g_free(list_path);
	g_free(canonical);
	return success;
}

gboolean tpad_recent_files_trim(guint maximum, GError **error)
{
	gchar *list_path = recent_file_path();
	GPtrArray *files;
	gboolean success = FALSE;
	gint lock_descriptor;

	if (maximum == 0 || !g_file_test(list_path, G_FILE_TEST_EXISTS)) {
		g_free(list_path);
		return TRUE;
	}
	if (maximum > TPAD_RECENT_FILES_MAXIMUM)
		maximum = TPAD_RECENT_FILES_MAXIMUM;
	lock_descriptor = lock_recent_file(list_path, error);

	if (lock_descriptor < 0)
		goto cleanup_path;
	files = g_ptr_array_new_with_free_func(g_free);
	if (!load_from_path(list_path, files, error))
		goto cleanup_files;
	apply_limit(files, maximum);
	success = write_to_path(list_path, files, error);

cleanup_files:
	g_ptr_array_unref(files);
	unlock_recent_file(lock_descriptor);
cleanup_path:
	g_free(list_path);
	return success;
}
