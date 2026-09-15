/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_pref.c , is part of tpad.
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
static GtkSpinButton *xSpinButton,*ySpinButton,*uSpinButton,*recentSpinButton;
static GtkWidget *recentFilesCheck;
static GtkWidget *preferences_window = NULL;
static gboolean recentEnabledChanged;
static gboolean recentLimitChanged;

static void recent_files_toggled(GtkToggleButton *button, gpointer user_data)
{
	(void) user_data;
	gtk_widget_set_sensitive(GTK_WIDGET(recentSpinButton),
	                         gtk_toggle_button_get_active(button));
	recentEnabledChanged = TRUE;
}

static void recent_limit_changed(GtkSpinButton *button, gpointer user_data)
{
	(void) button;
	(void) user_data;
	recentLimitChanged = TRUE;
}

static gboolean preferences_key_pressed(GtkWidget *dialog,
	GdkEventKey *event, gpointer user_data)
{
	(void) user_data;
	if (event->keyval != GDK_KEY_Escape)
		return FALSE;
	gtk_widget_destroy(dialog);
	return TRUE;
}

static void preferences_destroyed(GtkWidget *dialog, gpointer user_data)
{
	(void) dialog;
	(void) user_data;
	preferences_window = NULL;
	xSpinButton = NULL;
	ySpinButton = NULL;
	uSpinButton = NULL;
	recentSpinButton = NULL;
	recentFilesCheck = NULL;
	recentEnabledChanged = FALSE;
	recentLimitChanged = FALSE;
}


