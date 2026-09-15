/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014-2026 Andrew Smith (GNA SERVICES INC) <andresmi@icloud.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_main.c, is part of tpad.
 *
 *   tpad is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 ********************************************************************************/

#include "tpad_headers.h"

GtkWidget *window = NULL;
GtkSourceView *view = NULL;
G_LOCK_DEFINE(mBuff);
GtkSourceBuffer *mBuff;
unsigned int gbid = 0;
gchar *content;
gchar *origfile;
gboolean save_locked;
int tpad_wach_thread_set = 0;
int setModified = 0;
int madetemp = 0;
int run_count_update = 1;
gboolean disable_file_watch = FALSE;
gpointer current_data = NULL;

static void tpad_application_startup(GApplication *application,
	                                 gpointer user_data)
{
	(void) application;
	(void) user_data;
	gtk_source_init();
	spelling_init();
}

static void tpad_application_activate(GtkApplication *application,
	                                  gpointer user_data)
{
	const gchar *filename = user_data;

	config_setup();
	tpad_fp_init();
	(void) tpad_new_ui(application);
	new_file();
	if (filename != NULL && *filename != '\0')
		(void) show_file((gchar *) filename);

	printf("\nOpening file:\t%s.\n"
	       "tpad License and Copyright Notice:\n\n"
	       "Copyright (C) 2012-2026 Andrew Smith (GNA SERVICES INC) "
	       "<Andrew@GNAServicesInc.com>\n\n"
	       "tpad is free software: you can redistribute it and/or modify it\n"
	       "under the terms of the GNU General Public License as published by "
	       "the\nFree Software Foundation, either version 3 of the License, or\n"
	       "(at your option) any later version.\n\n"
	       "tpad is distributed in the hope that it will be useful, but\n"
	       "WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
	       "See the GNU General Public License for more details.\n\n"
	       "You should have received a copy of the GNU General Public License "
	       "along\nwith this program. If not, see <http://www.gnu.org/licenses/>.\n",
	       filename != NULL ? filename : "");
	gtk_window_present(GTK_WINDOW(window));
}

int tpad_main_impl(char *fchar)
{
	GtkApplication *application;
	const gchar *application_id = "com.gnaservicesinc.tpad";
	int status;

	/* Tpad does not play media or use accelerated/3D content.  Avoid probing
	 * optional media and GPU stacks that are unnecessary for a text editor.
	 * Keep an explicit user choice intact for diagnostic or accessibility
	 * purposes. */
#ifdef __linux__
	(void) g_setenv("GTK_MEDIA", "none", FALSE);
	(void) g_setenv("GSK_RENDERER", "cairo", FALSE);
#endif

	/* GtkApplication exports its actions on the session bus even in
	 * NON_UNIQUE mode.  Confined Snaps neither need nor permit that desktop
	 * shell introspection, so omit the ID there and avoid exporting it. */
	if (g_getenv("SNAP") != NULL)
		application_id = NULL;

	/* Tpad deliberately launches one process per document. NON_UNIQUE keeps
	 * GtkApplication from forwarding later processes to the first instance. */
	application = gtk_application_new(application_id,
	                                  G_APPLICATION_NON_UNIQUE);
	g_signal_connect(application, "startup",
	                 G_CALLBACK(tpad_application_startup), NULL);
	g_signal_connect(application, "activate",
	                 G_CALLBACK(tpad_application_activate), fchar);
	status = g_application_run(G_APPLICATION(application), 0, NULL);
	g_object_unref(application);
	g_clear_object(&mBuff);
	view = NULL;
	window = NULL;
	gtk_source_finalize();
	return status;
}
