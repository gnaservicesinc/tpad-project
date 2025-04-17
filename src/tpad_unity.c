/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014,2015 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_unity.c , is part of tpad.
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
//#ifdef HAVE_LIBUNITY
/*
 *  unity integration has been put one ice for now. 
 *  It seems a bit pointless and I see no reason to do it just because
 * 
 */
void ui_unity_init(void) {};
int ui_unity_urgent_on (void) {return(FALSE);}
int ui_unity_urgent_off (void) {return(FALSE);}
void ui_unity_destroy (void) {};
//void ui_unity_init(void){
	/*if (!entry)
		entry = unity_launcher_entry_get_for_desktop_file( _DESKTOP_FILE_NAME );
*/
//}

//void ui_unity_destroy (void)
//{
	/*
if (entry) {
		unity_launcher_entry_set_count_visible (entry, FALSE);
		g_main_context_iteration(NULL, FALSE);
	}
*/
//}
//int ui_unity_urgent_on (void)
//{
	/*
	if (!entry) ui_unity_init();

	
	if (entry) {
			unity_launcher_entry_set_urgent(entry,TRUE);
			return(TRUE);
	}
	else return(FALSE);
	*/
	//return(TRUE);
//}

//int ui_unity_urgent_off (void)
//{
	/*
	if (!entry) ui_unity_init();

	
	if (entry) {
			unity_launcher_entry_set_urgent(entry,FALSE);
			return(TRUE);
	}
	else return(FALSE);
		*/
	//return(TRUE);
//}

//#else
//#endif  /* HAVE_LIBUNITY */