/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013, 2014, 2015, 2016, 2017, 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_config.c , is part of tpad.
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
//////////////////////////////////////////////////////////////////////////
/***********************************DEC**********************************/
//////////////////////////////////////////////////////////////////////////


static cfgSet userCfg;
static char* ccfile;
//////////////////////////////////////////////////////////////////////////
void cfg_read_in();
void set_conf_defaults();
void clean_userCfg();
void print_userCfg(char *fun);
void config_file();
void reset_config ();
void read_in();
void cfg_check(char CallingFunction[]);
void do_tpad_config_write_out();
//////////////////////////////////////////////////////////////////////////
/***********************************PUB**********************************/
//////////////////////////////////////////////////////////////////////////
void cfg_on_exit() {
	tpad_config_write_out();
}

 

void get_cfg_mask_int(gchar* description) {
	// debugging function.

	#ifdef DEBUG_TOGGLE
	int i = 0;
	printf("\n\ndebugging information.\n\Description\t=\t%s\nbitmask (int)\t=\t%i\nbitmask\t=\t\n\t\t",description,userCfg.ibitmask); 
	for(i=0;i <= POS_CFG_MAX_USED_VALUE; i++) ( ((userCfg.ibitmask >> i) & 1 )) ? printf("1") : printf("0");

	printf("\n\t\t");
	for(i=0;i <= POS_CFG_MAX_USED_VALUE; i++) printf("%i",i);

	printf("\n\n");
	#endif
}

int cfg_show_full_path() {
	get_cfg_mask_int((gchar*) "cfg_show_full_path() precheck");
	cfg_check((gchar*)__func__);
	get_cfg_mask_int((gchar*) "cfg_show_full_path() postcheck");
	
	return ( (int) ((userCfg.ibitmask >> POS_SHOW_FULL_PATH) & 1 ) );
}
int cfg_lang() {
	cfg_check((gchar*)__func__);
	return ( (int) ((userCfg.ibitmask >> POS_SHOW_LANG) & 1 ) );
}
int cfg_line() {
	cfg_check((gchar*)__func__);
	return ( (int) ((userCfg.ibitmask >> POS_SHOW_LINE_NUMBERS) & 1 ) );
}
/*
char *cfg_id() {

	cfg_check((gchar*)__func__);
	char* cuid_string= (char*) calloc( 1024, sizeof( char ) );

	if (cuid_string == NULL) exit(1);

	uuid_unparse(userCfg.configID,cuid_string); 

	return(" ");
}
*/
int cfg_spell(){
	cfg_check((gchar*)__func__);
	return ( (int) ((userCfg.ibitmask >> POS_SHOW_SPELLING_ERRORS) & 1 ) );
}

int cfg_line_wrap(){
	cfg_check((gchar*)__func__);
	return ( (int) ((userCfg.ibitmask >> POS_LINE_WRAP) & 1 ) );
}
int cfg_use_open_guard () {
	cfg_check((gchar*)__func__);
	return ( (int) ((userCfg.ibitmask >> POS_USE_OPEN_GUARD) & 1 ) );
}
int cfg_ut8bom() {
	cfg_check((gchar*)__func__);
	return ( (int) ((userCfg.ibitmask >> POS_BOM8) & 1 ) );
}
int cfg_use_magic(){
return(0);
}
	//#ifdef HAVE_LIBMAGIC
	//cfg_check((gchar*)__func__);
	//return ( (int) ((userCfg.ibitmask >> POS_USEMAGIC) & 1 ) );
	//#else
	//return(0);
	//#endif
//}
//#

int cfg_auto_tab(){
	#ifdef AUTO_TAB_TOGGLE
	cfg_check((gchar*)__func__);
	return ( (int) ((userCfg.ibitmask >> POS_AUTO_TAB) & 1 ) );
	#else
	return(0);
	#endif
}

