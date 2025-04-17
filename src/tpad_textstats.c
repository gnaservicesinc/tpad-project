/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_textstats.c , is part of tpad.
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
void get_text_stats(GtkWidget *caller){
//	ui_unity_set_count();
	char buf_line_count[G_ASCII_DTOSTR_BUF_SIZE];
	char buf_char_count[G_ASCII_DTOSTR_BUF_SIZE];
	char buf_word_count[G_ASCII_DTOSTR_BUF_SIZE];
	gchar *cline_count=NULL;
	gchar *cchar_count=NULL;
	gchar *cword_count=NULL;

	cword_count=g_ascii_dtostr(buf_word_count,
	                           sizeof(buf_word_count),
	                           gtk_text_buffer_get_word_count(GTK_TEXT_BUFFER(mBuff)) );
	
	cchar_count=g_ascii_dtostr(buf_char_count,
	                           sizeof(buf_char_count),
	                           gtk_text_buffer_get_char_count (GTK_TEXT_BUFFER(mBuff)) );
	
	cline_count=g_ascii_dtostr(buf_line_count,
	                           sizeof(buf_line_count),
	                           gtk_text_buffer_get_line_count (GTK_TEXT_BUFFER(mBuff)) );
	    GtkWidget *dialog;
    GtkWidget *mainbox,*wordCountbox,*charCountbox,*lineCountbox,*vseparator[4];
	
    dialog=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	vseparator[0]=gtk_separator_new(GTK_ORIENTATION_VERTICAL);
	vseparator[1]=gtk_separator_new(GTK_ORIENTATION_VERTICAL);
	vseparator[2]=gtk_separator_new(GTK_ORIENTATION_VERTICAL);
	vseparator[3]=gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    gtk_window_set_title(GTK_WINDOW(dialog),_WORD_COUNT);
    gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW(dialog),FALSE);

    g_signal_connect(dialog,"destroy",G_CALLBACK(gtk_widget_destroy),NULL);



    mainbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    charCountbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    lineCountbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    wordCountbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
 

    gtk_container_add(GTK_CONTAINER(dialog),mainbox);
	gtk_container_add(GTK_CONTAINER(mainbox),wordCountbox);
    gtk_container_add(GTK_CONTAINER(mainbox),charCountbox);
    gtk_container_add(GTK_CONTAINER(mainbox),lineCountbox);	



	gtk_box_pack_start (GTK_BOX (wordCountbox),
	                     GTK_WIDGET (gtk_label_new(_WORD_COUNT_HEADING)), TRUE, TRUE, 1);
	gtk_box_pack_start (GTK_BOX (wordCountbox),
	                    GTK_WIDGET(vseparator[1]),TRUE,TRUE,1);
	gtk_box_pack_start (GTK_BOX (wordCountbox),
                        GTK_WIDGET (gtk_label_new(cword_count)), TRUE, TRUE, 1);
	
    gtk_box_pack_start (GTK_BOX (charCountbox),
                        GTK_WIDGET (gtk_label_new(_CHAR_COUNT_HEADING)), TRUE, TRUE, 1);
	gtk_box_pack_start (GTK_BOX (charCountbox),
                        GTK_WIDGET (gtk_label_new(cchar_count)), TRUE, TRUE, 1);
	gtk_box_pack_start (GTK_BOX (charCountbox),
	                    GTK_WIDGET(vseparator[2]),TRUE,TRUE,1);

	gtk_box_pack_start (GTK_BOX (lineCountbox),
	                    GTK_WIDGET (gtk_label_new(_LINE_COUNT_HEADING)), TRUE, TRUE, 1);
	gtk_box_pack_start (GTK_BOX (lineCountbox),
	                    GTK_WIDGET (gtk_label_new(cline_count)), TRUE, TRUE, 1);

	gtk_box_pack_start (GTK_BOX (lineCountbox),
	                    GTK_WIDGET(vseparator[3]),TRUE,TRUE,1);

    g_signal_connect(dialog,"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
	
    gtk_widget_show_all(dialog);
}
