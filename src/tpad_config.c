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
#include <sys/file.h>
//////////////////////////////////////////////////////////////////////////
/***********************************DEC**********************************/
//////////////////////////////////////////////////////////////////////////


static cfgSet userCfg;
static char* ccfile;
static gboolean cfg_needs_upgrade;
static gboolean cfg_recent_settings_dirty;
//////////////////////////////////////////////////////////////////////////
static gboolean cfg_read_in(void);
static void set_conf_defaults(void);
static void clean_userCfg(void);
static void print_userCfg(const char *fun);
static void config_file(void);
static void reset_config(void);
static gboolean read_in(void);
static gboolean sync_recent_settings_from_stream(FILE *config);
static gboolean sync_recent_settings_from_disk(gboolean acquire_lock);
static void cfg_check(const char CallingFunction[]);
static void do_tpad_config_write_out(void);
//////////////////////////////////////////////////////////////////////////
/***********************************PUB**********************************/
//////////////////////////////////////////////////////////////////////////
void cfg_on_exit(void) {
	tpad_config_write_out();
	g_clear_pointer(&ccfile, g_free);
}



void get_cfg_mask_int(gchar* description) {
	(void) description;
	// debugging function.

	#ifdef DEBUG_TOGGLE
	int i = 0;
	printf("\n\ndebugging information.\nDescription\t=\t%s\nbitmask (int)\t=\t%i\nbitmask\t=\t\n\t\t",description,userCfg.ibitmask);
	for(i=0;i <= POS_CFG_MAX_USED_VALUE; i++) ( ((userCfg.ibitmask >> i) & 1 )) ? printf("1") : printf("0");

	printf("\n\t\t");
	for(i=0;i <= POS_CFG_MAX_USED_VALUE; i++) printf("%i",i);

	printf("\n\n");
	#endif
}

