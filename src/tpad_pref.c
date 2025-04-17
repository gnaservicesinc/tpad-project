/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_pref.c , is part of tpad.
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
static GtkSpinButton *xSpinButton,*ySpinButton,*uSpinButton,*stSpingButton;


void do_UI_pref_update(GtkWidget *caller, gpointer dialog)
{
	cfg_set_default_width(gtk_spin_button_get_value_as_int (xSpinButton));
	cfg_set_default_height(gtk_spin_button_get_value_as_int (ySpinButton));
	cfg_set_undo(gtk_spin_button_get_value_as_int (uSpinButton));
	//cfg_set_stack_quantity(gtk_spin_button_get_value_as_int (stSpingButton));		
	set_language(); 
	//gtk_widget_destroy((GtkWidget*) dialog);
	cfg_save();
}
void show_ui_prefs (){

	GtkWidget *dialog,*mainbox,*buttonbox,*xbox,*ybox,*ubox,*stbox,*buttonAccept; 
	dialog=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(dialog),_UI_SETTINGS);
    gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW(dialog),FALSE);
	g_signal_connect(dialog,"destroy",G_CALLBACK(gtk_widget_destroy),NULL);
	mainbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	xbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	ybox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	ubox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0); 
	stbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	buttonbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);  

	gtk_container_add(GTK_CONTAINER(dialog),mainbox);
	gtk_container_add(GTK_CONTAINER(mainbox),xbox);
    	gtk_container_add(GTK_CONTAINER(mainbox),ybox);
	gtk_container_add(GTK_CONTAINER(mainbox),ubox);
	gtk_container_add(GTK_CONTAINER(mainbox),stbox);                     
	gtk_container_add(GTK_CONTAINER(mainbox),buttonbox);    

	gtk_box_pack_start (GTK_BOX (xbox),GTK_WIDGET (gtk_label_new(_DEFAULT_WIDTH)), TRUE, TRUE, 1); 
	xSpinButton=(GtkSpinButton*)gtk_spin_button_new_with_range(50,6000,10);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON(xSpinButton),0);                     
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(xSpinButton), TRUE);
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON(xSpinButton),TRUE);  
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(xSpinButton),(gdouble)cfg_wWidth());                  
	gtk_box_pack_start (GTK_BOX (xbox),GTK_WIDGET(xSpinButton),TRUE,TRUE,1);
	

	gtk_box_pack_start (GTK_BOX (ybox),GTK_WIDGET (gtk_label_new(_DEFAULT_HEIGHT)), TRUE, TRUE, 1); 
	ySpinButton=(GtkSpinButton*)gtk_spin_button_new_with_range(50,6000,10);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON(ySpinButton),0);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(ySpinButton), TRUE);     
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON(ySpinButton),TRUE);                 
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(ySpinButton),(gdouble)cfg_wHeight()); 
	gtk_box_pack_start (GTK_BOX (ybox),GTK_WIDGET(ySpinButton),TRUE,TRUE,1);

	gtk_box_pack_start (GTK_BOX (ubox),GTK_WIDGET (gtk_label_new(_UNDO_LEVEL)), TRUE, TRUE, 1); 
	uSpinButton=(GtkSpinButton*)gtk_spin_button_new_with_range(0,UNDO_MAX,5);
	gtk_spin_button_set_digits (GTK_SPIN_BUTTON(uSpinButton),0);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(uSpinButton), TRUE);  
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON(uSpinButton),TRUE);                    
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(uSpinButton),(gdouble)cfg_undo()); 
	gtk_box_pack_start (GTK_BOX (ubox),GTK_WIDGET(uSpinButton),TRUE,TRUE,1);

	GtkWidget *pToolbar = (GtkWidget *) gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(pToolbar),GTK_TOOLBAR_ICONS);
	GtkToolItem* toolApply = (GtkToolItem*) gtk_tool_button_new(NULL,"Apply");
	gtk_toolbar_insert(GTK_TOOLBAR(pToolbar),GTK_TOOL_ITEM(toolApply),0);

	
	gtk_box_pack_start (GTK_BOX (buttonbox),GTK_WIDGET (pToolbar), TRUE, TRUE, 1);
	
	g_signal_connect(toolApply,"clicked",G_CALLBACK(do_UI_pref_update),(gpointer) dialog); 

	gtk_widget_show_all(dialog);	
	
	
}