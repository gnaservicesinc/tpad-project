/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_textstats.c , is part of tpad.
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

static void attach_stat_row(GtkGrid *grid, gint row, const gchar *heading,
	const gchar *value)
{
	GtkWidget *heading_label = gtk_label_new(heading);
	GtkWidget *value_label = gtk_label_new(value);

	gtk_label_set_xalign(GTK_LABEL(heading_label), 0.0f);
	gtk_label_set_xalign(GTK_LABEL(value_label), 1.0f);
	gtk_widget_set_hexpand(heading_label, TRUE);
	gtk_grid_attach(grid, heading_label, 0, row, 1, 1);
	gtk_grid_attach(grid, value_label, 1, row, 1, 1);
}

void get_text_stats(GtkWidget *caller){
	(void) caller;
//	ui_unity_set_count();
	char buf_line_count[G_ASCII_DTOSTR_BUF_SIZE];
	char buf_char_count[G_ASCII_DTOSTR_BUF_SIZE];
	char buf_word_count[G_ASCII_DTOSTR_BUF_SIZE];
	gchar *cline_count=NULL;
	gchar *cchar_count=NULL;
	gchar *cword_count=NULL;

	cword_count=g_ascii_dtostr(buf_word_count,
	                           sizeof(buf_word_count),
	                           gtk_text_buffer_get_word_count(GTK_TEXT_BUFFER(mBuff)) );
	
	cchar_count=g_ascii_dtostr(buf_char_count,
	                           sizeof(buf_char_count),
	                           gtk_text_buffer_get_char_count (GTK_TEXT_BUFFER(mBuff)) );
	
	cline_count=g_ascii_dtostr(buf_line_count,
	                           sizeof(buf_line_count),
	                           gtk_text_buffer_get_line_count (GTK_TEXT_BUFFER(mBuff)) );
	GtkWidget *dialog = gtk_window_new();
	GtkWidget *grid = gtk_grid_new();

	gtk_window_set_title(GTK_WINDOW(dialog),_WORD_COUNT);
	gtk_window_set_resizable(GTK_WINDOW(dialog),FALSE);
	if (window != NULL && GTK_IS_WINDOW(window)) {
		gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
		gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
	}
	gtk_grid_set_column_spacing(GTK_GRID(grid), 18);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
	gtk_widget_set_margin_start(grid, 12);
	gtk_widget_set_margin_end(grid, 12);
	gtk_widget_set_margin_top(grid, 12);
	gtk_widget_set_margin_bottom(grid, 12);
	attach_stat_row(GTK_GRID(grid), 0, _WORD_COUNT_HEADING, cword_count);
	attach_stat_row(GTK_GRID(grid), 1, _CHAR_COUNT_HEADING, cchar_count);
	attach_stat_row(GTK_GRID(grid), 2, _LINE_COUNT_HEADING, cline_count);
	gtk_window_set_child(GTK_WINDOW(dialog),grid);
	gtk_window_present(GTK_WINDOW(dialog));
}
