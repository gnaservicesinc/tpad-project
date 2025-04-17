/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014-2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_buff.c , is part of tpad.
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
extern GtkSourceBuffer *mBuff;
extern GtkSourceView *view;
extern int tpad_text_buffer_ini_check;
#ifndef _TPAD_TEXT_BUFF_DS_
#define _TPAD_TEXT_BUFF_DS_
struct tpad_text_buff {
	unsigned int key;
	GtkSourceView *view;
	GtkSourceBuffer  *buffer;
	struct tpad_text_buff* left;
	struct tpad_text_buff* right;
};
#endif
#ifndef _TPAD_TEXT_BUFF_EX_COUNT_
#define _TPAD_TEXT_BUFF_EX_COUNT_
extern unsigned int global_key_count;
#endif
#ifndef _TPAD_TEXT_BUFF_GLOB_
#define _TPAD_TEXT_BUFF_GLOB_
extern struct tpad_text_buff* global_text_buffs;
#endif


unsigned int global_key_count=0;
int tpad_text_buffer_ini_check=0;

struct tpad_text_buff* global_text_buffs;

static struct tpad_text_buff *tpad_text_buff_new(unsigned int key);
static void destroy_text_buff(struct tpad_text_buff *leaf);
static void tpad_buff_insert(unsigned int key, struct tpad_text_buff **leaf);
static struct tpad_text_buff *tpad_buff_search(unsigned int key,  struct tpad_text_buff *leaf);
static void tpad_text_buff_init(unsigned int key);


static void tpad_text_buff_init(unsigned int key){
/*
	if(!tpad_text_buffer_ini_check){
		tpad_text_buffer_ini_check=1;
		global_key_count=0;
		global_text_buffs=(struct tpad_text_buff*) tpad_text_buff_new(key);
	}
*/
}
static struct tpad_text_buff* tpad_text_buff_new(unsigned int key){
 return((struct tpad_text_buff*) 0);
/*
	 struct tpad_text_buff *new=(struct tpad_text_buff*) calloc(1, sizeof(struct tpad_text_buff) );
	if(new == NULL) exit(1);
	
	new->buffer=(GtkSourceBuffer *) gtk_source_buffer_new(NULL);
	new->view=(GtkSourceView*) gtk_source_view_new_with_buffer(new->buffer);
	new->left=0;
	new->right=0;
	new->key=key;
	return((struct tpad_text_buff *)new);
*/
}
static void tpad_buff_insert(unsigned int key, struct tpad_text_buff **leaf)
{
/*
	if(!tpad_text_buffer_ini_check) tpad_text_buff_init(key);

	if( *leaf == 0 ){ //*leaf = (struct tpad_text_buff*) tpad_text_buff_new();
		 *leaf = (struct tpad_text_buff*) calloc(1, sizeof(struct tpad_text_buff) );
		if ( !*leaf ) exit(1);
		(*leaf)->key=key;
		(*leaf)->buffer=(GtkSourceBuffer *) gtk_source_buffer_new(NULL);
		(*leaf)->view=(GtkSourceView*) gtk_source_view_new_with_buffer((*leaf)->buffer);
		// initialize the children to null 
        	(*leaf)->left = 0;    
        	(*leaf)->right = 0; 
		}
	else if(key < (*leaf)->key) tpad_buff_insert(key, &(*leaf)->left );
	else if(key > (*leaf)->key) tpad_buff_insert(key, &(*leaf)->right );
    */
}
static struct tpad_text_buff *tpad_buff_search(unsigned int key,  struct tpad_text_buff *leaf){
return((struct tpad_text_buff*) 0);
/*
if(!tpad_text_buffer_ini_check) tpad_text_buff_init(key);


  if( leaf != 0 ){
      if(key==leaf->key) return(leaf);
      else if(key<leaf->key) return(tpad_buff_search(key, leaf->left));
      else return(tpad_buff_search(key, leaf->right) );   
  }
  else return((struct tpad_text_buff*) 0);
*/
}
static void destroy_text_buff(struct tpad_text_buff *leaf){

//if(!tpad_text_buffer_ini_check) tpad_text_buff_init();
/*
  if( leaf != 0 ){
      destroy_text_buff(leaf->left);
      destroy_text_buff(leaf->right);
      free( leaf );
  }
*/
}

unsigned int tpad_buff_add(){
if(!tpad_text_buffer_ini_check){
tpad_text_buffer_ini_check=1;
//mBuff=(GtkSourceBuffer *) gtk_source_buffer_new(NULL);
//view=(GtkSourceView*) gtk_source_view_new_with_buffer(mBuff);
}
return(1);
/*
if(!tpad_text_buffer_ini_check) tpad_text_buff_init(global_key_count);
else {
global_key_count++;
tpad_buff_insert(global_key_count,&global_text_buffs);
return( (unsigned int) global_key_count - 1);
}
return( (unsigned int) global_key_count);
*/
}
GtkSourceView *tpad_buff_get_view(unsigned int key){
return(view);
//if(!tpad_text_buffer_ini_check) tpad_text_buff_init(key);
//return( (tpad_buff_search(key,global_text_buffs))->view);
}

GtkSourceBuffer  *tpad_buff_get_buff(unsigned int key){
tpad_clean_mbuff();
return(mBuff);
//if(!tpad_text_buffer_ini_check) tpad_text_buff_init(key);
//return( (tpad_buff_search(key,global_text_buffs))->buffer);
}
void set_buffer(){


	    gtk_source_buffer_begin_not_undoable_action(GTK_SOURCE_BUFFER(mBuff));
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(mBuff),content,-1);
        gtk_source_buffer_end_not_undoable_action(GTK_SOURCE_BUFFER(mBuff));
        gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(mBuff),FALSE);
        GtkTextIter iter;
        gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(mBuff),&iter);
        gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(mBuff),&iter);
	content = NULL;
	tpad_clean_mbuff();
}

gboolean tpad_clean_mbuff() {
GtkTextIter start,end;

gtk_text_buffer_get_bounds( GTK_TEXT_BUFFER(mBuff),&start,&end);

char* data_to_convert = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),&start,&end,FALSE);

if (data_to_convert == NULL) return(FALSE);

gtk_source_buffer_begin_not_undoable_action(GTK_SOURCE_BUFFER(mBuff));

gtk_text_buffer_set_text(GTK_TEXT_BUFFER(mBuff),tpad_codeset_convert_to_curent_local_from_utf8((const gchar* ) data_to_convert,strlen(data_to_convert) ),-1);
	 gtk_source_buffer_end_not_undoable_action(GTK_SOURCE_BUFFER(mBuff));
	free(data_to_convert );
	return(TRUE);
}
