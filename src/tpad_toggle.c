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
extern GtkSourceView *view;

gboolean tpad_set_use_open_guard(gboolean enabled){
	gchar *current = tpad_fp_get_current();
	gboolean applied = tpad_open_guard_apply_enabled(enabled, current);

	cfg_set_use_open_guard((int) applied);
	cfg_save();
	g_free(current);
	return applied;
}
////////////////////////////////////////////////////////////////////
void tpad_set_line_wrap(gboolean enabled){
	if (view != NULL)
		gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),
		                            enabled ? GTK_WRAP_WORD : GTK_WRAP_NONE);
	cfg_set_show_line_wrap((int) enabled);
	cfg_save();
}
////////////////////////////////////////////////////////////////////

void tpad_set_show_full_path(gboolean enabled){
	gchar *current;

	cfg_set_show_full_path((int) enabled);
	current = tpad_fp_get_current();
	if (current != NULL)
		set_title();
	g_free(current);
	cfg_save();
}
////////////////////////////////////////////////////////////////////
#ifdef AUTO_TAB_TOGGLE

void tpad_set_auto_tab(gboolean enabled){
	if (view != NULL)
		gtk_source_view_set_auto_indent(view, enabled);
	cfg_set_auto_tab((int) enabled);
	cfg_save();
}
#endif
void toggle_covt (GtkWidget *caller){
doCOVT=gtk_check_button_get_active(GTK_CHECK_BUTTON(caller));
}

void toggle_case_sarch (GtkWidget *caller){
 searchCase=gtk_check_button_get_active(GTK_CHECK_BUTTON(caller));
}

void tpad_set_line_numbers(gboolean enabled){
	if (view != NULL)
		gtk_source_view_set_show_line_numbers(view, enabled);
	cfg_set_show_line((int) enabled);
	cfg_save();
}
