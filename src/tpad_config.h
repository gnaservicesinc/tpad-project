/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013, 2014, 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_config.h , is part of tpad.
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
/ ********************************************************************************/

#ifndef TPAD_CONFIG_H
#define  TPAD_CONFIG_H

#include "tpad_headers.h"
#ifndef _TPAD_CFG_WIDTH_MIN
	#define _TPAD_CFG_WIDTH_MIN 200
#endif
#ifndef _TPAD_CFG_HEIGHT_MIN
	#define _TPAD_CFG_HEIGHT_MIN 300
#endif

typedef struct {
  int ibitmask;
  int defualt_window_width;
  int default_window_height;
  int undo_level;
  int screenWidth;
  int screenHeight;
 // uuid_t configID;
} cfgSet;
void cfg_on_exit();
void cfg_set_auto_tab(int opt);
int cfg_auto_tab();
int cfg_get_default_auto_tab(char *cfg_file);
void config_setup();
void cfg_save();
void cfg_set_use_open_guard(int opt);
void cfg_set_screen_width(int opt);
void cfg_set_screen_height(int opt);
void cfg_set_default_width(int opt);
void cfg_set_default_height(int opt);
void cfg_set_show_lang(int opt);
void cfg_set_undo(int opt);
void cfg_set_show_spelling(int opt);
void cfg_set_show_line(int opt);
void cfg_set_show_full_path(int opt);
void cfg_set_show_line_wrap(int opt);
//void cfg_set_use_magic(int opt);
void cfg_set_use_ut8bom(int opt);
int cfg_line();
int cfg_line_wrap();
int cfg_show_full_path();
int cfg_spell();
int cfg_sWidth();
int cfg_sHeight();
int cfg_wWidth();
void get_cfg_mask_int(gchar* description);
//char *cfg_id();
int cfg_ut8bom();
int cfg_wHeight();
int cfg_undo();
int cfg_gaveUp();
int cfg_use_open_guard();
int cfg_lang();
int cfg_use_magic();
int cfg_use_stacks();
int cfg_stack_quantity();
int tpad_config_write_out();
int is_userCfg_valid();
#endif
