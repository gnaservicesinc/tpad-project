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

gchar *path_temp_file = NULL;

static gint check_guard_file(const gchar *current, const gchar *candidate)
{
	if (candidate != NULL && g_file_test(candidate, G_FILE_TEST_EXISTS))
		return gerror_openguard_popup((gchar *) current,
		                              (gchar *) candidate);
	return 1;
}

void tpad_open_guard_cleanup(void)
{
	(void) tpad_cp_remove_temp();
	g_clear_pointer(&path_temp_file, g_free);
}

int tpad_open_guard_check_path(const gchar *current)
{
	gchar *directory;
	gchar *basename;
	gchar *tpad_backup;
	gchar *vim_swap;
	gchar *office_lock;
	gchar *emacs_backup;
	gint proceed = 1;

	if (current == NULL || *current == '\0')
		return 1;

	directory = g_path_get_dirname(current);
	basename = g_path_get_basename(current);
	tpad_backup = g_strconcat(current, "~", NULL);
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
	return proceed;
}

gboolean tpad_open_guard_track_path(const gchar *current)
{
	if (current == NULL || *current == '\0')
		return FALSE;

	tpad_open_guard_cleanup();
	path_temp_file = g_strconcat(current, "~", NULL);
	if (g_file_test(path_temp_file, G_FILE_TEST_EXISTS))
		return FALSE;
	return tpad_copy((char *) current, path_temp_file) == 0;
}
