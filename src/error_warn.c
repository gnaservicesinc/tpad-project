/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, error_warn.c , is part of tpad.
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
extern GtkWidget *window;

void gerror_log(gchar *c_msg){
		
		/* Disabled 
			gchar *error_msg;
		   	error_msg=g_strconcat(_ERROR_INFO,"\n",c_msg,NULL);
			openlog("tpad", LOG_CONS, LOG_SYSLOG);
			syslog(LOG_ERR, _ERROR_OCCURED, (char*)error_msg);
			closelog();
			g_free(error_msg);
		*/
	gchar *error_msg;
	error_msg=g_strconcat(_ERROR_INFO,"\n",c_msg,NULL);
	
	fprintf(stderr,"%s",error_msg);
	g_free(error_msg);
			
}
gint gerror_openguard_popup(gchar *c_fileName, gchar * foundFilePath) {
		GtkWidget* ogDialog;
		gint iret=0;
		ogDialog = gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,
		                                     GTK_BUTTONS_YES_NO,_FILE_MAY_BE_ALREADY_OPEN);
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(ogDialog),  _FILE_MAY_BE_ALREADY_OPEN_DETAIL, c_fileName,foundFilePath);	
	int result = (int) gtk_dialog_run(GTK_DIALOG(ogDialog));

	switch (result)
  {
	  case GTK_RESPONSE_YES:
		  iret=1;
		  break;
		  default:
		  iret=0;
		 break;
  }
		gtk_widget_destroy(GTK_WIDGET(ogDialog));
	return(iret);
}
void gerror_popup(gchar *c_msg) {
		GtkWidget* errorDialog;
		errorDialog = gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,
		                                     GTK_BUTTONS_OK,"ERROR!");
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(errorDialog),
		                                         _ERROR_MSG,
		                                         c_msg);
		gtk_dialog_run(GTK_DIALOG(errorDialog)); 
		gtk_widget_destroy(GTK_WIDGET(errorDialog));
}

void gerror_warn(gchar *c_msg, gchar *extra_information, gboolean slog, gboolean abort)
  {
	    gchar *msg;
	    msg=g_strconcat(c_msg,"\n",extra_information,NULL);
	    if(slog) gerror_log(msg);
	    gerror_popup(msg);
	    g_free(msg);
	    if(abort){
			new_thread_tpad(NULL);
			force_quit_program();
		}
}
