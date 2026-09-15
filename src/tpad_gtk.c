/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2026 Andrew Smith (GNA SERVICES INC)
 *     All Rights Reserved.
 *
 *   This file, tpad_gtk.c, is part of tpad.
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

#include "tpad_gtk.h"

typedef enum {
	TPAD_ASYNC_ALERT_CHOOSE,
	TPAD_ASYNC_CLIPBOARD_READ_TEXT,
	TPAD_ASYNC_CLIPBOARD_STORE,
	TPAD_ASYNC_FONT_CHOOSE,
	TPAD_ASYNC_FILE_OPEN,
	TPAD_ASYNC_FILE_OPEN_MULTIPLE,
	TPAD_ASYNC_FILE_SAVE
} TpadAsyncOperation;

typedef struct {
	GMainLoop *loop;
	TpadAsyncOperation operation;
	GError *error;
	gpointer pointer_result;
	gint integer_result;
	gboolean boolean_result;
	gboolean completed;
} TpadAsyncWait;

static void tpad_async_wait_init(TpadAsyncWait *wait,
	                         TpadAsyncOperation operation)
{
	GMainContext *context;

	context = g_main_context_ref_thread_default();
	wait->loop = g_main_loop_new(context, FALSE);
	wait->operation = operation;
	wait->error = NULL;
	wait->pointer_result = NULL;
	wait->integer_result = -1;
	wait->boolean_result = FALSE;
	wait->completed = FALSE;
	g_main_context_unref(context);
}

static void tpad_async_wait_ready(GObject *source_object,
				  GAsyncResult *result, gpointer user_data)
{
	TpadAsyncWait *wait = user_data;

	/* GTK's async/finish contract requires the finish call to happen in the
	 * supplied completion callback.  Retaining GAsyncResult until after this
	 * callback returns can let a native dialog tear down its window before its
	 * finish function consumes the result. */
	switch (wait->operation) {
	case TPAD_ASYNC_ALERT_CHOOSE:
		wait->integer_result = gtk_alert_dialog_choose_finish(
			GTK_ALERT_DIALOG(source_object), result, &wait->error);
		break;
	case TPAD_ASYNC_CLIPBOARD_READ_TEXT:
		wait->pointer_result = gdk_clipboard_read_text_finish(
			GDK_CLIPBOARD(source_object), result, &wait->error);
		break;
	case TPAD_ASYNC_CLIPBOARD_STORE:
		wait->boolean_result = gdk_clipboard_store_finish(
			GDK_CLIPBOARD(source_object), result, &wait->error);
		break;
	case TPAD_ASYNC_FONT_CHOOSE:
		wait->pointer_result = gtk_font_dialog_choose_font_finish(
			GTK_FONT_DIALOG(source_object), result, &wait->error);
		break;
	case TPAD_ASYNC_FILE_OPEN:
		wait->pointer_result = gtk_file_dialog_open_finish(
			GTK_FILE_DIALOG(source_object), result, &wait->error);
		break;
	case TPAD_ASYNC_FILE_OPEN_MULTIPLE:
		wait->pointer_result = gtk_file_dialog_open_multiple_finish(
			GTK_FILE_DIALOG(source_object), result, &wait->error);
		break;
	case TPAD_ASYNC_FILE_SAVE:
		wait->pointer_result = gtk_file_dialog_save_finish(
			GTK_FILE_DIALOG(source_object), result, &wait->error);
		break;
	}
	wait->completed = TRUE;
	g_main_loop_quit(wait->loop);
}

static void tpad_async_wait_run(TpadAsyncWait *wait)
{
	if (!wait->completed)
		g_main_loop_run(wait->loop);
}

static void tpad_async_wait_clear(TpadAsyncWait *wait)
{
	g_clear_error(&wait->error);
	g_clear_pointer(&wait->loop, g_main_loop_unref);
}

static gboolean tpad_dialog_error_was_dismissal(const GError *error)
{
	return error != NULL &&
	       (g_error_matches(error, GTK_DIALOG_ERROR,
				GTK_DIALOG_ERROR_CANCELLED) ||
		g_error_matches(error, GTK_DIALOG_ERROR,
				GTK_DIALOG_ERROR_DISMISSED));
}

