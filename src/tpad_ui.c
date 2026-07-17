/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013, 2014, 2015, 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_ui.c , is part of tpad.
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
extern GtkSourceBuffer *mBuff;
extern gboolean save_locked;
extern GtkWidget *window;
extern gchar *content;
extern gchar* origfile;
extern GtkSourceView *view;
extern int setModified;
extern int madetemp;
extern gboolean disable_file_watch;
extern int tpad_wach_thread_set;
extern int spell_state;
extern int run_count_update;
extern GtkSpellChecker* doc_spelling;
extern GdkDisplay* Display;
extern GtkWidget *spelling;
extern GtkWidget *Notebook;
GtkWidget *spelling;
GdkDisplay* Display;

static gboolean on_window_delete(GtkWidget *widget, GdkEvent *event,
                                 gpointer user_data)
{
	(void) widget;
	(void) event;
	(void) user_data;
	quit_program();
	/* A successful quit exits.  Returning TRUE therefore means the user
	 * canceled and GTK must keep the window alive. */
	return TRUE;
}

static void on_quit_activate(GtkWidget *widget, gpointer user_data)
{
	(void) widget;
	(void) user_data;
	quit_program();
}

static void on_save_activate(GtkWidget *widget, gpointer user_data)
{
	(void) widget;
	(void) user_data;
	(void) save_file();
}

static void on_save_as_activate(GtkWidget *widget, gpointer user_data)
{
	(void) widget;
	(void) user_data;
	(void) save_as();
}

static void on_spelling_activate(GtkWidget *widget, gpointer user_data)
{
	(void) widget;
	(void) user_data;
	(void) toggle_spelling();
}

static void on_reload_activate(GtkWidget *widget, gpointer user_data)
{
	(void) widget;
	(void) user_data;
	tpad_reload();
}

#define DEFINE_VOID_ACTION_CALLBACK(callback, action) \
	static void callback(GtkWidget *widget, gpointer user_data) \
	{ \
		(void) widget; \
		(void) user_data; \
		action(); \
	}

#define DEFINE_WIDGET_ACTION_CALLBACK(callback, action) \
	static void callback(GtkWidget *widget, gpointer user_data) \
	{ \
		(void) user_data; \
		action(widget); \
	}

DEFINE_VOID_ACTION_CALLBACK(on_new_activate, runn)
DEFINE_VOID_ACTION_CALLBACK(on_open_activate, open_file)
DEFINE_VOID_ACTION_CALLBACK(on_print_activate, tpad_print_document)
DEFINE_VOID_ACTION_CALLBACK(on_find_activate, replace_dialog)
DEFINE_VOID_ACTION_CALLBACK(on_undo_activate, do_undo)
DEFINE_VOID_ACTION_CALLBACK(on_redo_activate, do_redo)
DEFINE_VOID_ACTION_CALLBACK(on_uppercase_activate, up_caps_document)
DEFINE_VOID_ACTION_CALLBACK(on_lowercase_activate, low_caps_document)
DEFINE_VOID_ACTION_CALLBACK(on_reverse_activate, rev_document)
DEFINE_VOID_ACTION_CALLBACK(on_frequency_activate, tpad_frequency_analysis)
DEFINE_VOID_ACTION_CALLBACK(on_font_activate, select_font)
DEFINE_VOID_ACTION_CALLBACK(on_about_activate, show_about)
DEFINE_VOID_ACTION_CALLBACK(on_preferences_activate, show_ui_prefs)

DEFINE_WIDGET_ACTION_CALLBACK(on_line_wrap_activate, toggle_line_wrap)
DEFINE_WIDGET_ACTION_CALLBACK(on_copy_filename_activate,
                              tpad_copy_file_name_to_clipboard)
