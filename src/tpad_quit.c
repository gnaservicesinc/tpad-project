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


//extern int tpad_wach_thread_set;
//extern int run_count_update;

void force_quit_program(){
		cfg_on_exit();
		tpad_cp_remove_temp();
		gtk_clipboard_store (gtk_clipboard_get(GDK_SELECTION_PRIMARY));
		gtk_clipboard_store (gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
		tpad_fp_cleanup();
		set_path_self_cleanup();
		//ui_unity_destroy();
		gtk_main_quit();
		exit(0);

}

void quit_program(){
	cfg_on_exit();
	//if(tpad_wach_thread_set) tpad_watch_exit();
	if (save_modified()){
		tpad_cp_remove_temp();
		gtk_clipboard_store (gtk_clipboard_get(GDK_SELECTION_PRIMARY));
		gtk_clipboard_store (gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
		//run_count_update=0;
		//g_free(content);
		content=NULL;
		tpad_free_spelling();
		tpad_fp_cleanup();
		set_path_self_cleanup();
		//ui_unity_destroy();
		gtk_main_quit();
		exit(0);
		}
}
