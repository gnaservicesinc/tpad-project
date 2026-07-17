/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014-2021 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_frequency_analysis.c , is part of tpad.
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
extern unsigned int frequency[];
extern unsigned int freqMode;
extern gchar freqModeChar;

extern size_t NumberOfElements;
unsigned int frequency[FA_TABLE_SIZE]={ 0 };
unsigned int freqMode=0;
gchar freqModeChar=0;

size_t NumberOfElements=0;

int tpad_frequency_analysis(void) {
	GtkTextIter start,end;
	gchar *temp_buffer;

	if (!gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(mBuff),
	                                          &start, &end))
		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(mBuff), &start, &end);

	/* This feature is read-only.  Beginning a user action here left the
	 * editor's undo group open indefinitely. */
	temp_buffer = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),
	                                       &start, &end, FALSE);

int no_rows=3;
double dcount=0.00;

memset(frequency, 0, sizeof(frequency));
freqMode = 0;
freqModeChar = 0;
NumberOfElements = strlen(temp_buffer);
unsigned int i = 0;
for(i=0;i<NumberOfElements;i++){
	unsigned int value = (guchar) temp_buffer[i];
	/* Preserve the historical ASCII-only analysis.  Treating individual
	 * UTF-8 bytes as characters would feed invalid strings to GTK labels. */
	if(value >= 33 && value < 128) {
frequency[value]++;
dcount+=1.00;
}
}

g_free(temp_buffer);



GtkWidget *mainbox,*graphbox,*modebox;

  GtkWidget *fawindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(fawindow), "Frequency Analysis");
  gtk_window_set_default_size(GTK_WINDOW(fawindow), 300, (no_rows*6));
  gtk_window_set_position(GTK_WINDOW(fawindow), GTK_WIN_POS_CENTER);

mainbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
graphbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
modebox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

gtk_container_add(GTK_CONTAINER(fawindow),mainbox);
gtk_container_add(GTK_CONTAINER(mainbox),graphbox);
gtk_container_add(GTK_CONTAINER(mainbox),modebox);

for(i=33;i<FA_TABLE_SIZE;i++) {


	if(frequency[i]) {
		if(frequency[i] > freqMode) {
				freqMode=frequency[i];
				freqModeChar=(char)i;

				}

double ldPercent=0;
ldPercent=(double)((double)((double)frequency[i]/(double) dcount)*100);
gchar ravalue[FA_TABLE_SIZE*6]={0};

snprintf(ravalue,(FA_TABLE_SIZE*6),"\n %u\n\n( %.2f%% )\n\n %c\t",frequency[i],ldPercent,(gchar) i);

gtk_box_pack_start (GTK_BOX (graphbox),GTK_WIDGET (gtk_label_new((const gchar *)ravalue)), TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (graphbox),GTK_WIDGET (gtk_label_new((const gchar *)"\t")), TRUE, TRUE, 1);
}
}

if(freqMode>25){
gchar ravalue[FA_TABLE_SIZE*6]={0};
snprintf(ravalue,(FA_TABLE_SIZE*6),"\n\n\n\nMode = \"%c\"",freqModeChar);

gtk_box_pack_start (GTK_BOX (modebox),GTK_WIDGET (gtk_label_new((const gchar *)ravalue)), TRUE, TRUE, 1);
}

  gtk_widget_show_all(fawindow);

  return 0;
}
