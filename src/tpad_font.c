/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014-2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_font.c , is part of tpad.
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

//extern GtkSourceBuffer *mBuff;
extern GtkSourceView *view;

void select_font(){
    gint response;
    GtkWidget *dialog = gtk_font_chooser_dialog_new(_FONT,NULL);
    gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_CENTER);

    response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_OK || response == GTK_RESPONSE_APPLY) {
        PangoFontDescription *font_desc;
        gchar *fontname = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));
	tpad_set_font(fontname);
        g_free(fontname);
    }
    gtk_widget_destroy(dialog);
}

void tpad_set_font( gchar *fontname) {
        PangoFontDescription *font_desc;
        font_desc = pango_font_description_from_string(fontname);

        // gtk_widget_override_font is deprecated in newer GTK versions
        #if GTK_CHECK_VERSION(3,16,0)
            // Use CSS provider for GTK 3.16+
            GtkCssProvider *provider = gtk_css_provider_new();
            gchar *css = g_strdup_printf("textview { font: %s; }", fontname);
            gtk_css_provider_load_from_data(provider, css, -1, NULL);
            gtk_style_context_add_provider(gtk_widget_get_style_context(GTK_WIDGET(view)),
                                         GTK_STYLE_PROVIDER(provider),
                                         GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
            g_free(css);
            g_object_unref(provider);
        #else
            // Use deprecated function for older GTK
            gtk_widget_override_font(GTK_WIDGET(view), font_desc);
        #endif

        pango_font_description_free(font_desc);
}
