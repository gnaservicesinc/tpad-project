/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013, 2014 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_headers.c , is part of tpad.
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
#ifndef _TPAD_HEADERS_H
#define _TPAD_HEADERS_H

#include "def.h"

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "include.h"

#ifndef _EXT_CONTENT_DEC_
#define _EXT_CONTENT_DEC_
extern gchar *content;
#endif


#include "malloc.h"
#include "tpad_logo.h"
#include "main.h"
#include "tpad_main.h"
#include "tpad_string.h"
#include "error_warn.h"
#include "tpad_codeset.h"
#include "tpad_config.h"
#include "tpad_buff.h"
#include "tpad_file.h"
#include "tpad_control.h"
#include "tpad_new_file.h"
#include "tpad_open_file.h"
#include "tpad_unity.h"
#include "tpad_show_file.h"
#include "tpad_quit.h"
#include "tpad_find.h"
#include "tpad_about.h"
#include "tpad_find_and_replace.h"
#include "tpad_font.h"
#include "tpad_print.h"
#include "tpad_ui.h"
#include "tpad_title.h"
#include "tpad_find.h"
#include "tpad_spawn.h"
#include "tpad_textstats.h"
#include "tpad_pref.h"
#include "tpad_toggle.h"
#include "tpad_save.h"
#include "tpad_spelling.h"
#include "tpad_reload.h"
#include "tpad_history.h"
#include "tpad_hconnect.h"
#include "tpad_doc.h"
#include "tpad_open_guard.h"
#include "tpad_fp.h"
#include "tpad_hash.h"
#include "tpad_tree.h"
#include "tpad_lang.h"
#include "tpad_bom8.h"
#include "tpad_copy.h"
#include "tpad_id.h"
#include "tpad_frequency_analysis.h"
#include "mbedtls/check_config.h"
#include "mbedtls/config.h"
#include "mbedtls/md5.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/platform_time.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "mbedtls/base64.h"
#endif
