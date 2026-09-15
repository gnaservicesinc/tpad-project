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
extern GtkSourceBuffer *mBuff;
extern GtkSourceView *view;
extern GtkWidget *window;
extern gboolean searchCase;
extern gboolean doCOVT;
extern unsigned int sdone;
extern GtkWidget *findentry,*replaceentry;
extern GtkTextIter match_start,match_end;
extern void do_find(GtkWidget *caller, GtkWidget *dialog);
unsigned int sdone=0;
GtkWidget *findentry;
GtkWidget *replaceentry;
gboolean searchCase=FALSE;
gboolean doCOVT=FALSE;
GtkTextIter match_start,match_end;
static gboolean find_match_found = FALSE;
static GtkWidget *find_dialog_window = NULL;

static gchar *current_find_text(void)
{
	const gchar *text;

	if (findentry == NULL)
		return NULL;
	text = gtk_editable_get_text(GTK_EDITABLE(findentry));
	return doCOVT ? g_strcompress(text) : g_strdup(text);
}

static gboolean current_selection_matches(const gchar *findwhat)
{
	GtkTextIter selection_start;
	GtkTextIter selection_end;
	gchar *selected;
	gboolean matches;

	if (findwhat == NULL || findwhat[0] == '\0' ||
	    !gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(mBuff),
	                                          &selection_start, &selection_end))
		return FALSE;
	selected = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),
	                                    &selection_start, &selection_end, FALSE);
	if (searchCase) {
		matches = g_strcmp0(selected, findwhat) == 0;
	} else {
		gchar *selected_folded = g_utf8_casefold(selected, -1);
		gchar *find_folded = g_utf8_casefold(findwhat, -1);

		matches = g_strcmp0(selected_folded, find_folded) == 0;
		g_free(find_folded);
		g_free(selected_folded);
	}
	g_free(selected);
	if (!matches)
		return FALSE;
	match_start = selection_start;
	match_end = selection_end;
	return TRUE;
}

static void find_dialog_finalized(gpointer user_data, GObject *dialog)
{
	(void) dialog;
	(void) user_data;
	find_dialog_window = NULL;
	findentry = NULL;
	replaceentry = NULL;
	find_match_found = FALSE;
}

static gboolean find_dialog_key_pressed(GtkEventControllerKey *controller,
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

static void append_expanding(GtkBox *box, GtkWidget *child)
{
	gtk_widget_set_hexpand(child, TRUE);
	gtk_box_append(box, child);
}

void do_find(GtkWidget *caller, GtkWidget *dialog)
{
	gchar *findwhat;
	GtkTextSearchFlags flags;
	GtkTextIter iter;

	(void) caller;
	(void) dialog;
	find_match_found = FALSE;
	findwhat = current_find_text();
	if (findwhat == NULL || *findwhat == '\0') {
		g_free(findwhat);
		return;
	}
	flags = searchCase ? GTK_TEXT_SEARCH_TEXT_ONLY
	                   : GTK_TEXT_SEARCH_CASE_INSENSITIVE;

	if (gtk_text_buffer_get_has_selection(GTK_TEXT_BUFFER(mBuff)))
		gtk_text_buffer_get_iter_at_mark(
		        GTK_TEXT_BUFFER(mBuff), &iter,
		        gtk_text_buffer_get_selection_bound(GTK_TEXT_BUFFER(mBuff)));
	else
		gtk_text_buffer_get_iter_at_mark(
		        GTK_TEXT_BUFFER(mBuff), &iter,
		        gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(mBuff)));

	find_match_found = gtk_text_iter_forward_search(
	        &iter, findwhat, flags, &match_start, &match_end, NULL);
	if (!find_match_found) {
		GtkTextIter limit = iter;

		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(mBuff), &iter);
		find_match_found = gtk_text_iter_forward_search(
		        &iter, findwhat, flags, &match_start, &match_end, &limit);
		sdone++;
	}
	if (find_match_found) {
		gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view), &match_end,
		                             0.0, FALSE, .5, .1);
		gtk_text_buffer_select_range(GTK_TEXT_BUFFER(mBuff),
		                             &match_start, &match_end);
	}
	g_free(findwhat);
}



void do_replace(GtkWidget *caller, GtkWidget *dialog){	
	gchar *replacement = NULL;
	gchar *findwhat;
	const gchar *replacement_text;

	if (replaceentry == NULL)
		return;
	findwhat = current_find_text();
	if (!current_selection_matches(findwhat)) {
		g_free(findwhat);
		do_find(caller, dialog);
		if (!find_match_found)
			return;
	} else {
		find_match_found = TRUE;
		g_free(findwhat);
	}
	if(doCOVT) {
		replacement = g_strcompress(gtk_editable_get_text(
			GTK_EDITABLE(replaceentry)));
		replacement_text = replacement;
	} else {
		replacement_text = gtk_editable_get_text(GTK_EDITABLE(replaceentry));
	}
	gtk_text_buffer_begin_user_action(GTK_TEXT_BUFFER(mBuff));
	gtk_text_buffer_delete(GTK_TEXT_BUFFER(mBuff), &match_start, &match_end);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(mBuff), &match_start,
	                       replacement_text, -1);
	gtk_text_buffer_end_user_action(GTK_TEXT_BUFFER(mBuff));
	find_match_found = FALSE;
	g_free(replacement);
}


