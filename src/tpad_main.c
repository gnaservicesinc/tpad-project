/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014, 2015 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_main.c , is part of tpad.
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
 ******************************************************************************/
////////////////////////////////////////////////////////////////////////
#include "tpad_headers.h"
extern GtkSourceBuffer *mBuff;
extern gboolean save_locked;
extern GtkWidget *window;
extern gchar* origfile;
extern GtkSourceView *view;
extern int setModified;
extern int madetemp;
extern gboolean disable_file_watch;
extern int tpad_wach_thread_set;
extern int spell_state;
extern int run_count_update;
extern unsigned int gbid;
extern GtkSpellChecker* doc_spelling;
////////////////////////////////////////////////////////////////////////
GtkWidget *window=NULL;
GtkSourceView *view=NULL;
G_LOCK_DEFINE (mBuff);
GtkSourceBuffer *mBuff;
int spell_state=2;
GtkSpinButton *xSpinButton,*ySpinButton,*uSpinButton;
unsigned int gbid=0;
gchar *content;
gchar* origfile;
gboolean save_locked;
int tpad_wach_thread_set=0;
int setModified=0;
int madetemp=0;
int run_count_update=1;
gboolean disable_file_watch = FALSE;
gpointer current_data = NULL;

////////////////////////////////////////////////////////////////////////
/* TPAD_MAIN  */
////////////////////////////////////////////////////////////////////////
extern int tpad_main_impl(char* fchar)
{

	////////////////////////////////////////////////////////////////////////
	/* GTK INIT */
	////////////////////////////////////////////////////////////////////////
	/* tpad is built against GTK 3.  GTK 4 is not source-compatible with the
	 * widgets and main-loop APIs used by the application. */
	gtk_init(NULL,NULL);
	////////////////////////////////////////////////////////////////////////
	/* CONFIG Setup */
	////////////////////////////////////////////////////////////////////////
	config_setup();
	//[FIXME] Unity integration needed.
	#ifdef HAVE_LIBUNITY
	//ui_unity_init();
	#endif
	//////////////////////////////////////
	tpad_fp_init();
	gtk_widget_show_all(GTK_WIDGET(tpad_new_ui()));
	new_file();
	if (fchar != NULL && *fchar != '\0')
		show_file(fchar);
	if(cfg_spell()) toggle_spelling();
	printf("\nOpening file:\t%s.\ntpad License and Copyright Notice:\n\nCopyright (C) 2012-2026 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>\n\ntpad is free software: you can redistribute it and/or modify it\nunder the terms of the GNU General Public License as published by the\nFree Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\ntpad is distributed in the hope that it will be useful, but\nWITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\nSee the GNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License along\nwith this program.  If not, see <http://www.gnu.org/licenses/>.\n", fchar != NULL ? fchar : "");
	////////////////////////////////////////////////////////////////////////
	/* GTK MAIN */
	////////////////////////////////////////////////////////////////////////
    	gtk_main();
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	/* MAIN RETURN */
	////////////////////////////////////////////////////////////////////////
	return (0);
	////////////////////////////////////////////////////////////////////////
}
