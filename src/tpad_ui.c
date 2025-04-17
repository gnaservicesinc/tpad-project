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
extern gchar path_temp_file[];
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




GtkWidget *tpad_new_ui(){
//tpad_fp_init();
	////////////////////////////////////////////////////////////////////////
	/* Declare some var for use in the main window for menus, tool bar, etc. */
	////////////////////////////////////////////////////////////////////////
	GtkAccelGroup *accel_group;
    	GtkWidget *menubar,*file,*hashmenu,*hashmd5menu,*hhash, *hash256menu,*hash512menu,*ToBase64menu,*FromBase64menu,*hashfile512menu,*hashfile256menu,*fileToBase64menu,*fileFromBase64menu,*hashfilemd5menu,*filemenu,*newfile,*open,*copyFileName, *save,*saveas,*freqana,*quit,*checkOnOpen,
	*search,*searchmenu,*find,*replace,*redo,*reload,*undo,*reverse,*capsup,*capsdown,*settings,
	*settingsmenu,*selectfont,*linenumber,*help,*helpmenu,*about,*toolbar,*DocStats,
	*scrolled_window,*mainbox,*lang_chk,*ShowFullPath,*lineWp,*keepAbove,*UIPrefs;
	#ifdef AUTO_TAB_TOGGLE
	GtkWidget *AutoTab;
	#endif
	#ifdef HAVE_LIBMAGIC
	GtkWidget *UseMagic;
	#endif
		
	GtkToolItem *toolnew,*toolopen,*toolsave,*toolreload,*toolsaveas,*toolredo,*toolundo,*toolquit;
	gint idisplayNumber=0,mwidth=0,mheight=0;

	 GtkWidget *Notebook = gtk_notebook_new ();

	////////////////////////////////////////////////////////////////////////
	// Get information on the display for use with config. 
	////////////////////////////////////////////////////////////////////////
	Display=gdk_display_manager_get_default_display (gdk_display_manager_get());
	GdkScreen* Screen= (GdkScreen*) gdk_display_get_screen((GdkDisplay *)Display, (gint) 0);
	cfg_set_screen_width(gdk_screen_get_width(Screen));
	cfg_set_screen_height(gdk_screen_get_height(Screen)); 
	////////////////////////////////////////////////////////////////////////
	// Main Window Setup
	////////////////////////////////////////////////////////////////////////
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"Tpad");
	gtk_window_set_default_size(GTK_WINDOW(window),cfg_wWidth(),cfg_wHeight());
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_window_set_icon(GTK_WINDOW(window),gdk_pixbuf_new_from_inline (-1, tpad_inline, FALSE, NULL));
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
		quit = gtk_menu_item_new_with_mnemonic(_QUIT_MNEMONIC);
		reload = gtk_menu_item_new_with_mnemonic(_RELOAD_MNEMONIC);
   		gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
	
    	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), newfile);
    	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open);
    	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save);
    	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), saveas);
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
	#ifdef HAVE_LIBMAGIC
	UseMagic=gtk_check_menu_item_new_with_label(_SETTINGS_USE_MAGIC);
	#endif
	// Set items active state in settings menu to match config state
	////////////////////////////////////////////////////////////////////////
	(cfg_line_wrap()) ? gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(lineWp),TRUE) : gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(lineWp),FALSE);
	(cfg_show_full_path()) ? gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ShowFullPath),TRUE) : gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ShowFullPath),FALSE);
	(cfg_line()) ? gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(linenumber),TRUE) : gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(linenumber),FALSE);
	(cfg_spell()) ? gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(spelling),TRUE) : gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(spelling),FALSE);
	(cfg_lang()) ? gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(lang_chk),TRUE) : gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(lang_chk),FALSE);       
	(cfg_use_open_guard()) ? gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(checkOnOpen),TRUE) : gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(checkOnOpen),FALSE);   
	#ifdef HAVE_LIBMAGIC
	(cfg_use_magic()) ? gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(UseMagic),TRUE) : gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(UseMagic),FALSE);     
 	#endif  
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
	#ifdef HAVE_LIBMAGIC
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), UseMagic);
	#endif
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
	                           GDK_KEY_r, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
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
	tpad_set_font("DejaVu Sans Mono 12");
	gtk_source_view_set_smart_home_end(GTK_SOURCE_VIEW(view),GTK_SOURCE_SMART_HOME_END_BEFORE);

	
		g_signal_connect(mBuff,"insert_text",G_CALLBACK(on_past),NULL);
	g_signal_connect(window,"delete_event",G_CALLBACK(quit_program),NULL);
	
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
	g_signal_connect(window,"delete_event",G_CALLBACK(quit_program),NULL);
    	g_signal_connect(newfile,"activate",G_CALLBACK(runn),NULL);
    	g_signal_connect(open,"activate",G_CALLBACK(open_file),NULL);
    	g_signal_connect(save,"activate",G_CALLBACK(save_file),NULL);
    	g_signal_connect(saveas,"activate",G_CALLBACK(save_as),NULL);
	g_signal_connect(reload,"activate",G_CALLBACK(tpad_reload),NULL);
    	g_signal_connect(quit,"activate",G_CALLBACK(quit_program),NULL);


	g_signal_connect(find,"activate",G_CALLBACK(replace_dialog),NULL);
    	g_signal_connect(replace,"activate",G_CALLBACK(replace_dialog),NULL);
	g_signal_connect(lineWp,"activate",G_CALLBACK(toggle_line_wrap),NULL);

	g_signal_connect(copyFileName,"activate",G_CALLBACK(tpad_copy_file_name_to_clipboard),NULL);

    	g_signal_connect(undo,"activate",G_CALLBACK(do_undo),NULL);
	g_signal_connect(redo,"activate",G_CALLBACK(do_redo),NULL);
	g_signal_connect(capsup,"activate",G_CALLBACK(up_caps_document),NULL);

	g_signal_connect(freqana,"activate",G_CALLBACK(tpad_frequency_analysis),NULL);

	g_signal_connect(capsdown,"activate",G_CALLBACK(low_caps_document),NULL);
    	g_signal_connect(reverse,"activate",G_CALLBACK(rev_document),NULL);
	g_signal_connect(lineWp,"activate",G_CALLBACK(toggle_line_wrap),NULL);
	g_signal_connect(selectfont,"activate",G_CALLBACK(select_font),NULL);
    	g_signal_connect(linenumber,"activate",G_CALLBACK(toggle_linenumber),NULL);
	g_signal_connect(spelling,"activate",G_CALLBACK(toggle_spelling),NULL);
	g_signal_connect(ShowFullPath,"activate",G_CALLBACK(toggle_show_full_path),NULL);	
	#ifdef AUTO_TAB_TOGGLE
	g_signal_connect(AutoTab,"activate",G_CALLBACK(toggle_auto_tab),NULL);
	#endif
	g_signal_connect(lang_chk,"activate",G_CALLBACK(toggle_show_lang),NULL);
	g_signal_connect(keepAbove,"activate",G_CALLBACK(toggle_keep_above),NULL);
	g_signal_connect(about,"activate",G_CALLBACK(show_about),NULL);
	g_signal_connect(UIPrefs,"activate",G_CALLBACK(show_ui_prefs),NULL);
	g_signal_connect(DocStats,"activate",G_CALLBACK(get_text_stats),NULL);
	g_signal_connect(checkOnOpen,"activate",G_CALLBACK(toggle_use_open_guard),NULL);
	#ifdef HAVE_LIBMAGIC
	g_signal_connect(UseMagic,"activate",G_CALLBACK(toggle_use_magic_settings),NULL);
	#endif


	g_signal_connect(ToBase64menu,"activate",G_CALLBACK(base64_connector),NULL);
	g_signal_connect(FromBase64menu,"activate",G_CALLBACK(fbase64_connector),NULL);
	g_signal_connect(fileToBase64menu,"activate",G_CALLBACK(base64_file_connector),NULL);
	g_signal_connect(fileFromBase64menu,"activate",G_CALLBACK(fbase64_file_connector),NULL);


	g_signal_connect(hash512menu,"activate",G_CALLBACK(h512_connector),NULL);
		g_signal_connect(hash256menu,"activate",G_CALLBACK(h256_connector),NULL);
		g_signal_connect(hashmd5menu,"activate",G_CALLBACK(hmd5_connector),NULL);

		g_signal_connect(hashfile512menu,"activate",G_CALLBACK(h512_file_connector),NULL);
		g_signal_connect(hashfile256menu,"activate",G_CALLBACK(h256_file_connector),NULL);
		g_signal_connect(hashfilemd5menu,"activate",G_CALLBACK(hmd5_file_connector),NULL);

    	g_signal_connect(toolnew,"clicked",G_CALLBACK(runn),NULL);
    	g_signal_connect(toolopen,"clicked",G_CALLBACK(open_file),NULL);
    	g_signal_connect(toolsave,"clicked",G_CALLBACK(save_file),NULL);
    	g_signal_connect(toolsaveas,"clicked",G_CALLBACK(save_as),NULL);
    	g_signal_connect(toolundo,"clicked",G_CALLBACK(do_undo),NULL);
    	g_signal_connect(toolredo,"clicked",G_CALLBACK(do_redo),NULL);
	g_signal_connect(toolreload,"clicked",G_CALLBACK(tpad_reload),NULL);
	
	g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK (on_key_press), NULL);



	//g_signal_connect(toolspelling,"clicked",G_CALLBACK(toggle_spelling),NULL);
	g_signal_connect(toolquit,"clicked",G_CALLBACK(quit_program),NULL);
	return(window);
}



gboolean on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {

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
