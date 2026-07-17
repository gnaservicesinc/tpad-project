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

extern GtkSourceBuffer *mBuff;
extern GtkSourceView *view;
extern GtkWidget *window;

void show_about(void){
    GtkWidget *dialog;
	GdkPixbuf *logo;
	gchar *comments;
    dialog=gtk_about_dialog_new();
	gchar* csCfg_id= (gchar*) NULL;
 csCfg_id= (gchar*) tpad_id_get_id();//cfg_id();
	comments = g_strdup_printf("%s\nConfig ID = %s\n", _ABT_COMMENT,
	                           csCfg_id);

    gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG(dialog),(const gchar *)"Tpad");
    gtk_about_dialog_set_version (GTK_ABOUT_DIALOG(dialog),(const gchar *)VERSION);
    gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG(dialog), comments);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),
		(const gchar *) "Copyright © 2012-2026 Andrew Smith "
		"(GNA SERVICES INC) <andresmi@icloud.com>");
    gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog),GTK_LICENSE_GPL_3_0);
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),(const gchar *)"https://launchpad.net/tpad-project");
	gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog),(const gchar *)"tpad Project Homepage");

	logo = gdk_pixbuf_new_from_data(tpad_inline + 24, GDK_COLORSPACE_RGB,
	                                TRUE, 8, 64, 64, 256, NULL, NULL);
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), logo);
	if (logo != NULL)
		g_object_unref(logo);
	g_free(comments);
	gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(dialog), _TRANSLATOR_CREDITS);
	gtk_window_set_position(GTK_WINDOW(GTK_ABOUT_DIALOG(dialog)),GTK_WIN_POS_CENTER);

	gtk_window_set_modal(GTK_WINDOW(dialog),TRUE);

	gtk_window_set_transient_for(GTK_WINDOW(dialog),GTK_WINDOW(window));

	 gtk_dialog_run(GTK_DIALOG(GTK_ABOUT_DIALOG(dialog)));
		g_free(csCfg_id);
	gtk_widget_destroy(GTK_WIDGET(dialog));

}
