/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_find.c , is part of tpad.
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

extern gchar *origfile;
extern GtkWidget *window;
static gboolean shutdown_started;

//extern int tpad_wach_thread_set;
//extern int run_count_update;

static void clean_up_and_destroy_window(void)
{
	GtkWidget *closing_window = window;

	if (shutdown_started)
		return;
	shutdown_started = TRUE;
	cfg_on_exit();
	tpad_open_guard_cleanup();
	tpad_clipboards_store();
	g_clear_pointer(&content, g_free);
	tpad_free_spelling();
	g_clear_pointer(&origfile, g_free);
	tpad_control_cleanup();
	tpad_fp_cleanup();
	set_path_self_cleanup();
	window = NULL;
	if (closing_window != NULL)
		gtk_window_destroy(GTK_WINDOW(closing_window));
}

void force_quit_program(void){
	clean_up_and_destroy_window();
}

void quit_program(void){
	//if(tpad_wach_thread_set) tpad_watch_exit();
	if (save_modified())
		clean_up_and_destroy_window();
}
