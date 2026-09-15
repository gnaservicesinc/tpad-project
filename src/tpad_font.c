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
#include "tpad_gtk.h"

extern GtkWidget *window;
extern GtkSourceView *view;

typedef struct {
	GtkCssProvider *provider;
	GdkDisplay *display;
	PangoFontDescription *description;
} TpadFontStyle;

static const gchar font_style_key[] = "tpad-font-style";

static void tpad_font_style_free(gpointer data)
{
	TpadFontStyle *style = data;

	gtk_style_context_remove_provider_for_display(
		style->display, GTK_STYLE_PROVIDER(style->provider));
	g_clear_object(&style->provider);
	g_clear_object(&style->display);
	g_clear_pointer(&style->description, pango_font_description_free);
	g_free(style);
}

static gchar *tpad_css_quote(const gchar *text)
{
	const guchar *cursor = (const guchar *) text;
	GString *quoted = g_string_new("\"");

	while (*cursor != '\0') {
		if (*cursor == '\\' || *cursor == '"')
			g_string_append_c(quoted, '\\');
		if (*cursor < 0x20 || *cursor == 0x7f)
			g_string_append_c(quoted, ' ');
		else
			g_string_append_c(quoted, (gchar) *cursor);
		cursor++;
	}
	g_string_append_c(quoted, '"');
	return g_string_free(quoted, FALSE);
}

static const gchar *tpad_font_style_name(PangoStyle style)
{
	switch (style) {
	case PANGO_STYLE_OBLIQUE:
		return "oblique";
	case PANGO_STYLE_ITALIC:
		return "italic";
	case PANGO_STYLE_NORMAL:
	default:
		return "normal";
	}
}

static const gchar *tpad_font_stretch_name(PangoStretch stretch)
{
	switch (stretch) {
	case PANGO_STRETCH_ULTRA_CONDENSED:
		return "ultra-condensed";
	case PANGO_STRETCH_EXTRA_CONDENSED:
		return "extra-condensed";
	case PANGO_STRETCH_CONDENSED:
		return "condensed";
	case PANGO_STRETCH_SEMI_CONDENSED:
		return "semi-condensed";
	case PANGO_STRETCH_SEMI_EXPANDED:
		return "semi-expanded";
	case PANGO_STRETCH_EXPANDED:
		return "expanded";
	case PANGO_STRETCH_EXTRA_EXPANDED:
		return "extra-expanded";
	case PANGO_STRETCH_ULTRA_EXPANDED:
		return "ultra-expanded";
	case PANGO_STRETCH_NORMAL:
	default:
		return "normal";
	}
}

static TpadFontStyle *tpad_font_style_get(void)
{
	TpadFontStyle *style;
	GtkWidget *widget;

	if (view == NULL)
		return NULL;
	widget = GTK_WIDGET(view);
	style = g_object_get_data(G_OBJECT(widget), font_style_key);
	if (style != NULL)
		return style;

	style = g_new0(TpadFontStyle, 1);
	style->provider = gtk_css_provider_new();
	style->display = g_object_ref(gtk_widget_get_display(widget));
	gtk_style_context_add_provider_for_display(
		style->display, GTK_STYLE_PROVIDER(style->provider),
		GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	gtk_widget_add_css_class(widget, "tpad-editor-font");
	g_object_set_data_full(G_OBJECT(widget), font_style_key, style,
			       tpad_font_style_free);
	return style;
}

void select_font(void)
{
	TpadFontStyle *style;
	PangoFontDescription *selected;
	gchar *fontname;

	style = tpad_font_style_get();
	selected = tpad_choose_font(
		window != NULL ? GTK_WINDOW(window) : NULL, _FONT,
		style != NULL ? style->description : NULL);
	if (selected == NULL)
		return;

	fontname = pango_font_description_to_string(selected);
	tpad_set_font(fontname);
	g_free(fontname);
	pango_font_description_free(selected);
}

void tpad_set_font(const gchar *fontname)
{
	TpadFontStyle *style;
	PangoFontDescription *font_desc;
	const gchar *family;
	gchar *quoted_family;
	gchar size_buffer[G_ASCII_DTOSTR_BUF_SIZE];
	gchar *css;
	double size;
	const gchar *unit;

	if (fontname == NULL || view == NULL)
		return;

	font_desc = pango_font_description_from_string(fontname);
	style = tpad_font_style_get();
	if (font_desc == NULL || style == NULL) {
		pango_font_description_free(font_desc);
		return;
	}

	family = pango_font_description_get_family(font_desc);
	quoted_family = tpad_css_quote(family != NULL ? family : "monospace");
	size = (double) pango_font_description_get_size(font_desc) / PANGO_SCALE;
	if (size <= 0.0)
		size = 12.0;
	unit = pango_font_description_get_size_is_absolute(font_desc)
	       ? "px" : "pt";
	g_ascii_formatd(size_buffer, sizeof(size_buffer), "%.3f", size);
	css = g_strdup_printf(
		".tpad-editor-font {"
		" font-family: %s;"
		" font-size: %s%s;"
		" font-style: %s;"
		" font-weight: %d;"
		" font-stretch: %s;"
		" }",
		quoted_family, size_buffer, unit,
		tpad_font_style_name(pango_font_description_get_style(font_desc)),
		(int) pango_font_description_get_weight(font_desc),
		tpad_font_stretch_name(
			pango_font_description_get_stretch(font_desc)));

#if GTK_CHECK_VERSION(4, 12, 0)
	gtk_css_provider_load_from_string(style->provider, css);
#else
	gtk_css_provider_load_from_data(style->provider, css, -1);
#endif
	g_clear_pointer(&style->description, pango_font_description_free);
	style->description = pango_font_description_copy(font_desc);

	g_free(css);
	g_free(quoted_family);
	pango_font_description_free(font_desc);
}
