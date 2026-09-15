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
#include "tpad_gtk.h"
extern GtkWidget *window;

void gerror_log(const gchar *c_msg){
		
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
gint gerror_openguard_popup(gchar *c_fileName, gchar *foundFilePath)
{
	const gchar *buttons[] = { _NO, _YES, NULL };
	gchar *detail;
	int response;

	detail = g_strdup_printf(_FILE_MAY_BE_ALREADY_OPEN_DETAIL,
				 c_fileName, foundFilePath);
	response = tpad_alert_choose(
		window != NULL ? GTK_WINDOW(window) : NULL,
		_FILE_MAY_BE_ALREADY_OPEN, detail, buttons, 1, 0);
	g_free(detail);
	return response == 1 ? 1 : 0;
}

void gerror_popup(const gchar *c_msg)
{
	const gchar *buttons[] = { gettext("OK"), NULL };
	gchar *detail;

	detail = g_strdup_printf(_ERROR_MSG, c_msg != NULL ? c_msg : "");
	(void) tpad_alert_choose(window != NULL ? GTK_WINDOW(window) : NULL,
				 "ERROR!", detail, buttons, 0, 0);
	g_free(detail);
}

void gerror_warn(const gchar *c_msg, const gchar *extra_information,
                 gboolean slog, gboolean abort)
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
