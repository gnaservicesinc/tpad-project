/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_toggle.c , is part of tpad.
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

extern gboolean searchCase;
extern gboolean doCOVT;
extern GtkWidget *window;
extern GtkSourceView *view;

 #ifdef HAVE_LIBMAGIC
void toggle_use_magic_settings(GtkWidget *caller){
    cfg_set_use_magic ((int)gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(caller)));
	cfg_save();
}
#endif
////////////////////////////////////////////////////////////////////
void toggle_use_open_guard(GtkWidget *caller){
    cfg_set_use_open_guard ((int)gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(caller)));
	cfg_save();
}
////////////////////////////////////////////////////////////////////
void toggle_line_wrap(GtkWidget *caller){
	(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(caller))) ? gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view),GTK_WRAP_WORD) : gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view),GTK_WRAP_NONE);
	(gtk_text_view_get_wrap_mode (GTK_TEXT_VIEW (view))) ? cfg_set_show_line_wrap(1) : cfg_set_show_line_wrap(0);
	cfg_save();
}
////////////////////////////////////////////////////////////////////

void toggle_show_full_path(GtkWidget *caller){
 (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(caller))) ? cfg_set_show_full_path(1) : cfg_set_show_full_path(0);	
if(tpad_fp_get_current()) set_title();
}
////////////////////////////////////////////////////////////////////
#ifdef AUTO_TAB_TOGGLE

void toggle_auto_tab(GtkWidget *caller){
	gboolean state=gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(caller));
	gtk_source_view_set_auto_indent(GTK_SOURCE_VIEW(view),(gboolean)state);
	cfg_set_auto_tab((int) state);
	cfg_save();
}
#endif
void toggle_covt (GtkWidget *caller){
doCOVT=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(caller));
}

void toggle_case_sarch (GtkWidget *caller){
 searchCase=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(caller));
}

void toggle_linenumber(GtkWidget *caller){
	gboolean state=gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(caller));
	gtk_source_view_set_show_line_numbers (view,(gboolean)state);
	cfg_set_show_line((int) state);
	cfg_save();
}


void toggle_keep_above(GtkWidget *caller){
	gtk_window_set_keep_above (GTK_WINDOW(window),gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(caller)));
	cfg_save();
}