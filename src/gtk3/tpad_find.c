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
	text = gtk_entry_get_text(GTK_ENTRY(findentry));
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

static gboolean find_dialog_key_pressed(GtkWidget *dialog,
	GdkEventKey *event, gpointer user_data)
{
	(void) user_data;
	if (event->keyval != GDK_KEY_Escape)
		return FALSE;

	gtk_widget_destroy(dialog);
	return TRUE;
}

static void find_dialog_destroyed(GtkWidget *dialog, gpointer user_data)
{
	(void) dialog;
	(void) user_data;
	find_dialog_window = NULL;
	findentry = NULL;
	replaceentry = NULL;
	find_match_found = FALSE;
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
		replacement = g_strcompress(gtk_entry_get_text(GTK_ENTRY(replaceentry)));
		replacement_text = replacement;
	} else {
		replacement_text = gtk_entry_get_text(GTK_ENTRY(replaceentry));
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
	if (find_dialog_window != NULL) {
		gtk_window_present(GTK_WINDOW(find_dialog_window));
		return;
	}

	sdone=0;
    GtkWidget *dialog;
    GtkWidget *mainbox,*boxbutton,*findbox,*replacebox,*vseparator[3];
	GtkWidget *buttonfind,*buttonreplace,*buttonfindreplace;
	GtkWidget *casebutton;
	GtkWidget *convtbutton;

	    dialog=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	find_dialog_window = dialog;
	g_signal_connect(dialog, "destroy", G_CALLBACK(find_dialog_destroyed), NULL);
	g_signal_connect(dialog, "key-press-event",
	                 G_CALLBACK(find_dialog_key_pressed), NULL);
	vseparator[0]=gtk_separator_new(GTK_ORIENTATION_VERTICAL);
	vseparator[1]=gtk_separator_new(GTK_ORIENTATION_VERTICAL);
	vseparator[2]=gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    gtk_window_set_title(GTK_WINDOW(dialog),_FIND_FIND_AND_REPLACE);
    gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW(dialog),FALSE);

    mainbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    findbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    replacebox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    boxbutton=gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);

    gtk_container_add(GTK_CONTAINER(dialog),mainbox);
    gtk_container_add(GTK_CONTAINER(mainbox),findbox);
    gtk_container_add(GTK_CONTAINER(mainbox),replacebox);
    gtk_container_add(GTK_CONTAINER(mainbox),boxbutton);
//GtkClipboard *S_Clip;
    gtk_box_pack_start (GTK_BOX (findbox),
                        GTK_WIDGET (gtk_label_new(_SEARCH_FOR)), TRUE, TRUE, 1);

	//S_Clip=gtk_clipboard_get(GDK_SELECTION_PRIMARY);
		gchar *recived;
		gchar *escaped = NULL;
		recived=gtk_clipboard_wait_for_text(GTK_CLIPBOARD(gtk_clipboard_get(GDK_SELECTION_PRIMARY)));
		findentry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(findentry),65536);
	gtk_entry_set_input_purpose (GTK_ENTRY(findentry),GTK_INPUT_PURPOSE_FREE_FORM);
		if(recived) {
			escaped = g_strescape(recived, NULL);
			gtk_entry_set_text(GTK_ENTRY(findentry),
			                   escaped != NULL ? escaped : recived);
		}
		g_free(escaped);
		g_free(recived);

	gtk_box_pack_start (GTK_BOX (findbox),
                        GTK_WIDGET (findentry), TRUE, TRUE, 1);
	gtk_box_pack_start (GTK_BOX (findbox),
	                    GTK_WIDGET(vseparator[0]),TRUE,TRUE,1);

    gtk_box_pack_start (GTK_BOX (replacebox),
                        GTK_WIDGET (gtk_label_new(_REPLACE_WITH)), TRUE, TRUE, 1);

    replaceentry = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(replaceentry),65536);
	gtk_entry_set_input_purpose (GTK_ENTRY(replaceentry),GTK_INPUT_PURPOSE_FREE_FORM);
    gtk_box_pack_start (GTK_BOX (replacebox),
                        GTK_WIDGET (replaceentry), TRUE, TRUE, 1);
	gtk_box_pack_start (GTK_BOX (replacebox),
	                    GTK_WIDGET(vseparator[1]),TRUE,TRUE,1);

    buttonfind=gtk_button_new_with_label(_FIND);
    gtk_box_pack_start (GTK_BOX (boxbutton),
                        GTK_WIDGET (buttonfind), TRUE, TRUE, 1);

    buttonreplace=gtk_button_new_with_label(_REPLACE);
    gtk_box_pack_start (GTK_BOX (boxbutton),
                        GTK_WIDGET (buttonreplace), TRUE, TRUE, 1);

	gtk_box_pack_start (GTK_BOX (boxbutton),
	                    GTK_WIDGET(vseparator[2]),TRUE,TRUE,1);

    buttonfindreplace=gtk_button_new_with_label(_FIND_AND_REPLACE);
    gtk_box_pack_start (GTK_BOX (boxbutton),
                        GTK_WIDGET (buttonfindreplace), TRUE, TRUE, 1);

	casebutton = gtk_check_button_new_with_label(_MATCH_CASE);
	gtk_box_pack_start(GTK_BOX(boxbutton), casebutton, TRUE, TRUE, 1);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(casebutton), searchCase);
	g_signal_connect(casebutton, "toggled",
	                 G_CALLBACK(toggle_case_sarch), NULL);

	convtbutton = gtk_check_button_new_with_label((const gchar*)"COVT");
	 gtk_box_pack_start (GTK_BOX (boxbutton),
                        GTK_WIDGET (convtbutton), TRUE, TRUE, 1);
	doCOVT=TRUE;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(convtbutton),doCOVT);
	g_signal_connect(convtbutton, "clicked",G_CALLBACK(toggle_covt),NULL );


    g_signal_connect(buttonfind,"clicked",G_CALLBACK(do_find), GTK_WINDOW(dialog));
    g_signal_connect(buttonreplace,"clicked",G_CALLBACK(do_replace),GTK_WINDOW(dialog));
    g_signal_connect(buttonfindreplace,"clicked",G_CALLBACK(do_find_replace),GTK_WINDOW(dialog));
    gtk_widget_show_all(dialog);
}
