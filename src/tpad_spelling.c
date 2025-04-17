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
pthread_mutex_t spellingmutex = PTHREAD_MUTEX_INITIALIZER;
extern int spell_state;
extern gboolean spell_disabled;
extern GtkSpellChecker* doc_spelling;
extern GtkSourceView *view;
extern gboolean spelling_init;
extern GtkWidget *spelling;
gboolean spell_disabled = (gboolean) FALSE;
gboolean spelling_init  = (gboolean) FALSE;
GtkSpellChecker* doc_spelling;


int toggle_spelling () {
	int rc;
	pthread_t thread;
	if( (rc=pthread_create( &thread, NULL, ttogle_spelling, NULL)) ){
    	return(1);
	}
	else {
	 pthread_join( thread, NULL);
	 return(0);
	}
}


void *ttogle_spelling(){
if (pthread_mutex_lock( &spellingmutex ) == 0) {
if(!spelling_init && !spell_disabled) {
	spelling_init  = (gboolean) TRUE;
	GError *errSP = NULL; 
	doc_spelling = gtk_spell_checker_new ();
	g_object_ref_sink (doc_spelling);
	if (! gtk_spell_checker_set_language (doc_spelling, _SPELL_LANG, &errSP)) {
		if (errSP != NULL){
			spell_disabled=TRUE;
			cfg_set_show_spelling(FALSE);
			cfg_save();
			gerror_warn(_SPELLING_FAILED_INIT,errSP->message, (gboolean) TRUE, (gboolean) FALSE); 
		}	
	}
	gtk_spell_checker_attach(doc_spelling, GTK_TEXT_VIEW (view));
	gtk_spell_checker_detach (doc_spelling);
	}

if (!spell_disabled) {
	cfg_set_show_spelling((int)gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(spelling)));
	cfg_save();
	
	if(doc_spelling && view && !spell_disabled)
		(cfg_spell()) ? gtk_spell_checker_attach(doc_spelling, GTK_TEXT_VIEW (view)) : gtk_spell_checker_detach (doc_spelling);
	}
pthread_mutex_unlock( &spellingmutex );
	}
pthread_exit ( (void*) NULL);	
}

void tpad_free_spelling() {
if (!spell_disabled && spelling_init && doc_spelling) {

	//If Spell Checking is enabled, detach it
	if(cfg_spell()) {
		if(doc_spelling) gtk_spell_checker_detach (doc_spelling);
	}	

	if(doc_spelling) {
		g_object_unref (doc_spelling);
	}	
	//Do not permit further use of spelling system 
	spell_disabled = TRUE;	
	
	
}
}


 