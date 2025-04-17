/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013, 2014, 2015, 2016, 2017, 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, def.h , is part of tpad.
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
////////////////////////////////////////////////////////////////////////
// 		DEF FILE -- DIRECTIVES
//		TABLE OF CONTENTS
//			SECTION ONE - LIB C MACRO DIRECTIVES
//			SECTION TWO - #DEFINE VALUES DIRECTIVES
//			SECTION THREE - GETTEXT DIRECTIVES  
////////////////////////////////////////////////////////////////////////
//	BEGIN SECTION ONE.   LIB C MACRO DIRECTIVES     	
////////////////////////////////////////////////////////////////////////
// Please, do not edit this section unless
// you know what you are doing, why you are doing it. :)
// Also, please grep the entire source for usage. 
////////////////////////////////////////////////////////////////////////
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#ifndef  _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED
#endif
#ifndef __USE_FILE_OFFSET64
#define __USE_FILE_OFFSET64
#endif
#ifndef _FILE_OFFSET_BITS 
#define _FILE_OFFSET_BITS 64
#endif
#ifndef _FORTIFY_SOURCE
#define _FORTIFY_SOURCE 2
#endif
#ifndef __USE_FORTIFY_LEVEL
#define __USE_FORTIFY_LEVEL 2
#endif
#if __STDC_VERSION__ < 199901L
	# if __GNUC__ >= 2
		#  define __func__ __FUNCTION__
     	# else
		#  define __func__ "<unknown>"
     	# endif
#endif
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif
////////////////////////////////////////////////////////////////////////
//	END OF SECTION ONE.   	
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
#ifndef  TPAD_STACKS_MAX
#define _TPAD_STACKS_MAX 9999
#endif
#ifndef UNDO_MAX
#define UNDO_MAX 3000
#endif
#ifndef THREAD_MAX 
#define THREAD_MAX 8
#endif
#ifndef _QUANTITY_OF_STACKS_DEFAULT
#define _QUANTITY_OF_STACKS_DEFAULT 1000
#endif
#ifndef _INT_VAL_MAX
#define _INT_VAL_MAX 2147483647
#endif
#ifndef _DESKTOP_FILE_NAME
#define _DESKTOP_FILE_NAME "tpad.desktop"
#endif
#ifndef WIDTH_MAX
#define WIDTH_MAX 3920
#endif
#ifndef HEIGHT_MAX
#define HEIGHT_MAX 3920
#endif
#ifndef UNDOMAX
#define UNDOMAX 1000
#endif
#ifndef _DEFAULT_WINDOW_HEIGHT
#define _DEFAULT_WINDOW_HEIGHT 900
#endif
#ifndef _DEFAULT_WINDOW_WIDTH
#define _DEFAULT_WINDOW_WIDTH 800
#endif
#ifndef WATCH_PROC_SLEEP_LENGTH
#define WATCH_PROC_SLEEP_LENGTH 1
#endif
#ifndef WATCH_STALL_TIME
#define WATCH_STALL_TIME 2
#endif
#ifndef _DISABLE_FILE_WRAPPING_DEFAULT_STATE
#define _DISABLE_FILE_WRAPPING_DEFAULT_STATE 0
#endif
#ifndef CONFIG_FILE_SUFFIX
#define CONFIG_FILE_SUFFIX "/.tpad.cfg"
#endif
#ifndef _TPAD_CFG_WIDTH_MIN
	#define _TPAD_CFG_WIDTH_MIN 200
#endif
#ifndef _TPAD_CFG_HEIGHT_MIN
	#define _TPAD_CFG_HEIGHT_MIN 300
#endif

#ifndef APP_ID_TPAD
#define APP_ID_TPAD SD_ID128_MAKE(5c,a0,93,f0,38,81,48,7b,8b,dd,b8,e2,56,ca,aa,a2)
#endif