int cfg_sWidth(){
	cfg_check((gchar*)__func__);
	return(userCfg.screenWidth);
}
int cfg_sHeight(){
	cfg_check((gchar*)__func__);
	return(userCfg.screenHeight);
}
int cfg_wWidth(){
	cfg_check((gchar*)__func__);
	return(userCfg.defualt_window_width);
}
int cfg_wHeight(){
	cfg_check((gchar*)__func__);
	return(userCfg.default_window_height);
}
int cfg_undo(){
	cfg_check((gchar*)__func__);
	return(userCfg.undo_level);
}
///////////////////////////////////////////////////////
void cfg_set_show_line(int opt) {
		if(opt) userCfg.ibitmask ^= (-1 ^ userCfg.ibitmask) & (1 << POS_SHOW_LINE_NUMBERS);
 		else userCfg.ibitmask ^= (-0 ^ userCfg.ibitmask) & (1 << POS_SHOW_LINE_NUMBERS);
}
void cfg_set_show_lang(int opt) {
		if(opt) userCfg.ibitmask ^= (-1 ^ userCfg.ibitmask) & (1 << POS_SHOW_LANG);
 		else userCfg.ibitmask ^= (-0 ^ userCfg.ibitmask) & (1 << POS_SHOW_LANG);
}
void cfg_set_show_full_path(int opt) {
		if(opt) userCfg.ibitmask ^= (-1 ^ userCfg.ibitmask) & (1 << POS_SHOW_FULL_PATH);
 		else userCfg.ibitmask ^= (-0 ^ userCfg.ibitmask) & (1 << POS_SHOW_FULL_PATH);
}
void cfg_set_show_spelling(int opt) {
		if(opt) userCfg.ibitmask ^= (-1 ^ userCfg.ibitmask) & (1 << POS_SHOW_SPELLING_ERRORS);
 		else userCfg.ibitmask ^= (-0 ^ userCfg.ibitmask) & (1 << POS_SHOW_SPELLING_ERRORS);
}
void cfg_set_show_line_wrap(int opt) {
		if(opt) userCfg.ibitmask ^= (-1 ^ userCfg.ibitmask) & (1 << POS_LINE_WRAP);
 		else userCfg.ibitmask ^= (-0 ^ userCfg.ibitmask) & (1 << POS_LINE_WRAP);
}
void cfg_set_use_open_guard(int opt) {
		if(opt) userCfg.ibitmask ^= (-1 ^ userCfg.ibitmask) & (1 << POS_USE_OPEN_GUARD);
 		else userCfg.ibitmask ^= (-0 ^ userCfg.ibitmask) & (1 << POS_USE_OPEN_GUARD);
}
void cfg_set_use_ut8bom(int opt) {
		if(opt) userCfg.ibitmask ^= (-1 ^ userCfg.ibitmask) & (1 << POS_BOM8);
 		else userCfg.ibitmask ^= (-0 ^ userCfg.ibitmask) & (1 << POS_BOM8);
}
void cfg_set_screen_width(int opt){
	#ifdef DEBUG_TOGGLE
	g_print("\nSet Screen Width\t=\t%i\n",opt);
	#endif
	if(opt > userCfg.defualt_window_width) userCfg.screenWidth=opt;
	else gerror_warn(_ERROR_SETTING_SETTING,(gchar*)__func__,0,0);
}
void cfg_set_screen_height(int opt){
	#ifdef DEBUG_TOGGLE
	g_print("\nSet Screen Height\t=\t%i\n",opt);
	#endif
	if(opt > userCfg.default_window_height) userCfg.screenHeight=opt;
	else gerror_warn(_ERROR_SETTING_SETTING,(gchar*)__func__,0,0);
}

void cfg_set_default_width(int opt){
	if(opt >= _TPAD_CFG_WIDTH_MIN) userCfg.defualt_window_width=opt;
	else gerror_warn(_ERROR_TOO_SMALL,(gchar*)__func__,0,0);
}
void cfg_set_default_height(int opt){
	if(opt >= _TPAD_CFG_HEIGHT_MIN) userCfg.default_window_height=opt;
	else gerror_warn(_ERROR_TOO_SMALL,(gchar*)__func__,0,0);
}
void cfg_set_undo(int opt){
	if(opt >= 0) userCfg.undo_level=opt;
	else gerror_warn(_ERROR_SETTING_SETTING,(gchar*)__func__,0,0);
}


void cfg_set_use_magic(int opt){
// #ifdef HAVE_LIBMAGIC
//	if(opt >= 0 && opt <=1) {
//		if(opt) userCfg.ibitmask ^= (-1 ^ userCfg.ibitmask) & (1 << POS_USEMAGIC);
 //		else userCfg.ibitmask ^= (-0 ^ userCfg.ibitmask) & (1 << POS_USEMAGIC);

//	}
//	else gerror_warn(_ERROR_SETTING_SETTING,(gchar*)__func__,0,0);
//#endif
}



void cfg_set_auto_tab(int opt) {
#ifdef AUTO_TAB_TOGGLE
	if(opt >= 0 && opt <=1) {
		if(opt) userCfg.ibitmask ^= (-1 ^ userCfg.ibitmask) & (1 << POS_AUTO_TAB);
 		else userCfg.ibitmask ^= (-0 ^ userCfg.ibitmask) & (1 << POS_AUTO_TAB);

	}
	else gerror_warn(_ERROR_SETTING_SETTING,(gchar*)__func__,0,0);
#endif
}

//////////////////////////////////////////////////////////////////////////

void config_setup() {
	config_file();
	set_conf_defaults();
	if(access(ccfile, R_OK|W_OK ) != -1) cfg_read_in();
	else cfg_save();
}
//////////////////////////////////////////////////////////////////////////
void cfg_save() {
	cfg_check((gchar*)__func__);
	tpad_config_write_out();
}

//////////////////////////////////////////////////////////////////////////
/***********************************PRI**********************************/
//////////////////////////////////////////////////////////////////////////
void cfg_check(char CallingFunction[]) {
	if (!is_userCfg_valid()){
		print_userCfg((gchar*)&CallingFunction);
		clean_userCfg();
	}
}

