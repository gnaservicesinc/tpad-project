/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013-2026 Andrew Smith (GNA SERVICES INC) <andresmi@icloud.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_ui.c, is part of tpad.
 *
 *   tpad is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 ********************************************************************************/

#include "tpad_headers.h"

extern GtkSourceBuffer *mBuff;
extern GtkWidget *window;
extern GtkSourceView *view;

GdkDisplay *Display;
static GMenu *recent_menu_model;

typedef struct {
	const gchar *label;
	const gchar *action;
} TpadMenuItem;

typedef struct {
	const gchar *action;
	const gchar *accelerator;
	guint keyval;
	GdkModifierType modifiers;
} TpadAccelerator;

static const TpadAccelerator tpad_accelerators[] = {
	{ "win.new", "<Primary>n", GDK_KEY_n, GDK_CONTROL_MASK },
	{ "win.open", "<Primary>o", GDK_KEY_o, GDK_CONTROL_MASK },
	{ "win.save", "<Primary>s", GDK_KEY_s, GDK_CONTROL_MASK },
	{ "win.print", "<Primary>p", GDK_KEY_p, GDK_CONTROL_MASK },
	{ "win.quit", "<Primary>q", GDK_KEY_q, GDK_CONTROL_MASK },
	{ "win.find", "<Primary>f", GDK_KEY_f, GDK_CONTROL_MASK },
	{ "win.replace", "<Primary>h", GDK_KEY_h, GDK_CONTROL_MASK },
	{ "win.undo", "<Primary>z", GDK_KEY_z, GDK_CONTROL_MASK },
	{ "win.redo", "<Primary><Shift>z", GDK_KEY_z,
	  GDK_CONTROL_MASK | GDK_SHIFT_MASK },
	{ "win.reverse", "<Primary>r", GDK_KEY_r, GDK_CONTROL_MASK },
	{ "win.uppercase", "<Primary>u", GDK_KEY_u, GDK_CONTROL_MASK },
	{ "win.lowercase", "<Primary>l", GDK_KEY_l, GDK_CONTROL_MASK },
	{ "win.preferences", "<Primary>comma", GDK_KEY_comma,
	  GDK_CONTROL_MASK },
	{ "win.full-path", "<Primary>F2", GDK_KEY_F2, GDK_CONTROL_MASK },
	{ "win.spelling", "<Primary>F3", GDK_KEY_F3, GDK_CONTROL_MASK },
	{ "win.language", "<Primary>F4", GDK_KEY_F4, GDK_CONTROL_MASK },
	{ "win.line-numbers", "<Primary>F5", GDK_KEY_F5,
	  GDK_CONTROL_MASK },
	{ "win.line-wrap", "<Primary>F6", GDK_KEY_F6, GDK_CONTROL_MASK },
#ifdef AUTO_TAB_TOGGLE
	{ "win.auto-tab", "<Primary>F7", GDK_KEY_F7, GDK_CONTROL_MASK },
#endif
};

static GMenu *tpad_menu_from_items(const TpadMenuItem *items, gsize length)
{
	GMenu *menu = g_menu_new();
	GMenu *section = g_menu_new();

	for (gsize index = 0; index < length; index++) {
		if (items[index].label == NULL) {
			if (g_menu_model_get_n_items(G_MENU_MODEL(section)) > 0)
				g_menu_append_section(menu, NULL, G_MENU_MODEL(section));
			g_clear_object(&section);
			section = g_menu_new();
			continue;
		}
		g_menu_append(section, items[index].label, items[index].action);
	}
	if (g_menu_model_get_n_items(G_MENU_MODEL(section)) > 0)
		g_menu_append_section(menu, NULL, G_MENU_MODEL(section));
	g_object_unref(section);
	return menu;
}

