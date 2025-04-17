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
pthread_mutex_t ntpad_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_var   = PTHREAD_COND_INITIALIZER;
////////////////////////////////////////////////////////////////////////
#ifdef G_OS_WIN32
extern gchar *prefix;
extern gchar *localedir;
#endif
// Forward declaration
static void* ntpad(void *ptr);

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


	// Fork off a thread to tpad_main for each argument passed and exit
        // or fork off a thead with no arguments to tpad_main and exit.

	// Declare an array of threads as well as a int for return code the size of argc plus one.

	pthread_t thread[argc + 1];
	gint  iThreadReturn[argc + 1];

	if (argc > 1){
		int i=0;
		for (i = 1; i < argc; i += 1){
		iThreadReturn[i] = pthread_create(&thread[i],
	                               NULL,
	                               ntpad,
	                               (void*) clean_path(argv[i]) );
		sleep(1);
	pthread_detach(thread[i]);
		}
	}
	else{
		iThreadReturn[0] = pthread_create(&thread[0],
	                               NULL,
	                               ntpad,
	                               (void*) "" );
		sleep(1);
	pthread_detach(thread[0]);
	}

	////////////////////////////////////////////////////////////////////////
	/* MAIN RETURN */
	////////////////////////////////////////////////////////////////////////


	return (0);
	////////////////////////////////////////////////////////////////////////
}

static void* ntpad(void *ptr){

   pthread_mutex_lock( &ntpad_mutex );

	char* ptrargv = (char*) ptr;
	if(fork() == 0) {
	 tpad_main(ptrargv);
	}
#ifdef G_OS_WIN32
	g_free (prefix);
	g_free (localedir);
#endif
	pthread_mutex_unlock( &ntpad_mutex );

  	pthread_exit( (void*) NULL );
}