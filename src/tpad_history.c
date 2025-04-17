/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014, 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_history.c , is part of tpad.
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

void do_undo(){
   if(gtk_source_buffer_can_undo(GTK_SOURCE_BUFFER(mBuff))) gtk_source_buffer_undo(GTK_SOURCE_BUFFER(mBuff));
}


void do_redo(){
  if(gtk_source_buffer_can_redo(GTK_SOURCE_BUFFER(mBuff)))  gtk_source_buffer_redo(GTK_SOURCE_BUFFER(mBuff));
}

void on_past (GtkWidget *caller){
	(cfg_line_wrap()) ? gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view),GTK_WRAP_WORD) : gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view),GTK_WRAP_NONE);

	
}