void tpad_ui_refresh_recent_menu(void)
{
	GPtrArray *files;
	GError *error = NULL;

	if (recent_menu_model == NULL)
		return;
	g_menu_remove_all(recent_menu_model);
	if (!cfg_recent_files_enabled()) {
		g_menu_append(recent_menu_model,
		              gettext("Recent files are disabled"), NULL);
		return;
	}

	files = tpad_recent_files_load(&error);
	if (error != NULL) {
		g_warning("Unable to load recent files: %s", error->message);
		g_clear_error(&error);
	}
	if (files->len == 0)
		g_menu_append(recent_menu_model, gettext("No Recent Files"), NULL);
	for (guint index = 0; index < files->len; index++) {
		const gchar *path = g_ptr_array_index(files, index);
		gchar *label = g_strdup(path);
		GMenuItem *item;

		g_strdelimit(label, "\r\n\t", ' ');
		item = g_menu_item_new(label, NULL);
		g_menu_item_set_action_and_target_value(
			item, "win.open-recent", g_variant_new_string(path));
		g_menu_append_item(recent_menu_model, item);
		g_object_unref(item);
		g_free(label);
	}
	g_ptr_array_unref(files);
}

static gboolean on_window_close_request(GtkWindow *closed_window,
	                                     gpointer user_data)
{
	(void) closed_window;
	(void) user_data;
	quit_program();
	/* quit_program() destroys the window on success. A cancellation keeps it. */
	return TRUE;
}

static void action_new(GSimpleAction *action, GVariant *parameter,
	                   gpointer user_data)
{
	(void) action;
	(void) parameter;
	(void) user_data;
	runn();
}

static void action_open(GSimpleAction *action, GVariant *parameter,
	                    gpointer user_data)
{
	(void) action;
	(void) parameter;
	(void) user_data;
	open_file();
}

static void action_open_recent(GSimpleAction *action, GVariant *parameter,
	                           gpointer user_data)
{
	const gchar *path;
	gchar *current;
	gchar *filename;

	(void) action;
	(void) user_data;
	if (parameter == NULL)
		return;
	path = g_variant_get_string(parameter, NULL);
	filename = tpad_filename_from_utf8(path);
	if (filename == NULL || !g_file_test(filename, G_FILE_TEST_IS_REGULAR)) {
		gerror_warn(_CAN_NOT_READ_FILE, path, TRUE, FALSE);
		g_free(filename);
		return;
	}
	g_free(filename);
	if (!save_modified())
		return;
	current = tpad_fp_get_current();
	if (current != NULL)
		(void) new_thread_tpad((gchar *) path);
	else
		(void) show_file((gchar *) path);
	g_free(current);
}

static void action_save(GSimpleAction *action, GVariant *parameter,
	                    gpointer user_data)
{
	(void) action;
	(void) parameter;
	(void) user_data;
	(void) save_file();
}

static void action_save_as(GSimpleAction *action, GVariant *parameter,
	                       gpointer user_data)
{
	(void) action;
	(void) parameter;
	(void) user_data;
	(void) save_as();
}

#define TPAD_DEFINE_VOID_ACTION(name, expression) \
	static void name(GSimpleAction *action, GVariant *parameter, \
	                 gpointer user_data) \
	{ \
		(void) action; \
		(void) parameter; \
		(void) user_data; \
		expression; \
	}

TPAD_DEFINE_VOID_ACTION(action_print, tpad_print_document())
TPAD_DEFINE_VOID_ACTION(action_reload, tpad_reload())
TPAD_DEFINE_VOID_ACTION(action_quit, quit_program())
TPAD_DEFINE_VOID_ACTION(action_find, replace_dialog())
TPAD_DEFINE_VOID_ACTION(action_undo, do_undo())
TPAD_DEFINE_VOID_ACTION(action_redo, do_redo())
TPAD_DEFINE_VOID_ACTION(action_copy_filename,
	                    tpad_copy_file_name_to_clipboard(NULL))