DEFINE_WIDGET_ACTION_CALLBACK(on_line_number_activate, toggle_linenumber)
DEFINE_WIDGET_ACTION_CALLBACK(on_full_path_activate, toggle_show_full_path)
DEFINE_WIDGET_ACTION_CALLBACK(on_language_activate, toggle_show_lang)
DEFINE_WIDGET_ACTION_CALLBACK(on_keep_above_activate, toggle_keep_above)
DEFINE_WIDGET_ACTION_CALLBACK(on_text_stats_activate, get_text_stats)
DEFINE_WIDGET_ACTION_CALLBACK(on_open_guard_activate, toggle_use_open_guard)
DEFINE_WIDGET_ACTION_CALLBACK(on_base64_activate, base64_connector)
DEFINE_WIDGET_ACTION_CALLBACK(on_base64_decode_activate, fbase64_connector)
DEFINE_WIDGET_ACTION_CALLBACK(on_file_base64_activate, base64_file_connector)
DEFINE_WIDGET_ACTION_CALLBACK(on_file_base64_decode_activate,
                              fbase64_file_connector)
DEFINE_WIDGET_ACTION_CALLBACK(on_sha512_activate, h512_connector)
DEFINE_WIDGET_ACTION_CALLBACK(on_sha256_activate, h256_connector)
DEFINE_WIDGET_ACTION_CALLBACK(on_md5_activate, hmd5_connector)
DEFINE_WIDGET_ACTION_CALLBACK(on_file_sha512_activate, h512_file_connector)
DEFINE_WIDGET_ACTION_CALLBACK(on_file_sha256_activate, h256_file_connector)
DEFINE_WIDGET_ACTION_CALLBACK(on_file_md5_activate, hmd5_file_connector)

