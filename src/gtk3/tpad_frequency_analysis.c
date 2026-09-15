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

enum {
	FA_COLUMN_CHARACTER,
	FA_COLUMN_CODE_POINT,
	FA_COLUMN_COUNT,
	FA_COLUMN_PERCENT,
	FA_COLUMN_TOTAL
};

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

static void append_frequency_column(GtkTreeView *tree_view,
	const gchar *title, gint model_column, gboolean align_right)
{
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	GtkTreeViewColumn *column;

	if (align_right)
		g_object_set(G_OBJECT(renderer), "xalign", 1.0, NULL);
	column = gtk_tree_view_column_new_with_attributes(title, renderer,
	                                                  "text", model_column,
	                                                  NULL);
	gtk_tree_view_column_set_alignment(column, align_right ? 1.0 : 0.0);
	gtk_tree_view_column_set_expand(column, TRUE);
	gtk_tree_view_column_set_min_width(column, 90);
	gtk_tree_view_append_column(tree_view, column);
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
		GtkWidget *message = gtk_message_dialog_new(GTK_WINDOW(window),
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
			"%s", gettext("There are no non-whitespace characters to analyze."));

		g_array_free(rows, TRUE);
		gtk_window_set_title(GTK_WINDOW(message), gettext("Frequency Analysis"));
		gtk_dialog_run(GTK_DIALOG(message));
		gtk_widget_destroy(message);
		return 0;
	}

	g_array_sort(rows, frequency_row_compare);

	{
		GtkWidget *dialog;
		GtkWidget *content_area;
		GtkWidget *scrolled_window;
		GtkWidget *tree_view;
		GtkListStore *store;
		GtkTreeIter iter;

		dialog = gtk_dialog_new_with_buttons(gettext("Frequency Analysis"),
			GTK_WINDOW(window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			gettext("_Close"), GTK_RESPONSE_CLOSE, NULL);
		gtk_window_set_default_size(GTK_WINDOW(dialog), 650, 500);
		gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ON_PARENT);
		gtk_window_set_resizable(GTK_WINDOW(dialog), TRUE);

		content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
		gtk_container_set_border_width(GTK_CONTAINER(content_area), 12);
		gtk_box_set_spacing(GTK_BOX(content_area), 12);

		store = gtk_list_store_new(FA_COLUMN_TOTAL, G_TYPE_STRING,
		                           G_TYPE_STRING, G_TYPE_UINT64, G_TYPE_STRING);
		for (i = 0; i < rows->len; i++) {
			const FrequencyRow *row = &g_array_index(rows, FrequencyRow, i);
			gchar character[7] = {0};
			gchar code_point[10];
			gchar percent[32];

			g_unichar_to_utf8(row->code_point, character);
			g_snprintf(code_point, sizeof(code_point), "U+%04X",
			           row->code_point);
			g_snprintf(percent, sizeof(percent), "%.2f%%",
			           100.0 * (double)row->count / (double)total);
			gtk_list_store_append(store, &iter);
			gtk_list_store_set(store, &iter,
			                   FA_COLUMN_CHARACTER, character,
			                   FA_COLUMN_CODE_POINT, code_point,
			                   FA_COLUMN_COUNT, row->count,
			                   FA_COLUMN_PERCENT, percent, -1);
		}
		g_array_free(rows, TRUE);

		tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
		g_object_unref(store);
		gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tree_view),
		                             GTK_TREE_VIEW_GRID_LINES_BOTH);
		gtk_tree_view_set_headers_clickable(GTK_TREE_VIEW(tree_view), FALSE);
		append_frequency_column(GTK_TREE_VIEW(tree_view), gettext("Character"),
		                        FA_COLUMN_CHARACTER, FALSE);
		append_frequency_column(GTK_TREE_VIEW(tree_view), gettext("Code point"),
		                        FA_COLUMN_CODE_POINT, FALSE);
		append_frequency_column(GTK_TREE_VIEW(tree_view), gettext("Count"),
		                        FA_COLUMN_COUNT, TRUE);
		append_frequency_column(GTK_TREE_VIEW(tree_view), gettext("Percent"),
		                        FA_COLUMN_PERCENT, TRUE);

		scrolled_window = gtk_scrolled_window_new(NULL, NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
		                               GTK_POLICY_AUTOMATIC,
		                               GTK_POLICY_AUTOMATIC);
		gtk_widget_set_hexpand(scrolled_window, TRUE);
		gtk_widget_set_vexpand(scrolled_window, TRUE);
		gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
		gtk_box_pack_start(GTK_BOX(content_area), scrolled_window,
		                   TRUE, TRUE, 0);

		gtk_widget_show_all(dialog);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	}

	return 0;
}