TPAD_DEFINE_VOID_ACTION(action_uppercase, up_caps_document())
TPAD_DEFINE_VOID_ACTION(action_lowercase, low_caps_document())
TPAD_DEFINE_VOID_ACTION(action_reverse, rev_document())
TPAD_DEFINE_VOID_ACTION(action_frequency, (void) tpad_frequency_analysis())
TPAD_DEFINE_VOID_ACTION(action_font, select_font())
TPAD_DEFINE_VOID_ACTION(action_preferences, show_ui_prefs())
TPAD_DEFINE_VOID_ACTION(action_text_stats, get_text_stats(NULL))
TPAD_DEFINE_VOID_ACTION(action_about, show_about())
TPAD_DEFINE_VOID_ACTION(action_sha512, h512_connector(NULL))
TPAD_DEFINE_VOID_ACTION(action_sha256, h256_connector(NULL))
TPAD_DEFINE_VOID_ACTION(action_md5, hmd5_connector(NULL))
TPAD_DEFINE_VOID_ACTION(action_base64, base64_connector(NULL))
TPAD_DEFINE_VOID_ACTION(action_base64_decode, fbase64_connector(NULL))
TPAD_DEFINE_VOID_ACTION(action_file_sha512, h512_file_connector(NULL))
TPAD_DEFINE_VOID_ACTION(action_file_sha256, h256_file_connector(NULL))
TPAD_DEFINE_VOID_ACTION(action_file_md5, hmd5_file_connector(NULL))
TPAD_DEFINE_VOID_ACTION(action_file_base64, base64_file_connector(NULL))
TPAD_DEFINE_VOID_ACTION(action_file_base64_decode,
	                    fbase64_file_connector(NULL))

static gboolean requested_action_state(GSimpleAction *action,
	                                    GVariant *parameter)
{
	GVariant *state;
	gboolean enabled;

	if (parameter != NULL && g_variant_is_of_type(parameter,
	                                             G_VARIANT_TYPE_BOOLEAN))
		return g_variant_get_boolean(parameter);
	state = g_action_get_state(G_ACTION(action));
	enabled = state == NULL || !g_variant_get_boolean(state);
	g_clear_pointer(&state, g_variant_unref);
	return enabled;
}

static void set_boolean_action_state(GSimpleAction *action, gboolean enabled)
{
	g_simple_action_set_state(action, g_variant_new_boolean(enabled));
}

static void action_line_wrap(GSimpleAction *action, GVariant *parameter,
	                         gpointer user_data)
{
	gboolean enabled = requested_action_state(action, parameter);
	(void) user_data;
	tpad_set_line_wrap(enabled);
	set_boolean_action_state(action, enabled);
}

static void action_line_numbers(GSimpleAction *action, GVariant *parameter,
	                            gpointer user_data)
{
	gboolean enabled = requested_action_state(action, parameter);
	(void) user_data;
	tpad_set_line_numbers(enabled);
	set_boolean_action_state(action, enabled);
}

static void action_spelling(GSimpleAction *action, GVariant *parameter,
	                        gpointer user_data)
{
	gboolean enabled = requested_action_state(action, parameter);
	(void) user_data;
	if (tpad_set_spelling(enabled) != 0)
		enabled = FALSE;
	set_boolean_action_state(action, enabled);
}

static void action_language(GSimpleAction *action, GVariant *parameter,
	                        gpointer user_data)
{
	gboolean enabled = requested_action_state(action, parameter);
	(void) user_data;
	tpad_set_language_highlighting(enabled);
	set_boolean_action_state(action, enabled);
}

static void action_full_path(GSimpleAction *action, GVariant *parameter,
	                         gpointer user_data)
{
	gboolean enabled = requested_action_state(action, parameter);
	(void) user_data;
	tpad_set_show_full_path(enabled);
	set_boolean_action_state(action, enabled);
}

static void action_open_guard(GSimpleAction *action, GVariant *parameter,
	                          gpointer user_data)
{
	gboolean enabled = requested_action_state(action, parameter);
	(void) user_data;
	enabled = tpad_set_use_open_guard(enabled);
	set_boolean_action_state(action, enabled);
}

#ifdef AUTO_TAB_TOGGLE
static void action_auto_tab(GSimpleAction *action, GVariant *parameter,
	                        gpointer user_data)
{
	gboolean enabled = requested_action_state(action, parameter);
	(void) user_data;
	tpad_set_auto_tab(enabled);
	set_boolean_action_state(action, enabled);
}
#endif

