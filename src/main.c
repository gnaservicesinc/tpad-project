/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014-2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, main.c , is part of tpad.
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
 ******************************************************************************/
////////////////////////////////////////////////////////////////////////
#include "tpad_headers.h"
#ifdef G_OS_WIN32
extern gchar *prefix;
extern gchar *localedir;
#endif

#ifndef G_OS_WIN32
static int fork_tpad_window(const char *argument)
{
	gchar *path = argument != NULL ? clean_path((gchar *) argument) : NULL;
	pid_t child;

	if (argument != NULL && path == NULL) {
		g_warning("Unable to convert file name to UTF-8: %s", argument);
		return -1;
	}

	/* Fork while the launcher is still single-threaded and before GTK is
	 * initialized.  This preserves tpad's historical non-blocking command
	 * line behavior without the unsafe worker-thread-then-fork sequence. */
	child = fork();
	if (child < 0) {
		g_warning("Unable to start tpad: %s", g_strerror(errno));
		g_free(path);
		return -1;
	}

	if (child > 0) {
		g_free(path);
		return 0;
	}

	int result = tpad_main(path);
	g_free(path);
	_exit(result == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
#endif

// Wrapper for tpad_main to avoid implicit declaration
int tpad_main(char* fchar) {
    extern int tpad_main_impl(char* fchar);
    return tpad_main_impl(fchar);
}
////////////////////////////////////////////////////////////////////////
/* MAIN  */
////////////////////////////////////////////////////////////////////////
extern int main(int argc, char* argv[])
{
	/* Filename conversion and gettext both depend on the environment locale.
	 * A process otherwise begins in the C locale, even when LC_ALL is set. */
	(void) setlocale(LC_ALL, "");

	////////////////////////////////////////////////////////////////////////
	// Native Language Support Int
	////////////////////////////////////////////////////////////////////////
#ifdef G_OS_WIN32
	gchar *prefix = g_win32_get_package_installation_directory_of_module (NULL);
	gchar *localedir = g_build_filename (prefix, "share", "locale", NULL);
#endif

#ifdef ENABLE_NLS

# ifndef G_OS_WIN32
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
# else
	bindtextdomain (GETTEXT_PACKAGE, localedir);
# endif
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	////////////////////////////////////////////////////////////////////////
	SET_BINARY_MODE(stdin);
	SET_BINARY_MODE(stdout);

	// Record the path to ourself.

	set_path_self(argv[0]);


#ifdef G_OS_WIN32
	/* Windows has no fork().  Keep one window per file, with the first
	 * window hosted by the original process. */
	for (int i = 2; i < argc; i++) {
		gchar *path = clean_path(argv[i]);
		if (new_thread_tpad(path) != 0)
			g_warning("Unable to open an additional tpad window");
		g_free(path);
	}

	////////////////////////////////////////////////////////////////////////
	/* MAIN RETURN */
	////////////////////////////////////////////////////////////////////////


	gchar *path = argc > 1 ? clean_path(argv[1]) : NULL;
	int result = tpad_main(path);
	g_free(path);
	return result;
#else
	int result = 0;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			if (fork_tpad_window(argv[i]) != 0)
				result = 1;
		}
	} else if (fork_tpad_window(NULL) != 0) {
		result = 1;
	}

	return result;
#endif
	////////////////////////////////////////////////////////////////////////
}