int cfg_show_full_path(void) {
	get_cfg_mask_int((gchar*) "cfg_show_full_path() precheck");
	cfg_check((gchar*)__func__);
	get_cfg_mask_int((gchar*) "cfg_show_full_path() postcheck");

	return ( (int) ((userCfg.ibitmask >> POS_SHOW_FULL_PATH) & 1 ) );
}
int cfg_lang(void) {
	cfg_check((gchar*)__func__);
	return ( (int) ((userCfg.ibitmask >> POS_SHOW_LANG) & 1 ) );
}
int cfg_line(void) {
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
int cfg_spell(void){
	cfg_check((gchar*)__func__);
	return ( (int) ((userCfg.ibitmask >> POS_SHOW_SPELLING_ERRORS) & 1 ) );
}

int cfg_line_wrap(void){
	cfg_check((gchar*)__func__);
	return ( (int) ((userCfg.ibitmask >> POS_LINE_WRAP) & 1 ) );
}
int cfg_use_open_guard(void) {
	cfg_check((gchar*)__func__);
	return ( (int) ((userCfg.ibitmask >> POS_USE_OPEN_GUARD) & 1 ) );
}
int cfg_recent_files_enabled(void) {
	if (!cfg_recent_settings_dirty)
		(void) sync_recent_settings_from_disk(TRUE);
	cfg_check((gchar*)__func__);
	return ( (int) ((userCfg.ibitmask >> POS_RECENT_FILES) & 1 ) );
}
int cfg_recent_files_limit(void) {
	if (!cfg_recent_settings_dirty)
		(void) sync_recent_settings_from_disk(TRUE);
	cfg_check((gchar*)__func__);
	return userCfg.recent_files_limit;
}
int cfg_ut8bom(void) {
	cfg_check((gchar*)__func__);
	return ( (int) ((userCfg.ibitmask >> POS_BOM8) & 1 ) );
}
int cfg_auto_tab(void){
	#ifdef AUTO_TAB_TOGGLE
	cfg_check((gchar*)__func__);
	return ( (int) ((userCfg.ibitmask >> POS_AUTO_TAB) & 1 ) );
	#else
	return(0);
	#endif
}

int cfg_sWidth(void){
	cfg_check((gchar*)__func__);
	return(userCfg.screenWidth);
}
int cfg_sHeight(void){
	cfg_check((gchar*)__func__);
	return(userCfg.screenHeight);
}
int cfg_wWidth(void){
	cfg_check((gchar*)__func__);
	return(userCfg.defualt_window_width);
}
int cfg_wHeight(void){
	cfg_check((gchar*)__func__);
	return(userCfg.default_window_height);
}
int cfg_undo(void){
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
void cfg_set_recent_files_enabled(int opt) {
		if(opt) userCfg.ibitmask ^= (-1 ^ userCfg.ibitmask) & (1 << POS_RECENT_FILES);
		else userCfg.ibitmask ^= (-0 ^ userCfg.ibitmask) & (1 << POS_RECENT_FILES);
		cfg_recent_settings_dirty = TRUE;
}
void cfg_set_recent_files_limit(int opt) {
	if (opt >= 0 && opt <= TPAD_RECENT_FILES_MAXIMUM) {
		userCfg.recent_files_limit = opt;
		cfg_recent_settings_dirty = TRUE;
	} else
		gerror_warn(_ERROR_SETTING_SETTING,(gchar*)__func__,0,0);
}
void cfg_set_use_ut8bom(int opt) {
		if(opt) userCfg.ibitmask ^= (-1 ^ userCfg.ibitmask) & (1 << POS_BOM8);
 		else userCfg.ibitmask ^= (-0 ^ userCfg.ibitmask) & (1 << POS_BOM8);
}
void cfg_set_screen_width(int opt){
	#ifdef DEBUG_TOGGLE
	g_print("\nSet Screen Width\t=\t%i\n",opt);
	#endif
	// Clamp and default rather than warning: during early startup some
	// environments may report 0 or invalid sizes. Avoid noisy popups.
	if (opt <= 0) opt = _DEFAULT_WINDOW_WIDTH;
	if (opt > WIDTH_MAX) opt = WIDTH_MAX;
	userCfg.screenWidth = opt;
}
void cfg_set_screen_height(int opt){
	#ifdef DEBUG_TOGGLE
	g_print("\nSet Screen Height\t=\t%i\n",opt);
	#endif
	// Clamp and default rather than warning: during early startup some
	// environments may report 0 or invalid sizes. Avoid noisy popups.
	if (opt <= 0) opt = _DEFAULT_WINDOW_HEIGHT;
	if (opt > HEIGHT_MAX) opt = HEIGHT_MAX;
	userCfg.screenHeight = opt;
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

void cfg_set_auto_tab(int opt) {
#ifndef AUTO_TAB_TOGGLE
	(void) opt;
#endif
#ifdef AUTO_TAB_TOGGLE
	if(opt >= 0 && opt <=1) {
		if(opt) userCfg.ibitmask ^= (-1 ^ userCfg.ibitmask) & (1 << POS_AUTO_TAB);
 		else userCfg.ibitmask ^= (-0 ^ userCfg.ibitmask) & (1 << POS_AUTO_TAB);

	}
	else gerror_warn(_ERROR_SETTING_SETTING,(gchar*)__func__,0,0);
#endif
}

//////////////////////////////////////////////////////////////////////////

void config_setup(void) {
	gboolean loaded;

	config_file();
	cfg_recent_settings_dirty = FALSE;
	set_conf_defaults();
	cfg_needs_upgrade = FALSE;
	loaded = cfg_read_in();
	if (loaded && !cfg_needs_upgrade)
		cfg_recent_settings_dirty = FALSE;
	if (!loaded || cfg_needs_upgrade)
		cfg_save();
}
//////////////////////////////////////////////////////////////////////////
void cfg_save(void) {
	cfg_check((gchar*)__func__);
	tpad_config_write_out();
}

//////////////////////////////////////////////////////////////////////////
/***********************************PRI**********************************/
//////////////////////////////////////////////////////////////////////////
static void cfg_check(const char CallingFunction[]) {
	if (!is_userCfg_valid()){
		print_userCfg(CallingFunction);
		clean_userCfg();
	}
}

static gboolean cfg_read_in(void) {
	gboolean loaded = read_in();

	cfg_check((gchar*)__func__);
	return loaded;
}
//////////////////////////////////////////////////////////////////////////
static void set_conf_defaults(void) {
	userCfg.ibitmask=0;
	userCfg.defualt_window_width=_DEFAULT_WINDOW_WIDTH;
	userCfg.default_window_height=_DEFAULT_WINDOW_HEIGHT;
	userCfg.undo_level=0;
	userCfg.screenWidth=FALSE;
	userCfg.screenHeight=FALSE;
	userCfg.recent_files_limit=TPAD_RECENT_FILES_DEFAULT;
	//uuid_generate(userCfg.configID);
	cfg_set_use_ut8bom(FALSE);
	#ifdef AUTO_TAB_TOGGLE
	cfg_set_auto_tab(FALSE);
	#endif
	cfg_set_show_line(TRUE);
	cfg_set_show_lang(FALSE);
	cfg_set_show_spelling(FALSE);
	cfg_set_show_full_path(TRUE);
	cfg_set_show_line_wrap(TRUE);
	cfg_set_use_open_guard(FALSE);
	cfg_set_recent_files_enabled(TRUE);

}
//////////////////////////////////////////////////////////////////////////
static void print_userCfg(const char *fun){
	(void) fun;
	#ifdef DEBUG_TOGGLE
	fprintf(stdout,
	        "\nDEBUG\n\nINVALID CONFIG in %s\n"
	        "ibitmask=%d width=%d height=%d undo=%d screen=%dx%d recent=%d\n",
	        fun, userCfg.ibitmask, userCfg.defualt_window_width,
	        userCfg.default_window_height, userCfg.undo_level,
	        userCfg.screenWidth, userCfg.screenHeight,
	        userCfg.recent_files_limit);
	#endif
}
//////////////////////////////////////////////////////////////////////////

int is_userCfg_valid(void) {

		if(userCfg.defualt_window_width < _TPAD_CFG_WIDTH_MIN || userCfg.defualt_window_width > WIDTH_MAX) {
	 return(FALSE);
	}


		if(userCfg.default_window_height < _TPAD_CFG_HEIGHT_MIN || userCfg.default_window_height > HEIGHT_MAX){
	 return(FALSE);
	}

		if(userCfg.undo_level < 0 || userCfg.undo_level > UNDOMAX) 	{
 		return(FALSE);
	}

		if(userCfg.recent_files_limit < 0 ||
		   userCfg.recent_files_limit > TPAD_RECENT_FILES_MAXIMUM) {
	 return(FALSE);
	}

		return(TRUE);

}
//////////////////////////////////////////////////////////////////////////
// Fix userCFG by replacing invalid values with default ones while
// preserving valid values (By range).
//////////////////////////////////////////////////////////////////////////

static void clean_userCfg(void){
		if(userCfg.screenWidth <= 0) {
			userCfg.screenWidth=_DEFAULT_WINDOW_WIDTH;
		}


		if(userCfg.screenHeight <= 0) {
			 userCfg.screenHeight=_DEFAULT_WINDOW_HEIGHT;
		}


		if(userCfg.defualt_window_width < _TPAD_CFG_WIDTH_MIN || userCfg.defualt_window_width >  WIDTH_MAX  )  {
			userCfg.defualt_window_width=_DEFAULT_WINDOW_WIDTH;
		}


		if(userCfg.default_window_height < _TPAD_CFG_HEIGHT_MIN || userCfg.default_window_height > HEIGHT_MAX) 	 {
			userCfg.default_window_height=_DEFAULT_WINDOW_HEIGHT;
		}


		if(userCfg.undo_level < 0 || userCfg.undo_level > UNDOMAX) {
			 userCfg.undo_level=0;
		}

		if(userCfg.recent_files_limit < 0 ||
		   userCfg.recent_files_limit > TPAD_RECENT_FILES_MAXIMUM) {
			 userCfg.recent_files_limit=TPAD_RECENT_FILES_DEFAULT;
		}

}
//////////////////////////////////////////////////////////////////////////
static void config_file(void){
	const gchar *config_override = g_getenv("TPAD_CONFIG_FILE");
	const gchar *home = g_get_home_dir();

	g_clear_pointer(&ccfile, g_free);
	if (config_override != NULL && config_override[0] != '\0' &&
		g_path_is_absolute(config_override)) {
		ccfile = g_strdup(config_override);
		return;
	}
	if (home == NULL)
		home = ".";
	ccfile = g_strconcat(home, CONFIG_FILE_SUFFIX, NULL);
}
//////////////////////////////////////////////////////////////////////////
int tpad_config_write_out(void){
	do_tpad_config_write_out();
	 return(0);
}
//////////////////////////////////////////////////////////////////////////
static void reset_config(void){
	 if (remove(ccfile) != 0 ){
		gerror_warn(_BAD_CFG_REPLACE_FAIL,ccfile,1,0);
	 }
	 else gerror_warn(_CFG_FILE_INVALID,_REMOVED_INVALID_CFG_FILE,1,0);

}
//////////////////////////////////////////////////////////////////////////
static gboolean read_in(void)
{
	FILE *ptr_cfg;
	gboolean loaded;
	int lock_result;
	long file_size;
	const size_t legacy_size = offsetof(cfgSet, recent_files_limit);

	ptr_cfg=fopen(ccfile,"rb");
	if (!ptr_cfg)
		return FALSE;
	do {
		lock_result = flock(fileno(ptr_cfg), LOCK_SH);
	} while (lock_result != 0 && errno == EINTR);
	if (lock_result != 0) {
		(void) fclose(ptr_cfg);
		return FALSE;
	}

	loaded = fseek(ptr_cfg, 0, SEEK_END) == 0;
	file_size = loaded ? ftell(ptr_cfg) : -1;
	if (loaded)
		loaded = fseek(ptr_cfg, 0, SEEK_SET) == 0;
	if (loaded && file_size == (long) sizeof(cfgSet)) {
		loaded = fread(&userCfg, sizeof(cfgSet), 1, ptr_cfg) == 1;
	} else if (loaded && file_size == (long) legacy_size) {
		/* The recent-file fields were appended so all legacy values retain
		 * their original offsets.  Supply defaults only for the new values. */
		loaded = fread(&userCfg, legacy_size, 1, ptr_cfg) == 1;
		if (loaded) {
			userCfg.recent_files_limit = TPAD_RECENT_FILES_DEFAULT;
			cfg_set_recent_files_enabled(TRUE);
			cfg_needs_upgrade = TRUE;
		}
	} else {
		loaded = FALSE;
	}
	(void) flock(fileno(ptr_cfg), LOCK_UN);
	if (fclose(ptr_cfg) != 0)
		loaded = FALSE;
	if (!loaded)
		reset_config();

	return loaded;


}
//////////////////////////////////////////////////////////////////////////
static gboolean sync_recent_settings_from_stream(FILE *config)
{
	cfgSet stored;
	gboolean loaded = FALSE;
	long file_size;

	if (fseek(config, 0, SEEK_END) == 0) {
		file_size = ftell(config);
		if (file_size == (long) sizeof(stored) &&
		    fseek(config, 0, SEEK_SET) == 0 &&
		    fread(&stored, sizeof(stored), 1, config) == 1 &&
		    stored.recent_files_limit >= 0 &&
		    stored.recent_files_limit <= TPAD_RECENT_FILES_MAXIMUM) {
			userCfg.ibitmask &= ~(1 << POS_RECENT_FILES);
			userCfg.ibitmask |= stored.ibitmask & (1 << POS_RECENT_FILES);
			userCfg.recent_files_limit = stored.recent_files_limit;
			loaded = TRUE;
		}
	}
	return loaded;
}
//////////////////////////////////////////////////////////////////////////
static gboolean sync_recent_settings_from_disk(gboolean acquire_lock)
{
	FILE *config;
	gboolean loaded = FALSE;
	int lock_result = 0;

	if (ccfile == NULL)
		return FALSE;
	config = fopen(ccfile, "rb");
	if (config == NULL)
		return FALSE;
	if (acquire_lock) {
		do {
			lock_result = flock(fileno(config), LOCK_SH);
		} while (lock_result != 0 && errno == EINTR);
	}
	if (lock_result == 0)
		loaded = sync_recent_settings_from_stream(config);
	if (acquire_lock && lock_result == 0)
		(void) flock(fileno(config), LOCK_UN);
	(void) fclose(config);
	return loaded;
}
//////////////////////////////////////////////////////////////////////////
static void do_tpad_config_write_out(void)
{
	FILE *ptr_cfg;
	gboolean failed = TRUE;
	int lock_descriptor;
	int lock_result;
	struct stat config_status;

	lock_descriptor = g_open(ccfile, O_CREAT | O_RDWR, 0600);
	if (lock_descriptor < 0) {
		gerror_warn(_FAILED_SAVE_FILE, ccfile, TRUE, FALSE);
		return;
	}
	if (fstat(lock_descriptor, &config_status) != 0 ||
	    !S_ISREG(config_status.st_mode)) {
		(void) close(lock_descriptor);
		gerror_warn(_FAILED_SAVE_FILE, ccfile, TRUE, FALSE);
		return;
	}
	do {
		lock_result = flock(lock_descriptor, LOCK_EX);
	} while (lock_result != 0 && errno == EINTR);
	if (lock_result != 0) {
		(void) close(lock_descriptor);
		gerror_warn(_FAILED_SAVE_FILE, ccfile, TRUE, FALSE);
		return;
	}
	ptr_cfg = fdopen(lock_descriptor, "r+b");
	if (ptr_cfg == NULL) {
		(void) close(lock_descriptor);
		gerror_warn(_FAILED_SAVE_FILE, ccfile, TRUE, FALSE);
		return;
	}
	if (!cfg_recent_settings_dirty)
		(void) sync_recent_settings_from_stream(ptr_cfg);
	failed = fseek(ptr_cfg, 0, SEEK_SET) != 0 ||
	         ftruncate(lock_descriptor, 0) != 0 ||
	         fwrite(&userCfg, sizeof(cfgSet), 1, ptr_cfg) != 1 ||
	         fflush(ptr_cfg) != 0 || fsync(lock_descriptor) != 0;
	if (fclose(ptr_cfg) != 0)
		failed = TRUE;
	if (failed)
		gerror_warn(_FAILED_SAVE_FILE, ccfile, TRUE, FALSE);
	if (!failed)
		cfg_recent_settings_dirty = FALSE;
}