static void add_boolean_action(GtkApplicationWindow *application_window,
	                           const gchar *name, gboolean initial_state,
	                           GCallback callback)
{
	GSimpleAction *action = g_simple_action_new_stateful(
		name, NULL, g_variant_new_boolean(initial_state));

	g_signal_connect(action, "activate", callback, NULL);
	g_action_map_add_action(G_ACTION_MAP(application_window),
	                        G_ACTION(action));
	g_object_unref(action);
}

static void setup_actions(GtkApplicationWindow *application_window)
{
	#define TPAD_ACTION_ENTRY(action_name, callback_name) \
		{ .name = (action_name), .activate = (callback_name) }
	#define TPAD_PARAMETER_ACTION_ENTRY(action_name, callback_name, type_name) \
		{ .name = (action_name), .activate = (callback_name), \
		  .parameter_type = (type_name) }
	static const GActionEntry entries[] = {
		TPAD_ACTION_ENTRY("new", action_new),
		TPAD_ACTION_ENTRY("open", action_open),
		TPAD_PARAMETER_ACTION_ENTRY("open-recent", action_open_recent, "s"),
		TPAD_ACTION_ENTRY("save", action_save),
		TPAD_ACTION_ENTRY("save-as", action_save_as),
		TPAD_ACTION_ENTRY("print", action_print),
		TPAD_ACTION_ENTRY("reload", action_reload),
		TPAD_ACTION_ENTRY("quit", action_quit),
		TPAD_ACTION_ENTRY("find", action_find),
		TPAD_ACTION_ENTRY("replace", action_find),
		TPAD_ACTION_ENTRY("undo", action_undo),
		TPAD_ACTION_ENTRY("redo", action_redo),
		TPAD_ACTION_ENTRY("copy-filename", action_copy_filename),
		TPAD_ACTION_ENTRY("uppercase", action_uppercase),
		TPAD_ACTION_ENTRY("lowercase", action_lowercase),
		TPAD_ACTION_ENTRY("reverse", action_reverse),
		TPAD_ACTION_ENTRY("frequency", action_frequency),
		TPAD_ACTION_ENTRY("font", action_font),
		TPAD_ACTION_ENTRY("preferences", action_preferences),
		TPAD_ACTION_ENTRY("text-stats", action_text_stats),
		TPAD_ACTION_ENTRY("about", action_about),
		TPAD_ACTION_ENTRY("sha512", action_sha512),
		TPAD_ACTION_ENTRY("sha256", action_sha256),
		TPAD_ACTION_ENTRY("md5", action_md5),
		TPAD_ACTION_ENTRY("base64", action_base64),
		TPAD_ACTION_ENTRY("base64-decode", action_base64_decode),
		TPAD_ACTION_ENTRY("file-sha512", action_file_sha512),
		TPAD_ACTION_ENTRY("file-sha256", action_file_sha256),
		TPAD_ACTION_ENTRY("file-md5", action_file_md5),
		TPAD_ACTION_ENTRY("file-base64", action_file_base64),
		TPAD_ACTION_ENTRY("file-base64-decode", action_file_base64_decode)
	};

	g_action_map_add_action_entries(G_ACTION_MAP(application_window), entries,
	                                G_N_ELEMENTS(entries), NULL);
	add_boolean_action(application_window, "line-wrap", cfg_line_wrap(),
	                   G_CALLBACK(action_line_wrap));
	add_boolean_action(application_window, "line-numbers", cfg_line(),
	                   G_CALLBACK(action_line_numbers));
	add_boolean_action(application_window, "spelling", cfg_spell(),
	                   G_CALLBACK(action_spelling));
	add_boolean_action(application_window, "language", cfg_lang(),
	                   G_CALLBACK(action_language));
	add_boolean_action(application_window, "full-path", cfg_show_full_path(),
	                   G_CALLBACK(action_full_path));
	add_boolean_action(application_window, "open-guard", cfg_use_open_guard(),
	                   G_CALLBACK(action_open_guard));
#ifdef AUTO_TAB_TOGGLE
	add_boolean_action(application_window, "auto-tab", cfg_auto_tab(),
	                   G_CALLBACK(action_auto_tab));
#endif
	#undef TPAD_PARAMETER_ACTION_ENTRY
	#undef TPAD_ACTION_ENTRY
}

