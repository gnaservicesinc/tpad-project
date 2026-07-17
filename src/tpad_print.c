
/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_print.c , is part of tpad.
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

extern GtkWidget *window;
extern GtkSourceView *view;

static gboolean tpad_print_paginate(GtkPrintOperation *operation,
									GtkPrintContext *context,
									gpointer user_data)
{
	GtkSourcePrintCompositor *compositor;

	compositor = GTK_SOURCE_PRINT_COMPOSITOR(user_data);
	if (!gtk_source_print_compositor_paginate(compositor, context))
		return FALSE;

	gtk_print_operation_set_n_pages(
		operation, gtk_source_print_compositor_get_n_pages(compositor));
	return TRUE;
}

static void tpad_print_draw_page(GtkPrintOperation *operation,
								 GtkPrintContext *context,
								 gint page_number,
								 gpointer user_data)
{
	(void) operation;
	gtk_source_print_compositor_draw_page(
		GTK_SOURCE_PRINT_COMPOSITOR(user_data), context, page_number);
}

static void tpad_print_show_error(const GError *error)
{
	GtkWidget *dialog;
	const gchar *detail;

	detail = error != NULL ? error->message : _PRINT_FAILED_UNKNOWN;
	g_warning("%s: %s", _PRINT_FAILED, detail);

	dialog = gtk_message_dialog_new(GTK_WINDOW(window),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", _PRINT_FAILED);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
										"%s", detail);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

static GtkPrintOperationResult tpad_print_run(GtkPrintOperationAction action,
											  const gchar *export_filename)
{
	GtkPrintOperation *operation;
	GtkSourcePrintCompositor *compositor;
	GtkPrintOperationResult result;
	const gchar *job_name;
	GError *error = NULL;

	operation = gtk_print_operation_new();
	compositor = gtk_source_print_compositor_new_from_view(view);
	job_name = gtk_window_get_title(GTK_WINDOW(window));

	gtk_print_operation_set_job_name(operation,
		job_name != NULL && *job_name != '\0' ? job_name : "Tpad");
	/* Make paper size and orientation available in the system print dialog. */
	gtk_print_operation_set_embed_page_setup(operation, TRUE);
	if (export_filename != NULL)
		gtk_print_operation_set_export_filename(operation, export_filename);

	g_signal_connect(operation, "paginate",
		G_CALLBACK(tpad_print_paginate), compositor);
	g_signal_connect(operation, "draw-page",
		G_CALLBACK(tpad_print_draw_page), compositor);

	result = gtk_print_operation_run(operation, action,
		GTK_WINDOW(window), &error);
	if (result == GTK_PRINT_OPERATION_RESULT_ERROR) {
		if (action == GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG)
			tpad_print_show_error(error);
		else
			g_warning("%s: %s", _PRINT_FAILED,
				error != NULL ? error->message : _PRINT_FAILED_UNKNOWN);
	}

	g_clear_error(&error);
	g_object_unref(compositor);
	g_object_unref(operation);
	return result;
}

void tpad_print_document(void)
{
	(void) tpad_print_run(GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, NULL);
}

gboolean tpad_print_export_for_test(const gchar *filename)
{
	GtkPrintOperationResult result;

	if (filename == NULL || *filename == '\0')
		return FALSE;

	result = tpad_print_run(GTK_PRINT_OPERATION_ACTION_EXPORT, filename);
	return result != GTK_PRINT_OPERATION_RESULT_ERROR &&
	       result != GTK_PRINT_OPERATION_RESULT_CANCEL;
}

void print(const gchar *s){
	(void) s;
	// Stay silent if debugging is disabled. 
	#ifdef DEBUG_TOGGLE
	g_print("tpad([%s]v.%s): %s\n",GDK_CURRENT_TIME,VERSION,s);
	#endif
}

void print_title(gchar* str){
	(void) str;
	// Stay silent if debugging is disabled. 
	#ifdef DEBUG_TOGGLE
	gint ihHash=0, ihStr=0,ihTmp=0;
	register gint i=0;
	ihHash=HASH_PRINT_COUNT / 2;
	ihStr=str_size(str) / 2;
	ihTmp= (gint) ihHash - ihStr;
	if (ihTmp < 0) ihTmp = 0;
	for (i = 0; i < HASH_PRINT_COUNT; i++) {
		 printf("\x23");
	}
	printf("\n");
	for (i = 0; i < ihTmp; i++) {
		printf(" ");
	}
	printf("%s\n",str);
	for (i = 0; i < HASH_PRINT_COUNT; i++) {
		printf("\x23");
	}
	printf("\nTIMESTAMP\t=\t%s\n",GDK_CURRENT_TIME);
	#endif
}
