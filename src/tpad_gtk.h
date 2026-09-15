/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2026 Andrew Smith (GNA SERVICES INC) <andresmi@icloud.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_gtk.h, is part of tpad.
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

#ifndef TPAD_GTK_H
#define TPAD_GTK_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

int tpad_alert_choose(GtkWindow *parent, const gchar *message,
			      const gchar *detail,
			      const gchar *const *buttons,
			      int default_button, int cancel_button);
void tpad_alert_show(GtkWindow *parent, const gchar *message,
			     const gchar *detail);
gchar *tpad_clipboard_read_primary_text(void);
void tpad_clipboard_set_text(const gchar *text);
void tpad_clipboards_store(void);
PangoFontDescription *tpad_choose_font(
	GtkWindow *parent, const gchar *title,
	const PangoFontDescription *initial_value);
GFile *tpad_file_dialog_open(GtkWindow *parent, const gchar *title,
			     const gchar *initial_path);
GListModel *tpad_file_dialog_open_multiple(GtkWindow *parent,
					   const gchar *title);
GFile *tpad_file_dialog_save(GtkWindow *parent, const gchar *title,
			     const gchar *initial_path,
			     const gchar *initial_name);

G_END_DECLS

#endif