static void set_accelerator(GtkApplication *application,
	                        const gchar *action, const gchar *accelerator)
{
	const gchar *accelerators[] = { accelerator, NULL };
	gtk_application_set_accels_for_action(application, action, accelerators);
}

static void setup_accelerators(GtkApplication *application)
{
	for (gsize index = 0; index < G_N_ELEMENTS(tpad_accelerators); index++)
		set_accelerator(application, tpad_accelerators[index].action,
		                tpad_accelerators[index].accelerator);
}

static GMenu *create_file_menu(void)
{
	GMenu *menu = g_menu_new();
	GMenu *section = g_menu_new();
	GMenu *final_section = g_menu_new();

	recent_menu_model = g_menu_new();
	g_object_add_weak_pointer(G_OBJECT(recent_menu_model),
	                          (gpointer *) &recent_menu_model);
	g_menu_append(section, _NEW_FILE_MNEMONIC, "win.new");
	g_menu_append(section, _OPEN_FILE_MNEMONIC, "win.open");
	g_menu_append_submenu(section, gettext("Open _Recent"),
	                      G_MENU_MODEL(recent_menu_model));
	g_object_unref(recent_menu_model);
	g_menu_append(section, _SAVE_FILE_MNEMONIC, "win.save");
	g_menu_append(section, _SAVE_FILE_AS_MNEMONIC, "win.save-as");
	g_menu_append(section, _PRINT_MNEMONIC, "win.print");
	g_menu_append_section(menu, NULL, G_MENU_MODEL(section));
	g_menu_append(final_section, _RELOAD_MNEMONIC, "win.reload");
	g_menu_append(final_section, _QUIT_MNEMONIC, "win.quit");
	g_menu_append_section(menu, NULL, G_MENU_MODEL(final_section));
	g_object_unref(section);
	g_object_unref(final_section);
	return menu;
}

static GMenuModel *create_menu_bar_model(void)
{
	const TpadMenuItem edit_items[] = {
		{ _FIND_MNEMONIC, "win.find" },
		{ _REPLACE_MNEMONIC, "win.replace" },
		{ NULL, NULL },
		{ _UNDO_MNEMONIC, "win.undo" },
		{ _REDO_MNEMONIC, "win.redo" },
		{ NULL, NULL },
		{ _CCFP_MNEMONIC, "win.copy-filename" },
		{ NULL, NULL },
		{ _REVERSE_ALL_TEXT, "win.reverse" },
		{ _MAKE_UPPERCASE, "win.uppercase" },
		{ _MAKE_LOWERCASE, "win.lowercase" },
		{ gettext("Frequency Analysis"), "win.frequency" }
	};
	const TpadMenuItem hash_items[] = {
		{ _HASH_MENU_CB_TO_512, "win.sha512" },
		{ _HASH_MENU_CB_TO_256, "win.sha256" },
		{ _HASH_MENU_CB_TO_MD5, "win.md5" },
		{ _MENU_CB_TO_BASE_64, "win.base64" },
		{ _MENU_CB_FROM_BASE_64, "win.base64-decode" },
		{ NULL, NULL },
		{ _HASH_MENU_F_TO_512, "win.file-sha512" },
		{ _HASH_MENU_F_TO_256, "win.file-sha256" },
		{ _HASH_MENU_F_TO_MD5, "win.file-md5" },
		{ _MENU_F_TO_BASE_64, "win.file-base64" },
		{ _MENU_F_FROM_BASE_64, "win.file-base64-decode" }
	};
	const TpadMenuItem settings_items[] = {
#ifdef AUTO_TAB_TOGGLE
		{ _AUTOTAB, "win.auto-tab" },
#endif
		{ _LINE_WP, "win.line-wrap" },
		{ _LINE_NUMBER, "win.line-numbers" },
		{ _SPELLING, "win.spelling" },
		{ _LANGUAGE, "win.language" },
		{ _SHOW_FULL_PATH, "win.full-path" },
		{ NULL, NULL },
		{ _SELECT_FONT_MNEMONIC, "win.font" },
		{ NULL, NULL },
		{ _PREF_MNEMONIC, "win.preferences" },
		{ _WORD_COUNT, "win.text-stats" },
		{ _CHECK_ON_OPEN, "win.open-guard" }
	};
	const TpadMenuItem help_items[] = {
		{ _ABOUT_MENU, "win.about" }
	};
	GMenu *bar = g_menu_new();
	GMenu *file = create_file_menu();
	GMenu *edit = tpad_menu_from_items(edit_items, G_N_ELEMENTS(edit_items));
	GMenu *hash = tpad_menu_from_items(hash_items, G_N_ELEMENTS(hash_items));
	GMenu *settings = tpad_menu_from_items(settings_items,
	                                      G_N_ELEMENTS(settings_items));
	GMenu *help = tpad_menu_from_items(help_items, G_N_ELEMENTS(help_items));

	g_menu_append_submenu(bar, _FILE_MENU, G_MENU_MODEL(file));
	g_menu_append_submenu(bar, _EDIT_MENU, G_MENU_MODEL(edit));
	g_menu_append_submenu(bar, _HASH_MENU, G_MENU_MODEL(hash));
	g_menu_append_submenu(bar, _SETTINGS_MENU, G_MENU_MODEL(settings));
	g_menu_append_submenu(bar, _HELP_MENU, G_MENU_MODEL(help));
	g_object_unref(file);
	g_object_unref(edit);
	g_object_unref(hash);
	g_object_unref(settings);
	g_object_unref(help);
	return G_MENU_MODEL(bar);
}

