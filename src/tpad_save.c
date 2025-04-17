/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_save.c , is part of tpad.
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
extern gboolean save_locked;
extern GtkWidget *window;
extern gchar* origfile;
extern int madetemp;
extern int tpad_wach_thread_set;
extern gboolean disable_file_watch;
extern short iforce_save;
 
gint save_watched(){
return(-1);


}

gint tpad_save_ondisk_change_detected() {

	// returns 1 if file save should be forced

	gint response=0;
	GtkWidget *dialog;

		dialog=gtk_message_dialog_new(GTK_WINDOW(window),
							GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_QUESTION,
							GTK_BUTTONS_NONE,
							_ON_DISK_CHANGES);
		gtk_dialog_add_button(GTK_DIALOG(dialog),_RELOAD_AND_DROP_UNSAVED_CHANGES,2);
        	gtk_dialog_add_button(GTK_DIALOG(dialog),_SAVE_FILE_ANYWAY,1);
        	gtk_dialog_add_button(GTK_DIALOG(dialog),_OPEN_FILE_SAVE_AS_INSTEAD,0);
        	gtk_dialog_add_button(GTK_DIALOG(dialog),_CANCEL,-1);
		response=gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);



		switch (response)
			{

			
			case 0:
				save_as();

			break;
			case 2:
				new_thread_tpad(tpad_fp_get_current());
				force_quit_program();
			break;		
			default:
				return(-1);
			break;

			}
	
	return(response);
}
gint save_modified(){
        gint response=0;
	GtkWidget *dialog;	
	gint isw = 0;


	if(gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(mBuff))==TRUE) 
	{
		
       	 	dialog= (GtkWidget *) gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,GTK_BUTTONS_NONE,_SAVE_CHANGES);
	
        	gtk_dialog_add_button(GTK_DIALOG(dialog),_YES,1);
        	gtk_dialog_add_button(GTK_DIALOG(dialog),_NO,0);
        	gtk_dialog_add_button(GTK_DIALOG(dialog),_CANCEL,-1);
	
        	response=gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);

		if(response == 0) return(1);

        	if(response == 1) save_file();
	
		if(response == -1) return(0);

		return(1);
	}
  return(1);
}
void save_file() {

short iabort_save = 0;

if(tpad_fp_get_current() == NULL) {
	struct passwd *pww = getpwuid(syscall(__NR_getuid));
	tpad_fp_set( (void*) g_strconcat((gchar *)pww->pw_dir,"/",(const gchar *)_FALLBACK_SAVE_FILE_NAME,NULL));
	save_as();
			
	}
else  {

	if ( tpad_control_compare_stored_file_hash_to_current_ondisk_file_hash() != 0 ){
		if( tpad_save_ondisk_change_detected() != 1) iabort_save = 1;
	}


	if (!iabort_save) {

		FILE *fp=NULL;
		if(madetemp==1)  fp=fopen(origfile, "w");
		else fp=fopen(tpad_fp_get_current(), "w");
		if(fp){

			
				GtkTextIter start,end;
				gtk_text_buffer_get_bounds( GTK_TEXT_BUFFER(mBuff),&start,&end);
				char* data_to_convert = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),&start,&end,FALSE);
				size_t bsize = strlen(data_to_convert);
	
if( cfg_ut8bom() && tpad_string_has_bom8(&data_to_convert) == 0 ) {
		
		fprintf(fp,"%c%c%c%s",239, 187, 191,tpad_codeset_convert_to_curent_local_from_utf8(data_to_convert,bsize));
}
else{			fprintf(fp,"%s",tpad_codeset_convert_to_curent_local_from_utf8(data_to_convert,bsize));
}	
			fclose(fp);
				gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(mBuff),FALSE);
				set_title();
				set_language();
				tpad_control_store_hash_of_current_file_set();
			}
			
		else {
			struct passwd *pww = getpwuid(syscall(__NR_getuid));
			tpad_fp_set( (void*) g_strconcat((gchar *)pww->pw_dir,"/",(const gchar *)_FALLBACK_SAVE_FILE_NAME,NULL));
			save_as();
			}
		}
		
	} 
	
}

	

void save_as(){
    GtkWidget *dialog;


    dialog = gtk_file_chooser_dialog_new(_SAVE_FILE,GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         _CANCEL,
                                         GTK_RESPONSE_CANCEL,
                                         _SAVE_TOOLBAR,
                                         GTK_RESPONSE_ACCEPT,NULL);
    gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER(dialog),TRUE);
    gtk_file_chooser_set_show_hidden (GTK_FILE_CHOOSER(dialog),TRUE);
    gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER(dialog),TRUE);
	if(tpad_fp_get_current()){

		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), (gchar*)get_currentfile_basename());
        gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(dialog),(const gchar *)tpad_fp_get_current());
		
	}
	else 
	{
			
		
	    if (getcRpath() != NULL) {
	gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(dialog),getcRpath());
	
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),(const gchar *)getcRpath());
		}
		else {
					 struct passwd *pw = getpwuid(syscall(__NR_getuid));
				gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(dialog),g_strconcat((gchar *)pw->pw_dir,"/",(const gchar *)_FALLBACK_SAVE_FILE_NAME,NULL));
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),(const gchar *)_FALLBACK_SAVE_FILE_NAME);
		}
	}
	
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
		tpad_fp_set( (void*) gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
        save_file();
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}
