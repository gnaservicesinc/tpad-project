/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013-2026 Andrew Smith (GNA SERVICES INC)
 *     <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_open_guard.c, is part of tpad.
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

extern gchar *origfile;
extern int madetemp;

gchar *path_temp_file = NULL;

static gint check_guard_file(const gchar *current, const gchar *candidate)
{
	if (candidate != NULL && g_file_test(candidate, G_FILE_TEST_EXISTS)) {
		gchar *display_candidate = tpad_filename_to_utf8(candidate);
		gint result = gerror_openguard_popup(
			(gchar *) current,
			display_candidate != NULL ? display_candidate : (gchar *) candidate);

		g_free(display_candidate);
		return result;
	}
	return 1;
}

void tpad_open_guard_cleanup(void)
{
	(void) tpad_cp_remove_temp();
	g_clear_pointer(&path_temp_file, g_free);
}

int tpad_open_guard_check_path(const gchar *current)
{
	gchar *filename;
	gchar *directory;
	gchar *basename;
	gchar *tpad_backup;
	gchar *vim_swap;
	gchar *office_lock;
	gchar *emacs_backup;
	gint proceed = 1;

	if (current == NULL || *current == '\0')
		return 1;

	filename = tpad_filename_from_utf8(current);
	if (filename == NULL)
		return 0;
	directory = g_path_get_dirname(filename);
	basename = g_path_get_basename(filename);
	tpad_backup = g_strconcat(filename, "~", NULL);
	vim_swap = g_strdup_printf("%s/.%s.swp", directory, basename);
	office_lock = g_strdup_printf("%s/.~lock.%s#", directory, basename);
	emacs_backup = g_strdup_printf("%s/#%s#", directory, basename);

	if (g_file_test(tpad_backup, G_FILE_TEST_EXISTS))
		proceed = check_guard_file(current, tpad_backup);
	else if (g_file_test(vim_swap, G_FILE_TEST_EXISTS))
		proceed = check_guard_file(current, vim_swap);
	else if (g_file_test(office_lock, G_FILE_TEST_EXISTS))
		proceed = check_guard_file(current, office_lock);
	else if (g_file_test(emacs_backup, G_FILE_TEST_EXISTS))
		proceed = check_guard_file(current, emacs_backup);

	g_free(emacs_backup);
	g_free(office_lock);
	g_free(vim_swap);
	g_free(tpad_backup);
	g_free(basename);
	g_free(directory);
	g_free(filename);
	return proceed;
}

gboolean tpad_open_guard_track_path(const gchar *current)
{
	gchar *filename;
	gboolean tracked;

	if (current == NULL || *current == '\0')
		return FALSE;

	tpad_open_guard_cleanup();
	filename = tpad_filename_from_utf8(current);
	if (filename == NULL)
		return FALSE;
	path_temp_file = g_strconcat(filename, "~", NULL);
	if (g_file_test(path_temp_file, G_FILE_TEST_EXISTS)) {
		g_clear_pointer(&path_temp_file, g_free);
		g_free(filename);
		return FALSE;
	}
	tracked = tpad_copy(filename, path_temp_file) == 0;
	if (!tracked)
		g_clear_pointer(&path_temp_file, g_free);
	g_free(filename);
	return tracked;
}

gboolean tpad_open_guard_apply_enabled(gboolean enabled,
	const gchar *current)
{
	/* Make the setting a runtime state transition, not merely a value that
	 * will be consulted the next time a document is opened. */
	if (enabled && current != NULL && *current != '\0' && madetemp &&
	    g_strcmp0(origfile, current) == 0 && path_temp_file != NULL &&
	    g_file_test(path_temp_file, G_FILE_TEST_EXISTS))
		return TRUE;

	tpad_open_guard_cleanup();
	madetemp = 0;
	g_clear_pointer(&origfile, g_free);

	/* An untitled document has nothing to guard yet, but the preference can
	 * remain enabled and will be applied when the document gets a path. */
	if (!enabled || current == NULL || *current == '\0')
		return enabled;

	if (!tpad_open_guard_check_path(current) ||
	    !tpad_open_guard_track_path(current)) {
		tpad_open_guard_cleanup();
		return FALSE;
	}

	madetemp = 1;
	origfile = g_strdup(current);
	return TRUE;
}