static GtkWidget *action_button(const gchar *label, const gchar *action)
{
	GtkWidget *button = gtk_button_new_with_label(label);
	gtk_actionable_set_action_name(GTK_ACTIONABLE(button), action);
	return button;
}

static GtkWidget *create_toolbar(void)
{
	GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);

	gtk_widget_add_css_class(toolbar, "toolbar");
	gtk_widget_set_margin_start(toolbar, 4);
	gtk_widget_set_margin_end(toolbar, 4);
	gtk_widget_set_margin_top(toolbar, 4);
	gtk_widget_set_margin_bottom(toolbar, 4);
	gtk_box_append(GTK_BOX(toolbar), action_button(_NEW_TOOLBAR, "win.new"));
	gtk_box_append(GTK_BOX(toolbar), action_button(_OPEN_TOOLBAR, "win.open"));
	gtk_box_append(GTK_BOX(toolbar), action_button(_SAVE_TOOLBAR, "win.save"));
	gtk_box_append(GTK_BOX(toolbar),
	               action_button(_SAVE_AS_TOOLBAR, "win.save-as"));
	gtk_box_append(GTK_BOX(toolbar), gtk_separator_new(GTK_ORIENTATION_VERTICAL));
	gtk_box_append(GTK_BOX(toolbar),
	               action_button(_TOOLRELOAD_FILE, "win.reload"));
	gtk_box_append(GTK_BOX(toolbar), gtk_separator_new(GTK_ORIENTATION_VERTICAL));
	gtk_box_append(GTK_BOX(toolbar), action_button(_UNDO_TOOLBAR, "win.undo"));
	gtk_box_append(GTK_BOX(toolbar), action_button(_REDO_TOOLBAR, "win.redo"));
	gtk_box_append(GTK_BOX(toolbar), gtk_separator_new(GTK_ORIENTATION_VERTICAL));
	gtk_box_append(GTK_BOX(toolbar), action_button(_QUIT_TOOLBAR, "win.quit"));
	return toolbar;
}