////////////////////////////////////////////////////////////////////////
//   DO NOT EDIT BELOW THIS LINE    //
//___________________________________
// EDITING ALLOWED ABOVE THIS LINE //
#ifndef _FILE_IS_A_DIRECTORY
#define _FILE_IS_A_DIRECTORY 1
#endif
#ifndef  _FILE_IS_A_LINK
#define _FILE_IS_A_LINK 2
#endif
#ifndef _ERROR_FILE_GET_TYPE 
#define _ERROR_FILE_GET_TYPE 3
#endif
#ifndef _FILE_IS_A_REGRULAR_FILE
#define _FILE_IS_A_REGRULAR_FILE 4
#endif
#ifndef _FILE_IS_A_EXE_FILE
#define _FILE_IS_A_EXE_FILE 5
#endif
#ifndef _FILE_IS_OF_UNKOWN_TYPE
#define _FILE_IS_OF_UNKOWN_TYPE 6
#endif

  #ifndef POS_SHOW_LINE_NUMBERS
    #define POS_SHOW_LINE_NUMBERS 0
  #endif

  #ifndef POS_SHOW_LANG
    #define POS_SHOW_LANG 1
  #endif

  #ifndef POS_LINE_WRAP
    #define POS_LINE_WRAP 2
  #endif

  #ifndef POS_SHOW_SPELLING_ERRORS 
   #define POS_SHOW_SPELLING_ERRORS 3
  #endif

  #ifndef POS_SHOW_FULL_PATH
    #define POS_SHOW_FULL_PATH 4
  #endif

 #ifndef POS_BOM8
     #define POS_BOM8 5
 #endif

  #ifndef AUTO_TAB_TOGGLE
    #ifdef POS_AUTO_TAB  
      #define POS_AUTO_TAB 6
    #endif
  #endif

#ifndef POS_USE_OPEN_GUARD
  #define POS_USE_OPEN_GUARD 7
#endif 

//UPDATE THIS IF YOU ADD MORE

#ifndef POS_CFG_MAX_USED_VALUE
  #define POS_CFG_MAX_USED_VALUE 7
#endif
//