int tpad_alert_choose(GtkWindow *parent, const gchar *message,
			      const gchar *detail,
			      const gchar *const *buttons,
			      int default_button, int cancel_button)
{
	GtkAlertDialog *dialog;
	TpadAsyncWait wait;
	GError *error = NULL;
	int response;

	dialog = gtk_alert_dialog_new("%s", message != NULL ? message : "");
	gtk_alert_dialog_set_modal(dialog, TRUE);
	if (detail != NULL)
		gtk_alert_dialog_set_detail(dialog, detail);
	if (buttons != NULL)
		gtk_alert_dialog_set_buttons(dialog, buttons);
	gtk_alert_dialog_set_default_button(dialog, default_button);
	gtk_alert_dialog_set_cancel_button(dialog, cancel_button);

	tpad_async_wait_init(&wait, TPAD_ASYNC_ALERT_CHOOSE);
	gtk_alert_dialog_choose(dialog, parent, NULL, tpad_async_wait_ready,
				&wait);
	tpad_async_wait_run(&wait);
	response = wait.integer_result;
	error = g_steal_pointer(&wait.error);
	if (error != NULL) {
		if (!tpad_dialog_error_was_dismissal(error))
			g_warning("Unable to show alert dialog: %s", error->message);
		response = cancel_button;
	}

	g_clear_error(&error);
	tpad_async_wait_clear(&wait);
	g_object_unref(dialog);
	return response;
}

void tpad_alert_show(GtkWindow *parent, const gchar *message,
			     const gchar *detail)
{
	GtkAlertDialog *dialog;

	dialog = gtk_alert_dialog_new("%s", message != NULL ? message : "");
	gtk_alert_dialog_set_modal(dialog, TRUE);
	if (detail != NULL)
		gtk_alert_dialog_set_detail(dialog, detail);
	gtk_alert_dialog_show(dialog, parent);
	g_object_unref(dialog);
}