static gboolean on_editor_file_drop(GtkDropTarget *target,
	                                 const GValue *value,
	                                 gdouble x, gdouble y,
	                                 gpointer user_data)
{
	GtkTextView *text_view = GTK_TEXT_VIEW(user_data);
	GtkTextBuffer *buffer;
	GdkFileList *file_list;
	GPtrArray *paths;
	GSList *files;
	GSList *item;
	GtkTextIter iter;
	gchar *formatted;
	gboolean inserted;
	gint buffer_x;
	gint buffer_y;

	(void) target;
	if (!G_VALUE_HOLDS(value, GDK_TYPE_FILE_LIST))
		return FALSE;

	file_list = g_value_get_boxed(value);
	if (file_list == NULL)
		return FALSE;

	paths = g_ptr_array_new_with_free_func(g_free);
	files = gdk_file_list_get_files(file_list);
	for (item = files; item != NULL; item = item->next) {
		GFile *file = G_FILE(item->data);
		gchar *path = g_file_get_path(file);

		if (path == NULL)
			path = g_file_get_uri(file);
		if (path != NULL)
			g_ptr_array_add(paths, path);
	}
	g_slist_free(files);

	formatted = tpad_string_format_drop_paths(
		(const gchar *const *) paths->pdata, paths->len);
	g_ptr_array_unref(paths);
	if (formatted[0] == '\0') {
		g_free(formatted);
		return FALSE;
	}

	gtk_text_view_window_to_buffer_coords(text_view,
	                                      GTK_TEXT_WINDOW_WIDGET,
	                                      (gint) x, (gint) y,
	                                      &buffer_x, &buffer_y);
	gtk_text_view_get_iter_at_location(text_view, &iter, buffer_x, buffer_y);
	buffer = gtk_text_view_get_buffer(text_view);
	gtk_text_buffer_begin_user_action(buffer);
	inserted = gtk_text_buffer_insert_interactive(
		buffer, &iter, formatted, -1,
		gtk_text_view_get_editable(text_view));
	if (inserted) {
		gtk_text_buffer_place_cursor(buffer, &iter);
		gtk_widget_grab_focus(GTK_WIDGET(text_view));
	}
	gtk_text_buffer_end_user_action(buffer);
	g_free(formatted);

	return inserted;
}

static gboolean activate_editor_action(gpointer user_data)
{
	const gchar *action = user_data;

	if (window != NULL)
		(void) gtk_widget_activate_action(window, action, NULL);
	return G_SOURCE_REMOVE;
}

static gboolean on_editor_key_pressed(GtkEventControllerKey *controller,
	                                  guint keyval, guint keycode,
	                                  GdkModifierType state,
	                                  gpointer user_data)
{
	const GdkModifierType action_modifiers =
		GDK_CONTROL_MASK | GDK_ALT_MASK | GDK_SHIFT_MASK |
		GDK_SUPER_MASK | GDK_META_MASK;
	GtkTextIter iter;

	(void) controller;
	(void) keycode;
	(void) user_data;
	/* GtkTextView owns Emacs-style bindings such as Ctrl+P, Ctrl+N, Ctrl+H,
	 * and Ctrl+U.  Dispatch Tpad's historical actions during capture before
	 * the editor consumes those key events. */
	for (gsize index = 0; index < G_N_ELEMENTS(tpad_accelerators); index++) {
		if (gdk_keyval_to_lower(keyval) == tpad_accelerators[index].keyval &&
		    (state & action_modifiers) ==
			    tpad_accelerators[index].modifiers) {
			g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,
			                activate_editor_action,
			                g_strdup(tpad_accelerators[index].action),
			                g_free);
			return TRUE;
		}
	}
	if ((state & action_modifiers) != 0)
		return FALSE;
	if (keyval == GDK_KEY_End)
		gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(mBuff), &iter);
	else if (keyval == GDK_KEY_Home)
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(mBuff), &iter);
	else
		return FALSE;
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(mBuff), &iter);
	return TRUE;
}