// EDITING ALLOWED BELOW THIS LINE //
//___________________________________
// DO NOT EDIT ABOVE THIS LINE //
////////////////////////////////////////////////////////////////////////
//	END OF SECTION TWO.        	
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//	BEGIN SECTION THREE.        	
////////////////////////////////////////////////////////////////////////
#ifndef _FONT
#define _FONT gettext("Choose Font")
#endif
#ifndef _LINE_NUMBER
#define _LINE_NUMBER gettext("Show Line Numbers")
#endif
#ifndef _LANGUAGE
#define _LANGUAGE gettext("Show Language")
#endif
#ifndef _AUTOTAB
#define _AUTOTAB gettext("Auto Indent")
#endif
#ifndef _SPELLING
#define _SPELLING gettext("Spell Check")
#endif
#ifndef _SHOW_FULL_PATH
#define  _SHOW_FULL_PATH gettext("Show Full File Path")
#endif
#ifndef _UI_SETTINGS
#define _UI_SETTINGS gettext("Display UI Settings")
#endif
#ifndef _DEFAULT_WIDTH
#define _DEFAULT_WIDTH gettext("Default Window Width")
#endif
#ifndef _DEFAULT_HEIGHT
#define _DEFAULT_HEIGHT gettext("Default Window Height")
#endif
#ifndef _UNDO_LEVEL
#define _UNDO_LEVEL gettext("Set Maximum Undo Level (0 For Unlimited) :")
#endif
#ifndef _FIND_FIND_AND_REPLACE
#define _FIND_FIND_AND_REPLACE gettext("Find / Find & Replace Text")
#endif
#ifndef _FIND_AND_REPLACE
#define _FIND_AND_REPLACE gettext(_FIND_FIND_AND_REPLACE)
#endif
#ifndef _SEARCH
#define _SEARCH_FOR gettext("Search Text For:")
#endif
#ifndef _ERROR_POPUP_TITLE
#define _ERROR_POPUP_TITLE gettext("Error!")
#endif
#ifndef _REPLACE_WITH
#define _REPLACE_WITH gettext("Replace Text With:")
#endif
#ifndef _FIND
#define _FIND gettext("Find")
#endif
#ifndef _HELP_MENU
#define _HELP_MENU gettext("Help")
#endif
#ifndef _ABOUT_MENU
#define _ABOUT_MENU gettext("About")
#endif
#ifndef _REPLACE
#define _REPLACE gettext("Replace")
#endif
#ifndef _SAVE_CHANGES
#define _SAVE_CHANGES gettext("Text was modified!\nSave it?")
#endif
#ifndef _RELOAD_AND_DROP_UNSAVED_CHANGES
#define _RELOAD_AND_DROP_UNSAVED_CHANGES gettext("Reload & Drop any Unsaved Changes")
#endif
#ifndef _YES
#define _YES gettext("Yes")
#endif
#ifndef _NO
#define _NO gettext("No")
#endif
#ifndef _CANCEL
#define _CANCEL gettext("Cancel")
#endif
#ifndef _ON_DISK_CHANGES
#define _ON_DISK_CHANGES gettext("WARNING!\nThe on disk content of the open file was modified while the file was open!\nSaving your changes now may result in data loss.\nSave the file using a different, unique, file name to avoid this risk.\n" )
#endif
#ifndef _SAVE_FILE_ANYWAY
#define _SAVE_FILE_ANYWAY gettext("Save File Anyway")
#endif
#ifndef _OPEN_FILE_SAVE_AS_INSTEAD
#define _OPEN_FILE_SAVE_AS_INSTEAD gettext("Do a Save As... instead")
#endif
#ifndef _SPELL_LANG
#define _SPELL_LANG gettext("en_US")
#endif
#ifndef _KEEP_WIND_ABOVE
#define _KEEP_WIND_ABOVE gettext("Keep Window Above")
#endif
#ifndef _LINE_WP
#define _LINE_WP gettext("Wrap Lines")
#endif
#ifndef _CHECK_ON_OPEN
#define _CHECK_ON_OPEN gettext("Use Temp and  Check For (lock/tmp/swp)")
#endif
#ifndef _SETTINGS_USE_MAGIC
#define _SETTINGS_USE_MAGIC gettext("Use Libmagic")
#endif
#ifndef _ABT_COMMENT
#define _ABT_COMMENT gettext("That simple, fast, and user friendly text editor you love.")
#endif
#ifndef _TRANSLATOR_CREDITS
#define _TRANSLATOR_CREDITS gettext("translator-credits")
#endif
#ifndef _CAN_NOT_READ_FILE
#define _CAN_NOT_READ_FILE gettext("Reading file failed!")
#endif
#ifndef _CONVERT_FAILED
#define _CONVERT_FAILED gettext("Conversion to UTF-8 FAILED! Is this a text document?")
#endif
#ifndef _FILE_MENU
#define _FILE_MENU gettext("_File")
#endif
#ifndef _NEW_FILE_MNEMONIC
#define _NEW_FILE_MNEMONIC gettext("_New File")
#endif
#ifndef _SAVE_FILE_MNEMONIC
#define _SAVE_FILE_MNEMONIC gettext("_Save File")
#endif
#ifndef _OPEN_FILE_MNEMONIC
#define _OPEN_FILE_MNEMONIC gettext("_Open File")
#endif
#ifndef _SAVE_FILE_AS_MNEMONIC
#define _SAVE_FILE_AS_MNEMONIC gettext("S_ave File As...")
#endif
#ifndef _QUIT_MNEMONIC
#define _QUIT_MNEMONIC gettext("_Quit")
#endif
#ifndef _EDIT_MENU
#define _EDIT_MENU gettext("Edit")
#endif
#ifndef _FIND_MNEMONIC
#define _FIND_MNEMONIC gettext("F_ind")
#endif
#ifndef _REPLACE_MNEMONIC
#define _REPLACE_MNEMONIC gettext("Re_place")
#endif
#ifndef _UNDO_MNEMONIC
#define _UNDO_MNEMONIC gettext("_Undo")
#endif
#ifndef _REDO_MNEMONIC
#define _REDO_MNEMONIC gettext("_Redo")
#endif
#ifndef _TOOLRELOAD_FILE
#define _TOOLRELOAD_FILE gettext("Reload")
#endif
#ifndef _RELOAD_MNEMONIC
#define _RELOAD_MNEMONIC gettext("R_eload")
#endif
#ifndef _SETTINGS_MENU
#define _SETTINGS_MENU gettext("Settings")
#endif
#ifndef _SELECT_FONT_MNEMONIC
#define _SELECT_FONT_MNEMONIC gettext("Se_lect Font")
#endif
#ifndef _PREF_MNEMONIC
#define _PREF_MNEMONIC gettext("_Preferences")
#endif
#ifndef _CCFP_MNEMONIC
#define _CCFP_MNEMONIC gettext("Copy F_ile Path")
#endif
#ifndef _FAILED_SAVE_FILE
#define _FAILED_SAVE_FILE gettext("WARNING: Saving File Failed! Unable to write to file: ")
#endif
#ifndef _SAVE_FILE
#define _SAVE_FILE gettext("Save File")
#endif
#ifndef _NEW_THREAD_FAILED
#define _NEW_THREAD_FAILED gettext("Thread could not be created")
#endif
#ifndef _WORD_COUNT
#define _WORD_COUNT gettext("Word Count")
#endif
#ifndef _WORD_COUNT_HEADING
#define _WORD_COUNT_HEADING gettext("Word Count:")
#endif
#ifndef _CHAR_COUNT_HEADING
#define _CHAR_COUNT_HEADING gettext("Char Count:")
#endif
#ifndef _LINE_COUNT_HEADING
#define _LINE_COUNT_HEADING gettext("Line Count:")
#endif
#ifndef _REVERSE_ALL_TEXT
#define _REVERSE_ALL_TEXT gettext("Reverse")
#endif
#ifndef _MAKE_UPPERCASE
#define _MAKE_UPPERCASE gettext("To upper case ")
#endif
#ifndef _BAD_CFG_REPLACE_FAIL
#define _BAD_CFG_REPLACE_FAIL gettext("Config file is invalid.\nFailed to remove bad config file.\nPlease manually remove it.")
#endif
#ifndef _SPELLING_FAILED_INIT
#define _SPELLING_FAILED_INIT gettext("Spell-checking is not working correctly. I have turned it off.\nTechnical information about the error:\n\nFailed to initialize spell checking.\ngtk_spell_checker_set_language returned with error\nError Message:\t")
#endif
#ifndef _CFG_FILE_INVALID
#define _CFG_FILE_INVALID gettext("Config file is invalid.")
#endif
#ifndef _REMOVED_INVALID_CFG_FILE
#define _REMOVED_INVALID_CFG_FILE gettext("Successfully removed bad config file.\nA new config file will be created.\nPress ok and tpad will restart...")
#endif
#ifndef _ERROR_SETTING_SETTING
#define _ERROR_SETTING_SETTING gettext("Error settings value out of range, change dropped.\nThe change will not be saved.\n However, you REALLY should save your work and exit tpad right now. Please report bug")
#endif
#ifndef _ERROR_TOO_SMALL
#define _ERROR_TOO_SMALL gettext("Error! The default window size must have a height of 50 or more and a width of 25 or more.")
#endif
#ifndef _MAKE_LOWERCASE
#define _MAKE_LOWERCASE gettext("To lower case")
#endif
#ifndef _FILE_MAY_BE_ALREADY_OPEN
#define _FILE_MAY_BE_ALREADY_OPEN gettext("File may alread be open in tpad or another program!")
#endif
#ifndef _FILE_MAY_BE_ALREADY_OPEN_DETAIL
#define _FILE_MAY_BE_ALREADY_OPEN_DETAIL gettext("Do you still want to open the file?\n------------------------------------\n\nDescription - File has a swap or temp file.\nPossible Causes --\n\n\t*File might be in-use already.\n\t*tpad or another program crashed or exited improperly while using this file.\n\t*other unknown reasons\n\nDetails --\n\nFile path:%s\n\nPath to tmp or swp file:%s\n")
#endif
#ifndef _ERROR_MOD_DOC_METHOD_VAL
#define _ERROR_MOD_DOC_METHOD_VAL gettext("Error invalid method. An error has occurred! Please save you work and exit tpad. Please report a bug!") 
#endif
#ifndef _ERROR_MAGIC_BAD
#define _ERROR_MAGIC_BAD gettext("File Is Not  A Text File -- BAD MAGIC")
#endif
#ifndef _ERROR_MAGIC_INIT
#define _ERROR_MAGIC_INIT gettext("Oh-No! I was unable to initialize magic the magic library\nSkipping magic!")
#endif
#ifndef _ERROR_MAGIC_DB
#define _ERROR_MAGIC_DB gettext("Uh-oh! I can not load the magic database\nSkipping!")
#endif
#ifndef _ERROR_STR_REV
#define _ERROR_STR_REV gettext("Document text changes have failed!\n\tcoult not convert the new content to UTF-8.\n")
#endif
#ifndef _QUIT_TOOLBAR
#define _QUIT_TOOLBAR gettext("Quit")
#endif
#ifndef _REDO_TOOLBAR
#define _REDO_TOOLBAR gettext("Redo")
#endif
#ifndef _UNDO_TOOLBAR
#define _UNDO_TOOLBAR gettext("Undo")
#endif
#ifndef _SAVE_AS_TOOLBAR
#define _SAVE_AS_TOOLBAR gettext("Save As...")
#endif
#ifndef _SAVE_TOOLBAR
#define _SAVE_TOOLBAR gettext("Save")
#endif
#ifndef _OPEN_TOOLBAR
#define _OPEN_TOOLBAR gettext("Open")
#endif
#ifndef _NEW_TOOLBAR
#define _NEW_TOOLBAR gettext("New")
#endif
#ifndef _TPAD_PANGO_FONT_DESCRIPTION_DEFAULT
#define _TPAD_PANGO_FONT_DESCRIPTION_DEFAULT gettext("DejaVu Sans Mono 12")
#endif
#ifndef _HASH_MENU_CB_TO_512
#define _HASH_MENU_CB_TO_512 gettext("Hash Selection To Clipboard [SHA-512]")
#endif
#ifndef _HASH_MENU_CB_TO_256
#define _HASH_MENU_CB_TO_256 gettext("Hash Selection To Clipboard [SHA-256]")
#endif
#ifndef _HASH_MENU_CB_TO_MD5
#define _HASH_MENU_CB_TO_MD5 gettext("Hash Selection To Clipboard [MD5]")
#endif
#ifndef _HASH_MENU
#define _HASH_MENU gettext("Hash")
#endif
#ifndef _FILE_WATCH_DISABLE
#define _FILE_WATCH_DISABLE gettext("Disable experimental file watching")
#endif
#ifndef _HASH_MENU_F_TO_512
#define _HASH_MENU_F_TO_512 gettext("Hash File To Clipboard [SHA-512]...")
#endif
#ifndef _HASH_MENU_F_TO_256
#define _HASH_MENU_F_TO_256 gettext("Hash File To Clipboard [SHA-256]...")
#endif
#ifndef _HASH_MENU_F_TO_MD5
#define _HASH_MENU_F_TO_MD5 gettext("Hash File To Clipboard [MD5]...")
#endif
#ifndef _CHECKED_FILE_ALREADY_EXISTS_
#define _CHECKED_FILE_ALREADY_EXISTS_ 0
#endif
#ifndef _CHECKED_FILE_DID_NOT_EXIST_AND_WAS_CREATED_
#define _CHECKED_FILE_DID_NOT_EXIST_AND_WAS_CREATED_ 1
#endif
#ifndef _CHECKED_FILE_UNABLE_TO_CHECK_FILE_ERROR_
#define _CHECKED_FILE_UNABLE_TO_CHECK_FILE_ERROR_ -1
#endif
#ifndef _CHECKED_FILE_SERIOUS_ERROR_
#define _CHECKED_FILE_SERIOUS_ERROR_ -2
#endif
#ifndef _HASH_MENU_F_TO_MD5
#define _HASH_MENU_F_TO_MD5 gettext("Hash File To Clipboard [MD5]...")
#endif
#ifndef _HASH_MENU_F_TO_MD5
#define _HASH_MENU_F_TO_MD5 gettext("Hash File To Clipboard [MD5]...")
#endif
#ifndef _ENABLE_FILE_WATCH_MESSAGE_HEADER
#define _ENABLE_FILE_WATCH_MESSAGE_HEADER gettext("Experimental File Watching Toggled")
#endif
#ifndef _MENU_CB_TO_BASE_64
#define _MENU_CB_TO_BASE_64 gettext("Convert Selection To Clipboard [TO BASE 64]")
#endif
#ifndef _MENU_F_TO_BASE_64
#define _MENU_F_TO_BASE_64 gettext("Convert File To Clipboard [TO BASE 64]...")
#endif
#ifndef _MENU_CB_FROM_BASE_64
#define _MENU_CB_FROM_BASE_64 gettext("Convert Selection To Clipboard [FROM BASE 64]")
#endif
#ifndef _MENU_F_FROM_BASE_64
#define _MENU_F_FROM_BASE_64 gettext("Convert File To Clipboard [FROM BASE 64]...")
#endif
#ifndef _ENABLE_FILE_WATCH_MESSAGE_BODY
#define _ENABLE_FILE_WATCH_MESSAGE_BODY gettext("You should save any work and EXIT tpad after enabling or disabling this feature. Failure to do so may result in problems. You have been warned.")
#endif

////////////////////////////////////////////////////////////////////////
//	END OF SECTION THREE.        	
////////////////////////////////////////////////////////////////////////