void do_UI_pref_update(GtkWidget *caller, gpointer dialog)
{
	const gboolean old_recent_enabled = cfg_recent_files_enabled();
	const int old_recent_limit = cfg_recent_files_limit();
	const gboolean new_recent_enabled = gtk_toggle_button_get_active(
		GTK_TOGGLE_BUTTON(recentFilesCheck));
	const int new_recent_limit = gtk_spin_button_get_value_as_int(
		recentSpinButton);
	const gboolean applied_recent_enabled = recentEnabledChanged
		? new_recent_enabled : old_recent_enabled;
	const int applied_recent_limit = recentLimitChanged
		? new_recent_limit : old_recent_limit;
	const gboolean should_trim = applied_recent_enabled &&
		(!old_recent_enabled ||
		 (applied_recent_limit > 0 &&
		  (old_recent_limit == 0 ||
		   applied_recent_limit < old_recent_limit)));
	GError *error = NULL;

	(void) caller;
	(void) dialog;
	cfg_set_default_width(gtk_spin_button_get_value_as_int (xSpinButton));
	cfg_set_default_height(gtk_spin_button_get_value_as_int (ySpinButton));
	cfg_set_undo(gtk_spin_button_get_value_as_int (uSpinButton));
	if (recentEnabledChanged)
		cfg_set_recent_files_enabled(new_recent_enabled);
	if (recentLimitChanged)
		cfg_set_recent_files_limit(new_recent_limit);
	tpad_ui_refresh_recent_menu();
	if (should_trim &&
	    !tpad_recent_files_trim((guint) applied_recent_limit, &error)) {
		g_warning("Unable to apply the recent-files limit: %s",
		          error != NULL ? error->message : "unknown error");
		g_clear_error(&error);
	}
	//cfg_set_stack_quantity(gtk_spin_button_get_value_as_int (stSpingButton));
	set_language();
	//gtk_widget_destroy((GtkWidget*) dialog);
	cfg_save();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(recentFilesCheck),
	                             applied_recent_enabled);
	gtk_spin_button_set_value(recentSpinButton,
	                          (gdouble)applied_recent_limit);
	recentEnabledChanged = FALSE;
	recentLimitChanged = FALSE;
}
void show_ui_prefs(void){

	GtkWidget *dialog,*mainbox,*buttonbox,*xbox,*ybox,*ubox,*stbox,
	          *recentCheckBox,*recentLimitBox;
	if (preferences_window != NULL) {
		gtk_window_present(GTK_WINDOW(preferences_window));
		return;
	}
	dialog=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	preferences_window = dialog;
	g_signal_connect(dialog, "destroy",
	                 G_CALLBACK(preferences_destroyed), NULL);
	g_signal_connect(dialog, "key-press-event",
	                 G_CALLBACK(preferences_key_pressed), NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),_UI_SETTINGS);
    gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW(dialog),FALSE);
	mainbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	xbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	ybox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	ubox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	stbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	recentCheckBox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	recentLimitBox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	buttonbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

	gtk_container_add(GTK_CONTAINER(dialog),mainbox);
	gtk_container_add(GTK_CONTAINER(mainbox),xbox);
	gtk_container_add(GTK_CONTAINER(mainbox),ybox);
	gtk_container_add(GTK_CONTAINER(mainbox),ubox);
	gtk_container_add(GTK_CONTAINER(mainbox),stbox);
	gtk_container_add(GTK_CONTAINER(mainbox),recentCheckBox);
	gtk_container_add(GTK_CONTAINER(mainbox),recentLimitBox);
	gtk_container_add(GTK_CONTAINER(mainbox),buttonbox);

	gtk_box_pack_start (GTK_BOX (xbox),GTK_WIDGET (gtk_label_new(_DEFAULT_WIDTH)), TRUE, TRUE, 1);
	xSpinButton=(GtkSpinButton*)gtk_spin_button_new_with_range(
		_TPAD_CFG_WIDTH_MIN, WIDTH_MAX, 10);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON(xSpinButton),0);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(xSpinButton), TRUE);
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON(xSpinButton),TRUE);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(xSpinButton),(gdouble)cfg_wWidth());
	gtk_box_pack_start (GTK_BOX (xbox),GTK_WIDGET(xSpinButton),TRUE,TRUE,1);


	gtk_box_pack_start (GTK_BOX (ybox),GTK_WIDGET (gtk_label_new(_DEFAULT_HEIGHT)), TRUE, TRUE, 1);
	ySpinButton=(GtkSpinButton*)gtk_spin_button_new_with_range(
		_TPAD_CFG_HEIGHT_MIN, HEIGHT_MAX, 10);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON(ySpinButton),0);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(ySpinButton), TRUE);
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON(ySpinButton),TRUE);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(ySpinButton),(gdouble)cfg_wHeight());
	gtk_box_pack_start (GTK_BOX (ybox),GTK_WIDGET(ySpinButton),TRUE,TRUE,1);

	gtk_box_pack_start (GTK_BOX (ubox),GTK_WIDGET (gtk_label_new(_UNDO_LEVEL)), TRUE, TRUE, 1);
	uSpinButton=(GtkSpinButton*)gtk_spin_button_new_with_range(0,UNDOMAX,5);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON(uSpinButton),0);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(uSpinButton), TRUE);
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON(uSpinButton),TRUE);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(uSpinButton),(gdouble)cfg_undo());
	gtk_box_pack_start (GTK_BOX (ubox),GTK_WIDGET(uSpinButton),TRUE,TRUE,1);

	recentFilesCheck=gtk_check_button_new_with_label(_REMEMBER_RECENT_FILES);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(recentFilesCheck),
	                             cfg_recent_files_enabled());
	gtk_box_pack_start(GTK_BOX(recentCheckBox),recentFilesCheck,
	                   TRUE,TRUE,1);

	gtk_box_pack_start(GTK_BOX(recentLimitBox),
	                   GTK_WIDGET(gtk_label_new(_RECENT_FILES_LIMIT)),
	                   TRUE,TRUE,1);
	recentSpinButton=(GtkSpinButton*)gtk_spin_button_new_with_range(
		0,TPAD_RECENT_FILES_MAXIMUM,10);
	gtk_spin_button_set_digits(recentSpinButton,0);
	gtk_spin_button_set_numeric(recentSpinButton,TRUE);
	gtk_spin_button_set_value(recentSpinButton,
	                          (gdouble)cfg_recent_files_limit());
	gtk_widget_set_sensitive(GTK_WIDGET(recentSpinButton),
	                         cfg_recent_files_enabled());
	gtk_box_pack_start(GTK_BOX(recentLimitBox),GTK_WIDGET(recentSpinButton),
	                   TRUE,TRUE,1);

	GtkWidget *pToolbar = (GtkWidget *) gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(pToolbar),GTK_TOOLBAR_ICONS);
	GtkToolItem* toolApply = (GtkToolItem*) gtk_tool_button_new(NULL,"Apply");
	gtk_toolbar_insert(GTK_TOOLBAR(pToolbar),GTK_TOOL_ITEM(toolApply),0);


	gtk_box_pack_start (GTK_BOX (buttonbox),GTK_WIDGET (pToolbar), TRUE, TRUE, 1);

	g_signal_connect(toolApply,"clicked",G_CALLBACK(do_UI_pref_update),(gpointer) dialog);
	g_signal_connect(recentFilesCheck,"toggled",
	                 G_CALLBACK(recent_files_toggled),NULL);
	g_signal_connect(recentSpinButton,"value-changed",
	                 G_CALLBACK(recent_limit_changed),NULL);

	gtk_widget_show_all(dialog);


}