static void update_screen_dimensions(void)
{
	gint width = _DEFAULT_WINDOW_WIDTH;
	gint height = _DEFAULT_WINDOW_HEIGHT;
	GListModel *monitors;

	Display = gdk_display_get_default();
	monitors = Display != NULL ? gdk_display_get_monitors(Display) : NULL;
	if (monitors != NULL && g_list_model_get_n_items(monitors) > 0) {
		GdkMonitor *monitor = g_list_model_get_item(monitors, 0);
		GdkRectangle geometry;

		gdk_monitor_get_geometry(monitor, &geometry);
		if (geometry.width > 0)
			width = geometry.width;
		if (geometry.height > 0)
			height = geometry.height;
		g_object_unref(monitor);
	}
	cfg_set_screen_width(width);
	cfg_set_screen_height(height);
}

GtkWidget *tpad_new_ui(GtkApplication *application)
{
	GtkApplicationWindow *application_window;
	GtkWidget *main_box;
	GtkWidget *menu_bar;
	GtkWidget *scrolled_window;
	GtkEventController *key_controller;
	GtkDropTarget *file_drop_target;
	GMenuModel *menu_model;

	update_screen_dimensions();
	window = gtk_application_window_new(application);
	application_window = GTK_APPLICATION_WINDOW(window);
	gtk_window_set_title(GTK_WINDOW(window), "Tpad");
	gtk_window_set_default_size(GTK_WINDOW(window), cfg_wWidth(), cfg_wHeight());
	gtk_window_set_icon_name(GTK_WINDOW(window), "tpad");
	setup_actions(application_window);
	setup_accelerators(application);

	main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	menu_model = create_menu_bar_model();
	menu_bar = gtk_popover_menu_bar_new_from_model(menu_model);
	g_object_unref(menu_model);
	gtk_box_append(GTK_BOX(main_box), menu_bar);
	gtk_box_append(GTK_BOX(main_box), create_toolbar());

	mBuff = gtk_source_buffer_new(NULL);
	gtk_text_buffer_set_enable_undo(GTK_TEXT_BUFFER(mBuff), TRUE);
	view = GTK_SOURCE_VIEW(gtk_source_view_new_with_buffer(mBuff));
	gtk_source_view_set_show_line_numbers(view, cfg_line());
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),
	                            cfg_line_wrap() ? GTK_WRAP_WORD : GTK_WRAP_NONE);
	gtk_source_view_set_smart_home_end(view, GTK_SOURCE_SMART_HOME_END_BEFORE);
	gtk_widget_set_hexpand(GTK_WIDGET(view), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(view), TRUE);
	tpad_set_font("DejaVu Sans Mono 12");
	file_drop_target = gtk_drop_target_new(GDK_TYPE_FILE_LIST,
	                                       GDK_ACTION_COPY);
	gtk_event_controller_set_propagation_phase(
		GTK_EVENT_CONTROLLER(file_drop_target), GTK_PHASE_CAPTURE);
	g_signal_connect(file_drop_target, "drop",
	                 G_CALLBACK(on_editor_file_drop), view);
	gtk_widget_add_controller(GTK_WIDGET(view),
	                          GTK_EVENT_CONTROLLER(file_drop_target));

	key_controller = gtk_event_controller_key_new();
	gtk_event_controller_set_propagation_phase(key_controller,
	                                          GTK_PHASE_CAPTURE);
	g_signal_connect(key_controller, "key-pressed",
	                 G_CALLBACK(on_editor_key_pressed), NULL);
	gtk_widget_add_controller(window, key_controller);

	scrolled_window = gtk_scrolled_window_new();
	gtk_widget_set_hexpand(scrolled_window, TRUE);
	gtk_widget_set_vexpand(scrolled_window, TRUE);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window),
	                              GTK_WIDGET(view));
	gtk_box_append(GTK_BOX(main_box), scrolled_window);
	gtk_window_set_child(GTK_WINDOW(window), main_box);
	g_signal_connect(window, "close-request",
	                 G_CALLBACK(on_window_close_request), NULL);
	tpad_ui_refresh_recent_menu();
	if (cfg_spell() && tpad_set_spelling(TRUE) != 0) {
		GAction *spell_action = g_action_map_lookup_action(
			G_ACTION_MAP(application_window), "spelling");
		if (spell_action != NULL)
			g_simple_action_set_state(G_SIMPLE_ACTION(spell_action),
			                          g_variant_new_boolean(FALSE));
	}
	return window;
}