void do_find_replace(GtkWidget *caller, GtkWidget *dialog){
	(void) caller;
	(void) dialog;
	opt_find_replace();
}
void replace_dialog(void){
	GtkEventController *key_controller;
	GtkWidget *dialog;
	GtkWidget *mainbox;
	GtkWidget *buttonbox;
	GtkWidget *findbox;
	GtkWidget *replacebox;
	GtkWidget *separators[3];
	GtkWidget *buttonfind;
	GtkWidget *buttonreplace;
	GtkWidget *buttonfindreplace;
	GtkWidget *casebutton;
	GtkWidget *convtbutton;
	gchar *received;
	gchar *escaped = NULL;

	if (find_dialog_window != NULL) {
		gtk_window_present(GTK_WINDOW(find_dialog_window));
		return;
	}

	sdone = 0;
	dialog = gtk_window_new();
	find_dialog_window = dialog;
	g_object_weak_ref(G_OBJECT(dialog), find_dialog_finalized, NULL);
	key_controller = gtk_event_controller_key_new();
	g_signal_connect(key_controller, "key-pressed",
	                 G_CALLBACK(find_dialog_key_pressed), dialog);
	gtk_widget_add_controller(dialog, key_controller);
	for (guint index = 0; index < G_N_ELEMENTS(separators); index++)
		separators[index] = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
	gtk_window_set_title(GTK_WINDOW(dialog), _FIND_FIND_AND_REPLACE);
	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
	if (window != NULL && GTK_IS_WINDOW(window)) {
		gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
		gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
	}

	mainbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	findbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	replacebox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	buttonbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	gtk_widget_set_margin_start(mainbox, 6);
	gtk_widget_set_margin_end(mainbox, 6);
	gtk_widget_set_margin_top(mainbox, 6);
	gtk_widget_set_margin_bottom(mainbox, 6);

	gtk_window_set_child(GTK_WINDOW(dialog), mainbox);
	append_expanding(GTK_BOX(mainbox), findbox);
	append_expanding(GTK_BOX(mainbox), replacebox);
	append_expanding(GTK_BOX(mainbox), buttonbox);
	append_expanding(GTK_BOX(findbox), gtk_label_new(_SEARCH_FOR));

	received = tpad_clipboard_read_primary_text();
	findentry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(findentry), 65536);
	gtk_entry_set_input_purpose(GTK_ENTRY(findentry),
	                            GTK_INPUT_PURPOSE_FREE_FORM);
	if (received != NULL) {
		escaped = g_strescape(received, NULL);
		gtk_editable_set_text(GTK_EDITABLE(findentry),
		                       escaped != NULL ? escaped : received);
	}
	g_free(escaped);
	g_free(received);

	append_expanding(GTK_BOX(findbox), findentry);
	gtk_box_append(GTK_BOX(findbox), separators[0]);
	append_expanding(GTK_BOX(replacebox), gtk_label_new(_REPLACE_WITH));

	replaceentry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(replaceentry), 65536);
	gtk_entry_set_input_purpose(GTK_ENTRY(replaceentry),
	                            GTK_INPUT_PURPOSE_FREE_FORM);
	append_expanding(GTK_BOX(replacebox), replaceentry);
	gtk_box_append(GTK_BOX(replacebox), separators[1]);

	buttonfind = gtk_button_new_with_label(_FIND);
	append_expanding(GTK_BOX(buttonbox), buttonfind);
	buttonreplace = gtk_button_new_with_label(_REPLACE);
	append_expanding(GTK_BOX(buttonbox), buttonreplace);
	gtk_box_append(GTK_BOX(buttonbox), separators[2]);
	buttonfindreplace = gtk_button_new_with_label(_FIND_AND_REPLACE);
	append_expanding(GTK_BOX(buttonbox), buttonfindreplace);

	casebutton = gtk_check_button_new_with_label(_MATCH_CASE);
	append_expanding(GTK_BOX(buttonbox), casebutton);
	gtk_check_button_set_active(GTK_CHECK_BUTTON(casebutton), searchCase);
	g_signal_connect(casebutton, "toggled",
	                 G_CALLBACK(toggle_case_sarch), NULL);

	convtbutton = gtk_check_button_new_with_label("COVT");
	append_expanding(GTK_BOX(buttonbox), convtbutton);
	doCOVT = TRUE;
	gtk_check_button_set_active(GTK_CHECK_BUTTON(convtbutton), doCOVT);
	g_signal_connect(convtbutton, "toggled", G_CALLBACK(toggle_covt), NULL);

	g_signal_connect(buttonfind, "clicked", G_CALLBACK(do_find), dialog);
	g_signal_connect(buttonreplace, "clicked", G_CALLBACK(do_replace), dialog);
	g_signal_connect(buttonfindreplace, "clicked",
	                 G_CALLBACK(do_find_replace), dialog);
	gtk_window_present(GTK_WINDOW(dialog));
}