#ifdef AUTO_TAB_TOGGLE
DEFINE_WIDGET_ACTION_CALLBACK(on_auto_tab_activate, toggle_auto_tab)
#endif
GtkWidget *tpad_new_ui(void){
//tpad_fp_init();
	////////////////////////////////////////////////////////////////////////
	/* Declare some var for use in the main window for menus, tool bar, etc. */
	////////////////////////////////////////////////////////////////////////
	GtkAccelGroup *accel_group;
	GtkWidget *menubar,*file,*hashmenu,*hashmd5menu,*hhash, *hash256menu,*hash512menu,*ToBase64menu,*FromBase64menu,*hashfile512menu,*hashfile256menu,*fileToBase64menu,*fileFromBase64menu,*hashfilemd5menu,*filemenu,*newfile,*open,*copyFileName, *save,*saveas,*print_item,*freqana,*quit,*checkOnOpen,
	*search,*searchmenu,*find,*replace,*redo,*reload,*undo,*reverse,*capsup,*capsdown,*settings,
	*settingsmenu,*selectfont,*linenumber,*help,*helpmenu,*about,*toolbar,*DocStats,
	*scrolled_window,*mainbox,*lang_chk,*ShowFullPath,*lineWp,*keepAbove,*UIPrefs;
	#ifdef AUTO_TAB_TOGGLE
	GtkWidget *AutoTab;
	#endif

	GtkToolItem *toolnew,*toolopen,*toolsave,*toolreload,*toolsaveas,*toolredo,*toolundo,*toolquit;

	////////////////////////////////////////////////////////////////////////
	// Get information on the display for use with config.
	// Be defensive: the primary monitor or workarea may be unavailable
	// at startup in some environments; use fallbacks to avoid spurious
	// out-of-range errors.
	////////////////////////////////////////////////////////////////////////
	Display = gdk_display_get_default();
	gint sw = 0, sh = 0;

	#if GTK_CHECK_VERSION(3,22,0)
	// Prefer the primary monitor workarea when available (GTK 3.x)
	GdkMonitor *monitor = Display != NULL
	                      ? gdk_display_get_primary_monitor(Display) : NULL;
	if (monitor == NULL && Display != NULL &&
	    gdk_display_get_n_monitors(Display) > 0)
		monitor = gdk_display_get_monitor(Display, 0);
	if (monitor) {
		GdkRectangle workarea = {0, 0, 0, 0};
		gdk_monitor_get_workarea(monitor, &workarea);
		sw = workarea.width;
		sh = workarea.height;
	}
	#else
	{
		GdkScreen *screen = gdk_display_get_default_screen(Display);
		if (screen) {
			sw = gdk_screen_get_width(screen);
			sh = gdk_screen_get_height(screen);
		}
	}
	#endif

	if (sw <= 0) sw = _DEFAULT_WINDOW_WIDTH;
	if (sh <= 0) sh = _DEFAULT_WINDOW_HEIGHT;

	cfg_set_screen_width(sw);
	cfg_set_screen_height(sh);
	////////////////////////////////////////////////////////////////////////
	// Main Window Setup
	////////////////////////////////////////////////////////////////////////
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"Tpad");
	gtk_window_set_default_size(GTK_WINDOW(window),cfg_wWidth(),cfg_wHeight());
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	GdkPixbuf *window_icon = gdk_pixbuf_new_from_data(
	        tpad_inline + 24, GDK_COLORSPACE_RGB, TRUE, 8,
	        64, 64, 256, NULL, NULL);
	gtk_window_set_icon(GTK_WINDOW(window), window_icon);
	g_object_unref(window_icon);
	mainbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
 	////////////////////////////////////////////////////////////////////////
	// Menu bar setup
	////////////////////////////////////////////////////////////////////////
	menubar = gtk_menu_bar_new();
 	////////////////////////////////////////////////////////////////////////
	//Setup file menu
 	////////////////////////////////////////////////////////////////////////
    	filemenu = gtk_menu_new();

		file = gtk_menu_item_new_with_label(_FILE_MENU);
		newfile = gtk_menu_item_new_with_mnemonic(_NEW_FILE_MNEMONIC);
		open = gtk_menu_item_new_with_mnemonic(_OPEN_FILE_MNEMONIC);
		save = gtk_menu_item_new_with_mnemonic(_SAVE_FILE_MNEMONIC);
		saveas = gtk_menu_item_new_with_mnemonic(_SAVE_FILE_AS_MNEMONIC);
		print_item = gtk_menu_item_new_with_mnemonic(_PRINT_MNEMONIC);
		quit = gtk_menu_item_new_with_mnemonic(_QUIT_MNEMONIC);
		reload = gtk_menu_item_new_with_mnemonic(_RELOAD_MNEMONIC);
   		gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);

    	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), newfile);
    	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), saveas);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), print_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), gtk_separator_menu_item_new() );
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), reload);
    	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), gtk_separator_menu_item_new() );
    	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
	////////////////////////////////////////////////////////////////////////
	// Setup Hash Menu
	////////////////////////////////////////////////////////////////////////
	hashmenu = gtk_menu_new();
		hhash  = gtk_menu_item_new_with_label(_HASH_MENU);
		hash512menu = gtk_menu_item_new_with_label(_HASH_MENU_CB_TO_512);
		hash256menu = gtk_menu_item_new_with_label(_HASH_MENU_CB_TO_256);
		hashmd5menu = gtk_menu_item_new_with_label(_HASH_MENU_CB_TO_MD5);
		ToBase64menu = gtk_menu_item_new_with_label(_MENU_CB_TO_BASE_64);
		FromBase64menu = gtk_menu_item_new_with_label(_MENU_CB_FROM_BASE_64);
		hashfile512menu = gtk_menu_item_new_with_label(_HASH_MENU_F_TO_512);
		hashfile256menu = gtk_menu_item_new_with_label(_HASH_MENU_F_TO_256);
		hashfilemd5menu = gtk_menu_item_new_with_label(_HASH_MENU_F_TO_MD5);
		fileToBase64menu = gtk_menu_item_new_with_label(_MENU_F_TO_BASE_64);
		fileFromBase64menu = gtk_menu_item_new_with_label(_MENU_F_FROM_BASE_64);

		gtk_menu_item_set_submenu(GTK_MENU_ITEM(hhash), hashmenu);
		gtk_menu_shell_append(GTK_MENU_SHELL(hashmenu), hash512menu);
		gtk_menu_shell_append(GTK_MENU_SHELL(hashmenu), hash256menu);
		gtk_menu_shell_append(GTK_MENU_SHELL(hashmenu), hashmd5menu);
		gtk_menu_shell_append(GTK_MENU_SHELL(hashmenu), ToBase64menu);
		gtk_menu_shell_append(GTK_MENU_SHELL(hashmenu), FromBase64menu);
		gtk_menu_shell_append(GTK_MENU_SHELL(hashmenu), gtk_separator_menu_item_new() );
		gtk_menu_shell_append(GTK_MENU_SHELL(hashmenu), hashfile512menu);
		gtk_menu_shell_append(GTK_MENU_SHELL(hashmenu), hashfile256menu);
		gtk_menu_shell_append(GTK_MENU_SHELL(hashmenu), hashfilemd5menu);
		gtk_menu_shell_append(GTK_MENU_SHELL(hashmenu), fileToBase64menu);
		gtk_menu_shell_append(GTK_MENU_SHELL(hashmenu), fileFromBase64menu);
	////////////////////////////////////////////////////////////////////////
	//Setup search menu
 	////////////////////////////////////////////////////////////////////////
		searchmenu=gtk_menu_new();

    	search = gtk_menu_item_new_with_label(_EDIT_MENU);
		find = gtk_menu_item_new_with_mnemonic(_FIND_MNEMONIC);
		replace = gtk_menu_item_new_with_mnemonic(_REPLACE_MNEMONIC);
		undo = gtk_menu_item_new_with_mnemonic(_UNDO_MNEMONIC);
		redo = gtk_menu_item_new_with_mnemonic(_REDO_MNEMONIC);
		copyFileName = gtk_menu_item_new_with_mnemonic(_CCFP_MNEMONIC);
		reverse= gtk_menu_item_new_with_label((const gchar*)_REVERSE_ALL_TEXT);
		capsup=gtk_menu_item_new_with_label((const gchar*)_MAKE_UPPERCASE);
		capsdown=gtk_menu_item_new_with_label((const gchar*)_MAKE_LOWERCASE);

		freqana=gtk_menu_item_new_with_label((const gchar*) "Frequency Analysis");

    	gtk_menu_item_set_submenu(GTK_MENU_ITEM(search), searchmenu);
    	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), find);
    	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), replace);
    	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), gtk_separator_menu_item_new() );
    	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), undo);
    	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), redo);
    	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), gtk_separator_menu_item_new() );
	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), copyFileName);
	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), gtk_separator_menu_item_new() );
	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), reverse);
	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), capsup);
	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), capsdown);
	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), freqana);
	////////////////////////////////////////////////////////////////////////
	//Setup settings menu
	////////////////////////////////////////////////////////////////////////
	settingsmenu=gtk_menu_new();

	settings = gtk_menu_item_new_with_label((const gchar*) _SETTINGS_MENU);
		selectfont = gtk_menu_item_new_with_mnemonic(_SELECT_FONT_MNEMONIC);
    	linenumber = gtk_check_menu_item_new_with_label (_LINE_NUMBER);
	lang_chk = gtk_check_menu_item_new_with_label (_LANGUAGE);
	#ifdef AUTO_TAB_TOGGLE
	AutoTab = gtk_check_menu_item_new_with_label (_AUTOTAB);
	#endif
	spelling= gtk_check_menu_item_new_with_label(_SPELLING);
	ShowFullPath = gtk_check_menu_item_new_with_label(_SHOW_FULL_PATH);
	UIPrefs = gtk_menu_item_new_with_mnemonic(_PREF_MNEMONIC);
	DocStats =gtk_menu_item_new_with_label((const gchar*)_WORD_COUNT);
	keepAbove = gtk_check_menu_item_new_with_label(_KEEP_WIND_ABOVE);
	lineWp = gtk_check_menu_item_new_with_label(_LINE_WP);
	checkOnOpen=gtk_check_menu_item_new_with_label(_CHECK_ON_OPEN);
	// Set items active state in settings menu to match config state
	////////////////////////////////////////////////////////////////////////
	(cfg_line_wrap()) ? gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(lineWp),TRUE) : gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(lineWp),FALSE);
	(cfg_show_full_path()) ? gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ShowFullPath),TRUE) : gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ShowFullPath),FALSE);
	(cfg_line()) ? gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(linenumber),TRUE) : gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(linenumber),FALSE);
	(cfg_spell()) ? gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(spelling),TRUE) : gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(spelling),FALSE);
	(cfg_lang()) ? gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(lang_chk),TRUE) : gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(lang_chk),FALSE);
	(cfg_use_open_guard()) ? gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(checkOnOpen),TRUE) : gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(checkOnOpen),FALSE);
	#ifdef AUTO_TAB_TOGGLE
	(cfg_auto_tab()) ? gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(AutoTab),TRUE) : gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(AutoTab),FALSE);
	#endif
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(settings), settingsmenu);
	#ifdef AUTO_TAB_TOGGLE
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), AutoTab);
	#endif
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), lineWp);
    	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), linenumber);
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), spelling);
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), lang_chk);
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), ShowFullPath);
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), keepAbove);
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), gtk_separator_menu_item_new() );
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), selectfont);
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), gtk_separator_menu_item_new() );
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), UIPrefs);
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), DocStats);
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), checkOnOpen);
	////////////////////////////////////////////////////////////////////////
	//Set up help menu
	////////////////////////////////////////////////////////////////////////
	helpmenu=gtk_menu_new();

    	help =  gtk_menu_item_new_with_label((const gchar*)_HELP_MENU);
	about =  gtk_menu_item_new_with_label((const gchar*) _ABOUT_MENU);
   	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), helpmenu);
   	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), about);

	////////////////////////////////////////////////////////////////////////
	//Setup some accels
 	////////////////////////////////////////////////////////////////////////
	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

	gtk_widget_add_accelerator(newfile, "activate", accel_group,
                               GDK_KEY_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    	gtk_widget_add_accelerator(open, "activate", accel_group,
                               GDK_KEY_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(save, "activate", accel_group,
	                           GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(print_item, "activate", accel_group,
	                           GDK_KEY_p, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(quit, "activate", accel_group,
                               GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    	gtk_widget_add_accelerator(find, "activate", accel_group,
                               GDK_KEY_f, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    	gtk_widget_add_accelerator(replace, "activate", accel_group,
                               GDK_KEY_h, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    	gtk_widget_add_accelerator(undo, "activate", accel_group,
                               GDK_KEY_z, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    	gtk_widget_add_accelerator(reverse, "activate", accel_group,
                               GDK_KEY_r, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    	gtk_widget_add_accelerator(capsup, "activate", accel_group,
                               GDK_KEY_u, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	    	gtk_widget_add_accelerator(capsdown, "activate", accel_group,
                               GDK_KEY_l, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(redo, "activate", accel_group,
	                           GDK_KEY_z,
	                           GDK_CONTROL_MASK | GDK_SHIFT_MASK,
	                           GTK_ACCEL_VISIBLE);
	#ifdef AUTO_TAB_TOGGLE
	gtk_widget_add_accelerator(AutoTab, "activate", accel_group,
                               GDK_KEY_F7, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	#endif
	gtk_widget_add_accelerator(lineWp, "activate", accel_group,
                               GDK_KEY_F6, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(linenumber, "activate", accel_group,
                               GDK_KEY_F5, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(lang_chk, "activate", accel_group,
                               GDK_KEY_F4, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(spelling, "activate", accel_group,
                               GDK_KEY_F3, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(ShowFullPath, "activate", accel_group,
                               GDK_KEY_F2, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(keepAbove, "activate", accel_group,
                               GDK_KEY_F1, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(UIPrefs, "activate", accel_group,
	                           GDK_KEY_comma, GDK_CONTROL_MASK,
	                           GTK_ACCEL_VISIBLE);
	////////////////////////////////////////////////////////////////////////
	// Put all of these menus into the main menu menu bar
	////////////////////////////////////////////////////////////////////////
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);
    	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), search);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), hhash);
    	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), settings);
    	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help);

	gtk_box_pack_start(GTK_BOX(mainbox), menubar, FALSE, FALSE, 0);
	////////////////////////////////////////////////////////////////////////
	// Set up toolbar
	////////////////////////////////////////////////////////////////////////
		toolbar = gtk_toolbar_new();
    	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar),GTK_TOOLBAR_ICONS);

		toolquit = gtk_tool_button_new(NULL,_QUIT_TOOLBAR);
    	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolquit),0);

		gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(gtk_separator_tool_item_new ()),0);
		toolredo = gtk_tool_button_new(NULL,_REDO_TOOLBAR);

    	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolredo),0);

		toolundo = gtk_tool_button_new(NULL,_UNDO_TOOLBAR);
    	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolundo),0);

	    	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(gtk_separator_tool_item_new ()),0);

	toolreload = gtk_tool_button_new(NULL,_TOOLRELOAD_FILE);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolreload),0);

    	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(gtk_separator_tool_item_new ()),0);

		toolsaveas = gtk_tool_button_new(NULL,_SAVE_AS_TOOLBAR);
    	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolsaveas),0);

		toolsave = gtk_tool_button_new(NULL,_SAVE_TOOLBAR);
    	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolsave),0);

		toolopen =  gtk_tool_button_new(NULL,_OPEN_TOOLBAR);
    	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolopen),0);

		toolnew = gtk_tool_button_new(NULL,_NEW_TOOLBAR);
    	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolnew),0);
	////////////////////////////////////////////////////////////////////////
	// Add the tool bar to the box main box
	////////////////////////////////////////////////////////////////////////
    	gtk_box_pack_start(GTK_BOX(mainbox), toolbar, FALSE, FALSE, 0);
	////////////////////////////////////////////////////////////////////////
	// Set up GTKSourceView with a buffer
	////////////////////////////////////////////////////////////////////////
	mBuff=gtk_source_buffer_new(NULL);

	view=(GtkSourceView *)gtk_source_view_new_with_buffer(mBuff);
    	(cfg_line()) ? gtk_source_view_set_show_line_numbers (view,TRUE) : gtk_source_view_set_show_line_numbers (view,FALSE) ;
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),
	                            cfg_line_wrap() ? GTK_WRAP_WORD : GTK_WRAP_NONE);
	tpad_set_font("DejaVu Sans Mono 12");
	gtk_source_view_set_smart_home_end(GTK_SOURCE_VIEW(view),GTK_SOURCE_SMART_HOME_END_BEFORE);


	////////////////////////////////////////////////////////////////////////
	// Put our source view into a scrolled window and added it to box mainbox
	////////////////////////////////////////////////////////////////////////
	scrolled_window=gtk_scrolled_window_new(NULL,NULL);
    	gtk_container_add(GTK_CONTAINER(scrolled_window),GTK_WIDGET(view));
    	gtk_box_pack_start(GTK_BOX(mainbox),GTK_WIDGET(scrolled_window),TRUE,TRUE,0);
	////////////////////////////////////////////////////////////////////////
	// Add box mainbox to main window, window
	gtk_container_add(GTK_CONTAINER(window),mainbox);
	////////////////////////////////////////////////////////////////////////
	// Connect signals from above to callbacks
	g_signal_connect(window,"delete-event",G_CALLBACK(on_window_delete),NULL);
	g_signal_connect(newfile,"activate",G_CALLBACK(on_new_activate),NULL);
	g_signal_connect(open,"activate",G_CALLBACK(on_open_activate),NULL);
	g_signal_connect(save,"activate",G_CALLBACK(on_save_activate),NULL);
	g_signal_connect(saveas,"activate",G_CALLBACK(on_save_as_activate),NULL);
	g_signal_connect(print_item,"activate",G_CALLBACK(on_print_activate),NULL);
	g_signal_connect(reload,"activate",G_CALLBACK(on_reload_activate),NULL);
	g_signal_connect(quit,"activate",G_CALLBACK(on_quit_activate),NULL);


	g_signal_connect(find,"activate",G_CALLBACK(on_find_activate),NULL);
	g_signal_connect(replace,"activate",G_CALLBACK(on_find_activate),NULL);
	g_signal_connect(lineWp,"activate",G_CALLBACK(on_line_wrap_activate),NULL);

	g_signal_connect(copyFileName,"activate",G_CALLBACK(on_copy_filename_activate),NULL);

	g_signal_connect(undo,"activate",G_CALLBACK(on_undo_activate),NULL);
	g_signal_connect(redo,"activate",G_CALLBACK(on_redo_activate),NULL);
	g_signal_connect(capsup,"activate",G_CALLBACK(on_uppercase_activate),NULL);

	g_signal_connect(freqana,"activate",G_CALLBACK(on_frequency_activate),NULL);

	g_signal_connect(capsdown,"activate",G_CALLBACK(on_lowercase_activate),NULL);
	g_signal_connect(reverse,"activate",G_CALLBACK(on_reverse_activate),NULL);
	g_signal_connect(selectfont,"activate",G_CALLBACK(on_font_activate),NULL);
	g_signal_connect(linenumber,"activate",G_CALLBACK(on_line_number_activate),NULL);
	g_signal_connect(spelling,"activate",G_CALLBACK(on_spelling_activate),NULL);
	g_signal_connect(ShowFullPath,"activate",G_CALLBACK(on_full_path_activate),NULL);
	#ifdef AUTO_TAB_TOGGLE
	g_signal_connect(AutoTab,"activate",G_CALLBACK(on_auto_tab_activate),NULL);
	#endif
	g_signal_connect(lang_chk,"activate",G_CALLBACK(on_language_activate),NULL);
	g_signal_connect(keepAbove,"activate",G_CALLBACK(on_keep_above_activate),NULL);
	g_signal_connect(about,"activate",G_CALLBACK(on_about_activate),NULL);
	g_signal_connect(UIPrefs,"activate",G_CALLBACK(on_preferences_activate),NULL);
	g_signal_connect(DocStats,"activate",G_CALLBACK(on_text_stats_activate),NULL);
	g_signal_connect(checkOnOpen,"activate",G_CALLBACK(on_open_guard_activate),NULL);


	g_signal_connect(ToBase64menu,"activate",G_CALLBACK(on_base64_activate),NULL);
	g_signal_connect(FromBase64menu,"activate",G_CALLBACK(on_base64_decode_activate),NULL);
	g_signal_connect(fileToBase64menu,"activate",G_CALLBACK(on_file_base64_activate),NULL);
	g_signal_connect(fileFromBase64menu,"activate",G_CALLBACK(on_file_base64_decode_activate),NULL);


	g_signal_connect(hash512menu,"activate",G_CALLBACK(on_sha512_activate),NULL);
		g_signal_connect(hash256menu,"activate",G_CALLBACK(on_sha256_activate),NULL);
		g_signal_connect(hashmd5menu,"activate",G_CALLBACK(on_md5_activate),NULL);

		g_signal_connect(hashfile512menu,"activate",G_CALLBACK(on_file_sha512_activate),NULL);
		g_signal_connect(hashfile256menu,"activate",G_CALLBACK(on_file_sha256_activate),NULL);
		g_signal_connect(hashfilemd5menu,"activate",G_CALLBACK(on_file_md5_activate),NULL);

	g_signal_connect(toolnew,"clicked",G_CALLBACK(on_new_activate),NULL);
	g_signal_connect(toolopen,"clicked",G_CALLBACK(on_open_activate),NULL);
	g_signal_connect(toolsave,"clicked",G_CALLBACK(on_save_activate),NULL);
	g_signal_connect(toolsaveas,"clicked",G_CALLBACK(on_save_as_activate),NULL);
	g_signal_connect(toolundo,"clicked",G_CALLBACK(on_undo_activate),NULL);
	g_signal_connect(toolredo,"clicked",G_CALLBACK(on_redo_activate),NULL);
	g_signal_connect(toolreload,"clicked",G_CALLBACK(on_reload_activate),NULL);

	g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK (on_key_press), NULL);



	//g_signal_connect(toolspelling,"clicked",G_CALLBACK(toggle_spelling),NULL);
	g_signal_connect(toolquit,"clicked",G_CALLBACK(on_quit_activate),NULL);
	return(window);
}



gboolean on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	(void) widget;
	(void) user_data;

	GtkTextIter start,end;

	switch (event->keyval)
	{
	case GDK_KEY_End:

			gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(mBuff),&end);
		gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(mBuff),&end);

	break;

	case GDK_KEY_Home:
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(mBuff),&start);
		gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(mBuff),&start);

	break;

   	 default:
      		return FALSE;
  	}

  return FALSE;
}
