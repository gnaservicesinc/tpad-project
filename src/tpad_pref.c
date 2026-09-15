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
static GtkCheckButton *recentFilesCheck;
static GtkWidget *preferences_window = NULL;
static gboolean recentEnabledChanged;
static gboolean recentLimitChanged;
extern GtkWidget *window;

static void recent_files_toggled(GtkCheckButton *button, gpointer user_data)
{
	(void) user_data;
	gtk_widget_set_sensitive(GTK_WIDGET(recentSpinButton),
	                         gtk_check_button_get_active(button));
	recentEnabledChanged = TRUE;
}

static void recent_limit_changed(GtkSpinButton *button, gpointer user_data)
{
	(void) button;
	(void) user_data;
	recentLimitChanged = TRUE;
}

static gboolean preferences_key_pressed(GtkEventControllerKey *controller,
	guint keyval, guint keycode, GdkModifierType state, gpointer user_data)
{
	(void) controller;
	(void) keycode;
	(void) state;
	if (keyval != GDK_KEY_Escape)
		return FALSE;
	gtk_window_destroy(GTK_WINDOW(user_data));
	return TRUE;
}

static void preferences_finalized(gpointer user_data, GObject *dialog)
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

static void append_expanding(GtkBox *box, GtkWidget *child)
{
	gtk_widget_set_hexpand(child, TRUE);
	gtk_box_append(box, child);
}


void do_UI_pref_update(GtkWidget *caller, gpointer dialog)
{
	const gboolean old_recent_enabled = cfg_recent_files_enabled();
	const int old_recent_limit = cfg_recent_files_limit();
	const gboolean new_recent_enabled = gtk_check_button_get_active(
		recentFilesCheck);
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
	set_language(); 
	cfg_save();
	gtk_check_button_set_active(recentFilesCheck, applied_recent_enabled);
	gtk_spin_button_set_value(recentSpinButton,
	                          (gdouble)applied_recent_limit);
	recentEnabledChanged = FALSE;
	recentLimitChanged = FALSE;
}
void show_ui_prefs(void){

	GtkWidget *dialog,*mainbox,*buttonbox,*xbox,*ybox,*ubox,
	          *recentCheckBox,*recentLimitBox;
	GtkWidget *apply_button;
	GtkEventController *key_controller;
	if (preferences_window != NULL) {
		gtk_window_present(GTK_WINDOW(preferences_window));
		return;
	}
	dialog=gtk_window_new();
	preferences_window = dialog;
	g_object_weak_ref(G_OBJECT(dialog), preferences_finalized, NULL);
	key_controller = gtk_event_controller_key_new();
	g_signal_connect(key_controller, "key-pressed",
	                 G_CALLBACK(preferences_key_pressed), dialog);
	gtk_widget_add_controller(dialog, key_controller);
	gtk_window_set_title(GTK_WINDOW(dialog),_UI_SETTINGS);
	gtk_window_set_resizable(GTK_WINDOW(dialog),FALSE);
	if (window != NULL && GTK_IS_WINDOW(window)) {
		gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
		gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
	}
	mainbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,6);
	xbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
	ybox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
	ubox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
	recentCheckBox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
	recentLimitBox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
	buttonbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
	gtk_widget_set_margin_start(mainbox, 12);
	gtk_widget_set_margin_end(mainbox, 12);
	gtk_widget_set_margin_top(mainbox, 12);
	gtk_widget_set_margin_bottom(mainbox, 12);

	gtk_window_set_child(GTK_WINDOW(dialog),mainbox);
	gtk_box_append(GTK_BOX(mainbox),xbox);
	gtk_box_append(GTK_BOX(mainbox),ybox);
	gtk_box_append(GTK_BOX(mainbox),ubox);
	gtk_box_append(GTK_BOX(mainbox),recentCheckBox);
	gtk_box_append(GTK_BOX(mainbox),recentLimitBox);
	gtk_box_append(GTK_BOX(mainbox),buttonbox);

	append_expanding(GTK_BOX(xbox),gtk_label_new(_DEFAULT_WIDTH));
	xSpinButton=(GtkSpinButton*)gtk_spin_button_new_with_range(
		_TPAD_CFG_WIDTH_MIN, WIDTH_MAX, 10);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON(xSpinButton),0);                     
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(xSpinButton), TRUE);
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON(xSpinButton),TRUE);  
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(xSpinButton),(gdouble)cfg_wWidth());                  
	append_expanding(GTK_BOX(xbox),GTK_WIDGET(xSpinButton));
	

	append_expanding(GTK_BOX(ybox),gtk_label_new(_DEFAULT_HEIGHT));
	ySpinButton=(GtkSpinButton*)gtk_spin_button_new_with_range(
		_TPAD_CFG_HEIGHT_MIN, HEIGHT_MAX, 10);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON(ySpinButton),0);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(ySpinButton), TRUE);     
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON(ySpinButton),TRUE);                 
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(ySpinButton),(gdouble)cfg_wHeight()); 
	append_expanding(GTK_BOX(ybox),GTK_WIDGET(ySpinButton));

	append_expanding(GTK_BOX(ubox),gtk_label_new(_UNDO_LEVEL));
	uSpinButton=(GtkSpinButton*)gtk_spin_button_new_with_range(0,UNDOMAX,5);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON(uSpinButton),0);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(uSpinButton), TRUE);  
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON(uSpinButton),TRUE);                    
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(uSpinButton),(gdouble)cfg_undo()); 
	append_expanding(GTK_BOX(ubox),GTK_WIDGET(uSpinButton));

	recentFilesCheck=GTK_CHECK_BUTTON(
		gtk_check_button_new_with_label(_REMEMBER_RECENT_FILES));
	gtk_check_button_set_active(recentFilesCheck,
	                            cfg_recent_files_enabled());
	append_expanding(GTK_BOX(recentCheckBox),GTK_WIDGET(recentFilesCheck));

	append_expanding(GTK_BOX(recentLimitBox),
	                 gtk_label_new(_RECENT_FILES_LIMIT));
	recentSpinButton=(GtkSpinButton*)gtk_spin_button_new_with_range(
		0,TPAD_RECENT_FILES_MAXIMUM,10);
	gtk_spin_button_set_digits(recentSpinButton,0);
	gtk_spin_button_set_numeric(recentSpinButton,TRUE);
	gtk_spin_button_set_value(recentSpinButton,
	                          (gdouble)cfg_recent_files_limit());
	gtk_widget_set_sensitive(GTK_WIDGET(recentSpinButton),
	                         cfg_recent_files_enabled());
	append_expanding(GTK_BOX(recentLimitBox),GTK_WIDGET(recentSpinButton));

	apply_button = gtk_button_new_with_label("Apply");
	gtk_widget_set_halign(apply_button, GTK_ALIGN_END);
	gtk_box_append(GTK_BOX(buttonbox), apply_button);
	
	g_signal_connect(apply_button,"clicked",G_CALLBACK(do_UI_pref_update),dialog);
	g_signal_connect(recentFilesCheck,"toggled",
	                 G_CALLBACK(recent_files_toggled),NULL);
	g_signal_connect(recentSpinButton,"value-changed",
	                 G_CALLBACK(recent_limit_changed),NULL);

	gtk_window_present(GTK_WINDOW(dialog));
	
	
}
