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
extern int spell_state;
extern gboolean spell_disabled;
extern GtkSpellChecker* doc_spelling;
extern GtkSourceView *view;
extern gboolean spelling_init;
extern GtkWidget *spelling;
gboolean spell_disabled = (gboolean) FALSE;
gboolean spelling_init  = (gboolean) FALSE;
GtkSpellChecker* doc_spelling;


int toggle_spelling(void) {
	GError *error = NULL;
	gboolean enabled;

	/* GtkSpell and every widget it touches must stay on GTK's main thread.
	 * The former worker was joined immediately, so it provided no concurrency. */
	if (!spelling_init && !spell_disabled) {
		doc_spelling = gtk_spell_checker_new();
		if (doc_spelling == NULL) {
			spell_disabled = TRUE;
			return 1;
		}
		g_object_ref_sink(doc_spelling);
		if (!gtk_spell_checker_set_language(doc_spelling, _SPELL_LANG,
		                                    &error)) {
			spell_disabled = TRUE;
			cfg_set_show_spelling(FALSE);
			cfg_save();
			gerror_warn(_SPELLING_FAILED_INIT,
			            error != NULL ? error->message : _SPELL_LANG,
			            TRUE, FALSE);
			g_clear_error(&error);
			g_clear_object(&doc_spelling);
			if (spelling != NULL)
				gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(spelling),
				                               FALSE);
			return 1;
		}
		spelling_init = TRUE;
	}

	if (spell_disabled || doc_spelling == NULL || view == NULL)
		return 1;

	enabled = spelling != NULL
	          ? gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(spelling))
	          : cfg_spell();
	cfg_set_show_spelling(enabled);
	cfg_save();

	if (enabled)
		gtk_spell_checker_attach(doc_spelling, GTK_TEXT_VIEW(view));
	else
		gtk_spell_checker_detach(doc_spelling);

	return 0;
}

void tpad_free_spelling(void) {
if (!spell_disabled && spelling_init && doc_spelling) {

	//If Spell Checking is enabled, detach it
	if(cfg_spell()) {
		if(doc_spelling) gtk_spell_checker_detach (doc_spelling);
	}	

	if(doc_spelling) {
		g_object_unref (doc_spelling);
		doc_spelling = NULL;
	}	
	//Do not permit further use of spelling system 
	spell_disabled = TRUE;	
	spelling_init = FALSE;
	
	
}
}