gchar *tpad_clipboard_read_primary_text(void)
{
	GdkDisplay *display;
	GdkClipboard *clipboard;
	TpadAsyncWait wait;
	GError *error = NULL;
	gchar *text;

	display = gdk_display_get_default();
	if (display == NULL)
		return NULL;

	clipboard = gdk_display_get_primary_clipboard(display);
	tpad_async_wait_init(&wait, TPAD_ASYNC_CLIPBOARD_READ_TEXT);
	gdk_clipboard_read_text_async(clipboard, NULL, tpad_async_wait_ready,
				      &wait);
	tpad_async_wait_run(&wait);
	text = g_steal_pointer(&wait.pointer_result);
	error = g_steal_pointer(&wait.error);
	if (error != NULL &&
	    !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
		g_debug("Unable to read primary clipboard text: %s",
			error->message);

	g_clear_error(&error);
	tpad_async_wait_clear(&wait);
	return text;
}

static void tpad_clipboard_store(GdkClipboard *clipboard)
{
	TpadAsyncWait wait;
	GError *error = NULL;

	if (clipboard == NULL)
		return;
	tpad_async_wait_init(&wait, TPAD_ASYNC_CLIPBOARD_STORE);
	gdk_clipboard_store_async(clipboard, G_PRIORITY_DEFAULT, NULL,
	                          tpad_async_wait_ready, &wait);
	tpad_async_wait_run(&wait);
	error = g_steal_pointer(&wait.error);
	if (!wait.boolean_result &&
	    error != NULL &&
	    !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
		g_debug("Unable to persist clipboard text: %s", error->message);
	g_clear_error(&error);
	tpad_async_wait_clear(&wait);
}

void tpad_clipboard_set_text(const gchar *text)
{
	GdkDisplay *display;
	GdkClipboard *clipboard;

	if (text == NULL)
		return;
	display = gdk_display_get_default();
	if (display == NULL)
		return;

	clipboard = gdk_display_get_clipboard(display);
	gdk_clipboard_set_text(clipboard, text);
	/* Complete the compositor/clipboard-manager handoff now so copied results
	 * remain available when a one-document Tpad process closes immediately. */
	tpad_clipboard_store(clipboard);
}

void tpad_clipboards_store(void)
{
	GdkDisplay *display = gdk_display_get_default();
	GdkClipboard *primary;
	GdkClipboard *clipboard;

	if (display == NULL)
		return;
	primary = gdk_display_get_primary_clipboard(display);
	clipboard = gdk_display_get_clipboard(display);
	tpad_clipboard_store(primary);
	if (clipboard != primary)
		tpad_clipboard_store(clipboard);
}

PangoFontDescription *tpad_choose_font(
	GtkWindow *parent, const gchar *title,
	const PangoFontDescription *initial_value)
{
	GtkFontDialog *dialog;
	PangoFontDescription *initial_copy = NULL;
	PangoFontDescription *font;
	TpadAsyncWait wait;
	GError *error = NULL;

	dialog = gtk_font_dialog_new();
	gtk_font_dialog_set_modal(dialog, TRUE);
	if (title != NULL)
		gtk_font_dialog_set_title(dialog, title);
	if (initial_value != NULL)
		initial_copy = pango_font_description_copy(initial_value);

	tpad_async_wait_init(&wait, TPAD_ASYNC_FONT_CHOOSE);
	gtk_font_dialog_choose_font(dialog, parent, initial_copy, NULL,
				    tpad_async_wait_ready, &wait);
	tpad_async_wait_run(&wait);
	font = g_steal_pointer(&wait.pointer_result);
	error = g_steal_pointer(&wait.error);
	if (error != NULL && !tpad_dialog_error_was_dismissal(error))
		g_warning("Unable to show font dialog: %s", error->message);

	g_clear_error(&error);
	tpad_async_wait_clear(&wait);
	pango_font_description_free(initial_copy);
	g_object_unref(dialog);
	return font;
}

static GtkFileDialog *tpad_file_dialog_new(const gchar *title,
					   const gchar *initial_path)
{
	GtkFileDialog *dialog;
	GFile *initial_file = NULL;
	gchar *filename;

	dialog = gtk_file_dialog_new();
	gtk_file_dialog_set_modal(dialog, TRUE);
	if (title != NULL)
		gtk_file_dialog_set_title(dialog, title);
	if (initial_path == NULL || *initial_path == '\0')
		return dialog;

	filename = tpad_filename_from_utf8(initial_path);
	if (filename == NULL)
		return dialog;
	initial_file = g_file_new_for_path(filename);
	if (g_file_test(filename, G_FILE_TEST_IS_DIR))
		gtk_file_dialog_set_initial_folder(dialog, initial_file);
	else
		gtk_file_dialog_set_initial_file(dialog, initial_file);
	g_object_unref(initial_file);
	g_free(filename);
	return dialog;
}

static void tpad_file_dialog_report_error(const gchar *operation,
					  const GError *error)
{
	if (error != NULL && !tpad_dialog_error_was_dismissal(error))
		g_warning("Unable to %s with file dialog: %s", operation,
			  error->message);
}

GFile *tpad_file_dialog_open(GtkWindow *parent, const gchar *title,
			     const gchar *initial_path)
{
	GtkFileDialog *dialog;
	TpadAsyncWait wait;
	GError *error = NULL;
	GFile *file;

	dialog = tpad_file_dialog_new(title, initial_path);
	tpad_async_wait_init(&wait, TPAD_ASYNC_FILE_OPEN);
	gtk_file_dialog_open(dialog, parent, NULL, tpad_async_wait_ready, &wait);
	tpad_async_wait_run(&wait);
	file = g_steal_pointer(&wait.pointer_result);
	error = g_steal_pointer(&wait.error);
	tpad_file_dialog_report_error("open a file", error);

	g_clear_error(&error);
	tpad_async_wait_clear(&wait);
	g_object_unref(dialog);
	return file;
}

GListModel *tpad_file_dialog_open_multiple(GtkWindow *parent,
					   const gchar *title)
{
	GtkFileDialog *dialog;
	TpadAsyncWait wait;
	GError *error = NULL;
	GListModel *files;

	dialog = tpad_file_dialog_new(title, NULL);
	tpad_async_wait_init(&wait, TPAD_ASYNC_FILE_OPEN_MULTIPLE);
	gtk_file_dialog_open_multiple(dialog, parent, NULL,
				      tpad_async_wait_ready, &wait);
	tpad_async_wait_run(&wait);
	files = g_steal_pointer(&wait.pointer_result);
	error = g_steal_pointer(&wait.error);
	tpad_file_dialog_report_error("open files", error);

	g_clear_error(&error);
	tpad_async_wait_clear(&wait);
	g_object_unref(dialog);
	return files;
}

GFile *tpad_file_dialog_save(GtkWindow *parent, const gchar *title,
			     const gchar *initial_path,
			     const gchar *initial_name)
{
	GtkFileDialog *dialog;
	TpadAsyncWait wait;
	GError *error = NULL;
	GFile *file;

	dialog = tpad_file_dialog_new(title, initial_path);
	if (initial_name != NULL && *initial_name != '\0')
		gtk_file_dialog_set_initial_name(dialog, initial_name);
	tpad_async_wait_init(&wait, TPAD_ASYNC_FILE_SAVE);
	gtk_file_dialog_save(dialog, parent, NULL, tpad_async_wait_ready, &wait);
	tpad_async_wait_run(&wait);
	file = g_steal_pointer(&wait.pointer_result);
	error = g_steal_pointer(&wait.error);
	tpad_file_dialog_report_error("save a file", error);

	g_clear_error(&error);
	tpad_async_wait_clear(&wait);
	g_object_unref(dialog);
	return file;
}
