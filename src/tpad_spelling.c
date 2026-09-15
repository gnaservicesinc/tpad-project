/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014, 2015, 2016, 2017 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_spelling.c , is part of tpad.
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

static SpellingChecker *spell_checker;
static SpellingTextBufferAdapter *spell_adapter;

static gboolean initialize_spelling(void)
{
	GMenuModel *menu;

	if (spell_adapter != NULL)
		return TRUE;
	if (mBuff == NULL || view == NULL)
		return FALSE;

	spelling_init();
	spell_checker = spelling_checker_new(NULL, _SPELL_LANG);
	if (spell_checker == NULL ||
	    spelling_checker_get_language(spell_checker) == NULL)
		goto failed;

	spell_adapter = spelling_text_buffer_adapter_new(mBuff, spell_checker);
	if (spell_adapter == NULL)
		goto failed;

	menu = spelling_text_buffer_adapter_get_menu_model(spell_adapter);
	gtk_text_view_set_extra_menu(GTK_TEXT_VIEW(view), menu);
	gtk_widget_insert_action_group(GTK_WIDGET(view), "spelling",
	                               G_ACTION_GROUP(spell_adapter));
	return TRUE;

failed:
	g_clear_object(&spell_adapter);
	g_clear_object(&spell_checker);
	return FALSE;
}

int tpad_set_spelling(gboolean enabled)
{
	if (!enabled) {
		if (spell_adapter != NULL)
			spelling_text_buffer_adapter_set_enabled(spell_adapter, FALSE);
		cfg_set_show_spelling(FALSE);
		cfg_save();
		return 0;
	}

	if (!initialize_spelling()) {
		cfg_set_show_spelling(FALSE);
		cfg_save();
		gerror_warn(_SPELLING_FAILED_INIT, _SPELL_LANG, TRUE, FALSE);
		return 1;
	}

	spelling_text_buffer_adapter_set_enabled(spell_adapter, TRUE);
	cfg_set_show_spelling(TRUE);
	cfg_save();
	return 0;
}

void tpad_free_spelling(void)
{
	if (view != NULL) {
		gtk_text_view_set_extra_menu(GTK_TEXT_VIEW(view), NULL);
		gtk_widget_insert_action_group(GTK_WIDGET(view), "spelling", NULL);
	}
	if (spell_adapter != NULL)
		spelling_text_buffer_adapter_set_enabled(spell_adapter, FALSE);
	g_clear_object(&spell_adapter);
	g_clear_object(&spell_checker);
}
