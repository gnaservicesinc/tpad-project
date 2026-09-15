/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014-2021 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_frequency_analysis.c , is part of tpad.
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
extern GtkWidget *window;

typedef struct {
	gunichar code_point;
	guint64 count;
} FrequencyRow;

static gint frequency_row_compare(gconstpointer left, gconstpointer right)
{
	const FrequencyRow *first = left;
	const FrequencyRow *second = right;

	if (first->count != second->count)
		return first->count < second->count ? 1 : -1;
	if (first->code_point == second->code_point)
		return 0;
	return first->code_point < second->code_point ? -1 : 1;
}

static GtkWidget *new_frequency_window(void)
{
	GtkWidget *analysis_window = gtk_window_new();

	gtk_window_set_title(GTK_WINDOW(analysis_window),
	                     gettext("Frequency Analysis"));
	gtk_window_set_modal(GTK_WINDOW(analysis_window), TRUE);
	if (window != NULL && GTK_IS_WINDOW(window)) {
		gtk_window_set_transient_for(GTK_WINDOW(analysis_window),
		                             GTK_WINDOW(window));
		gtk_window_set_destroy_with_parent(GTK_WINDOW(analysis_window), TRUE);
	}
	return analysis_window;
}

static void close_frequency_window(GtkButton *button, gpointer user_data)
{
	(void) button;
	gtk_window_destroy(GTK_WINDOW(user_data));
}

static GtkWidget *new_close_button(GtkWindow *analysis_window)
{
	GtkWidget *close_button = gtk_button_new_with_mnemonic(gettext("_Close"));

	gtk_widget_set_halign(close_button, GTK_ALIGN_END);
	g_signal_connect(close_button, "clicked",
	                 G_CALLBACK(close_frequency_window), analysis_window);
	return close_button;
}

static void show_empty_analysis(void)
{
	GtkWidget *analysis_window = new_frequency_window();
	GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
	GtkWidget *message = gtk_label_new(
		gettext("There are no non-whitespace characters to analyze."));

	gtk_label_set_wrap(GTK_LABEL(message), TRUE);
	gtk_widget_set_margin_start(content_box, 12);
	gtk_widget_set_margin_end(content_box, 12);
	gtk_widget_set_margin_top(content_box, 12);
	gtk_widget_set_margin_bottom(content_box, 12);
	gtk_box_append(GTK_BOX(content_box), message);
	gtk_box_append(GTK_BOX(content_box),
	               new_close_button(GTK_WINDOW(analysis_window)));
	gtk_window_set_child(GTK_WINDOW(analysis_window), content_box);
	gtk_window_present(GTK_WINDOW(analysis_window));
}

static void append_frequency_row(GString *table, const FrequencyRow *row,
	guint64 total)
{
	gchar character[7] = {0};

	if (g_unichar_isprint(row->code_point))
		(void) g_unichar_to_utf8(row->code_point, character);
	g_string_append_printf(table, "%s\tU+%04X\t%" G_GUINT64_FORMAT
	                       "\t%.2f%%\n", character, row->code_point,
	                       row->count,
	                       100.0 * (double)row->count / (double)total);
}

int tpad_frequency_analysis(void) {
	GtkTextIter start,end;
	gchar *temp_buffer;
	const gchar *cursor;
	GHashTable *counts;
	GHashTableIter hash_iter;
	gpointer hash_key;
	gpointer hash_value;
	GArray *rows;
	GString *table;
	guint64 total = 0;
	guint i;

	if (!gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(mBuff),
	                                          &start, &end))
		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(mBuff), &start, &end);

	/* This feature is read-only.  Beginning a user action here left the
	 * editor's undo group open indefinitely. */
	temp_buffer = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),
	                                       &start, &end, FALSE);

	counts = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
	for (cursor = temp_buffer; *cursor != '\0'; cursor = g_utf8_next_char(cursor)) {
		gunichar code_point = g_utf8_get_char(cursor);
		guint64 *count;

		if (code_point == ' ' || code_point == '\n' || code_point == '\r'
		    || code_point == '\t')
			continue;
		count = g_hash_table_lookup(counts, GUINT_TO_POINTER(code_point));
		if (count == NULL) {
			count = g_new0(guint64, 1);
			g_hash_table_insert(counts, GUINT_TO_POINTER(code_point), count);
		}
		(*count)++;
		total++;
	}
	g_free(temp_buffer);

	rows = g_array_sized_new(FALSE, FALSE, sizeof(FrequencyRow),
	                         g_hash_table_size(counts));
	g_hash_table_iter_init(&hash_iter, counts);
	while (g_hash_table_iter_next(&hash_iter, &hash_key, &hash_value)) {
		FrequencyRow row;

		row.code_point = GPOINTER_TO_UINT(hash_key);
		row.count = *((guint64 *)hash_value);
		g_array_append_val(rows, row);
	}
	g_hash_table_destroy(counts);

	if (rows->len == 0) {
		g_array_free(rows, TRUE);
		show_empty_analysis();
		return 0;
	}

	g_array_sort(rows, frequency_row_compare);
	table = g_string_new(NULL);
	g_string_append_printf(table, "%s\t%s\t%s\t%s\n",
	                       gettext("Character"), gettext("Code point"),
	                       gettext("Count"), gettext("Percent"));
	for (i = 0; i < rows->len; i++) {
		const FrequencyRow *row = &g_array_index(rows, FrequencyRow, i);

		append_frequency_row(table, row, total);
	}
	g_array_free(rows, TRUE);

	{
		GtkWidget *analysis_window;
		GtkWidget *content_box;
		GtkWidget *scrolled_window;
		GtkWidget *text_view;
		GtkTextBuffer *text_buffer;
		PangoTabArray *tabs;

		analysis_window = new_frequency_window();
		content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
		gtk_widget_set_margin_start(content_box, 12);
		gtk_widget_set_margin_end(content_box, 12);
		gtk_widget_set_margin_top(content_box, 12);
		gtk_widget_set_margin_bottom(content_box, 12);
		gtk_window_set_default_size(GTK_WINDOW(analysis_window), 650, 500);

		text_view = gtk_text_view_new();
		gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
		gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);
		gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view), TRUE);
		gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_NONE);
		tabs = pango_tab_array_new(3, TRUE);
		pango_tab_array_set_tab(tabs, 0, PANGO_TAB_LEFT, 120);
		pango_tab_array_set_tab(tabs, 1, PANGO_TAB_LEFT, 260);
		pango_tab_array_set_tab(tabs, 2, PANGO_TAB_LEFT, 400);
		gtk_text_view_set_tabs(GTK_TEXT_VIEW(text_view), tabs);
		pango_tab_array_free(tabs);
		text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
		gtk_text_buffer_set_text(text_buffer, table->str, (gint)table->len);
		g_string_free(table, TRUE);

		scrolled_window = gtk_scrolled_window_new();
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
		                               GTK_POLICY_AUTOMATIC,
		                               GTK_POLICY_AUTOMATIC);
		gtk_widget_set_hexpand(scrolled_window, TRUE);
		gtk_widget_set_vexpand(scrolled_window, TRUE);
		gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window),
		                              text_view);
		gtk_box_append(GTK_BOX(content_box), scrolled_window);
		gtk_box_append(GTK_BOX(content_box),
		               new_close_button(GTK_WINDOW(analysis_window)));
		gtk_window_set_child(GTK_WINDOW(analysis_window), content_box);
		gtk_window_present(GTK_WINDOW(analysis_window));
	}

	return 0;
}