void cfg_read_in() {
	read_in();
	cfg_check((gchar*)__func__);
}
//////////////////////////////////////////////////////////////////////////
void set_conf_defaults() {
	userCfg.ibitmask=0;
	userCfg.defualt_window_width=_DEFAULT_WINDOW_WIDTH;
	userCfg.default_window_height=_DEFAULT_WINDOW_HEIGHT;
	userCfg.undo_level=FALSE;
	userCfg.screenWidth=FALSE;
	userCfg.screenHeight=FALSE;
	//uuid_generate(userCfg.configID);
	//#ifdef HAVE_LIBMAGIC
	//cfg_set_use_magic(FALSE);
	//#endif
	cfg_set_use_ut8bom(FALSE);
	#ifdef AUTO_TAB_TOGGLE
	cfg_set_auto_tab(FALSE);
	#endif
	cfg_set_show_line(TRUE);
	cfg_set_show_lang(FALSE);
	cfg_set_show_spelling(FALSE);
	cfg_set_show_line(FALSE);
	cfg_set_show_full_path(TRUE);
	cfg_set_show_line_wrap(TRUE);
	cfg_use_open_guard(FALSE);

}
//////////////////////////////////////////////////////////////////////////
void print_userCfg(char *fun){
	fprintf(stdout,"\nDEBUG\n\nDUMPING CONFIG\n");
	fwrite(&userCfg, sizeof(cfgSet),1,stdout);
}
//////////////////////////////////////////////////////////////////////////

int is_userCfg_valid() {

		if(userCfg.defualt_window_width < _TPAD_CFG_WIDTH_MIN || userCfg.defualt_window_width > WIDTH_MAX) {
	 return(FALSE);
	}


		if(userCfg.default_window_height < _TPAD_CFG_HEIGHT_MIN || userCfg.default_window_height > HEIGHT_MAX){
	 return(FALSE);
	}

		if(userCfg.undo_level < 0 || userCfg.undo_level > UNDOMAX) 	{
 		return(FALSE);
	}	

		return(TRUE);
	
}
//////////////////////////////////////////////////////////////////////////
// Fix userCFG by replacing invalid values with default ones while
// preserving valid values (By range). 
//////////////////////////////////////////////////////////////////////////

void clean_userCfg(){
		if(userCfg.screenWidth < _TPAD_CFG_WIDTH_MIN) {
			userCfg.screenWidth=_DEFAULT_WINDOW_WIDTH;
		}


		if(userCfg.screenHeight < _TPAD_CFG_HEIGHT_MIN) {
			 userCfg.screenHeight=_DEFAULT_WINDOW_HEIGHT;
		}


		if(userCfg.defualt_window_width < _TPAD_CFG_WIDTH_MIN || userCfg.defualt_window_width >  WIDTH_MAX  )  {
			userCfg.defualt_window_width=_DEFAULT_WINDOW_WIDTH;
		}


		if(userCfg.default_window_height < _TPAD_CFG_HEIGHT_MIN || userCfg.default_window_height > HEIGHT_MAX) 	 {
			userCfg.default_window_height=_DEFAULT_WINDOW_HEIGHT;
		}
	

		if(userCfg.undo_level < 0 || userCfg.undo_level > UNDOMAX) {
			 userCfg.undo_level=FALSE;
		}
		
}
//////////////////////////////////////////////////////////////////////////
void config_file(){
	struct passwd *pw = getpwuid(syscall(__NR_getuid));
	ccfile = g_strconcat((gchar *)pw->pw_dir,(gchar *)CONFIG_FILE_SUFFIX,NULL);
}
//////////////////////////////////////////////////////////////////////////
int tpad_config_write_out(){
	do_tpad_config_write_out();
	 return(0);
}
//////////////////////////////////////////////////////////////////////////
void reset_config (){
	 if (remove(ccfile) != 0 ){
		gerror_warn(_BAD_CFG_REPLACE_FAIL,ccfile,1,0); 
	 }
	 else gerror_warn(_CFG_FILE_INVALID,_REMOVED_INVALID_CFG_FILE,1,0);
		 
}
//////////////////////////////////////////////////////////////////////////
void read_in()
{
	FILE *ptr_cfg;


	ptr_cfg=fopen(ccfile,"rb");
	if (!ptr_cfg) {
		reset_config ();
	}
	else {
		//Success opening, read data into the struct 
		fseek(ptr_cfg, sizeof(cfgSet), SEEK_END);
		rewind(ptr_cfg);
		size_t result;
		long lSize =1;
		result=fread(&userCfg,sizeof(cfgSet),1,ptr_cfg);
		if (result != lSize){
			reset_config ();
		}
		fclose(ptr_cfg);
	}
	

}
//////////////////////////////////////////////////////////////////////////	
void do_tpad_config_write_out()
{
	FILE *ptr_cfg;
	ptr_cfg=fopen(ccfile,"wb");
	
	if(!ptr_cfg) reset_config ();
	else {
		fwrite(&userCfg, sizeof(cfgSet),1,ptr_cfg);
		fclose(ptr_cfg);
		}

}

