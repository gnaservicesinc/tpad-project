/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014, 2015, 2016, 2017, 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_about.c , is part of tpad.
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
#include "tpad_gtk.h"

extern GtkSourceBuffer *mBuff;
extern GtkSourceView *view;
extern GtkWidget *window;

static GtkWidget *about_window;

void show_about(void)
{
	GtkWidget *dialog;
	GdkTexture *texture = NULL;
	GBytes *image_bytes;
	gchar *comments;
	gchar *config_id;

	if (about_window != NULL) {
		gtk_window_present(GTK_WINDOW(about_window));
		return;
	}

	config_id = tpad_id_get_id();
	comments = g_strdup_printf("%s\nConfig ID = %s\n", _ABT_COMMENT,
				   config_id != NULL ? config_id : "");
	dialog = gtk_about_dialog_new();
	about_window = dialog;
	g_object_add_weak_pointer(G_OBJECT(dialog), (gpointer *) &about_window);

	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Tpad");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), VERSION);
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), comments);
	gtk_about_dialog_set_copyright(
		GTK_ABOUT_DIALOG(dialog),
		"Copyright © 2012-2026 Andrew Smith "
		"(GNA SERVICES INC) <andresmi@icloud.com>");
	gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog),
					  GTK_LICENSE_GPL_3_0);
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),
				     "https://launchpad.net/tpad-project");
	gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog),
					   "tpad Project Homepage");

	image_bytes = g_bytes_new_static(tpad_inline + 24, 64 * 64 * 4);
	texture = gdk_memory_texture_new(64, 64, GDK_MEMORY_R8G8B8A8,
					 image_bytes, 256);
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog),
				  GDK_PAINTABLE(texture));
	g_clear_object(&texture);
	g_bytes_unref(image_bytes);

	gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(dialog),
						_TRANSLATOR_CREDITS);
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	if (window != NULL)
	{
		gtk_window_set_transient_for(GTK_WINDOW(dialog),
					     GTK_WINDOW(window));
		gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
	}
	gtk_window_present(GTK_WINDOW(dialog));

	g_free(comments);
	g_free(config_id);

}
