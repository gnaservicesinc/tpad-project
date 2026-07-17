/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *s
 *   This file, tpad_control.c , is part of tpad.
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
extern gchar* FILE_HASH_ON_LAST_OPEN_OR_SAVE;
extern gchar* FILE_NAME_ON_LAST_OPEN_OR_SAVE;
extern short FILE_HASH_STORED;
gchar* FILE_NAME_ON_LAST_OPEN_OR_SAVE =  (char*) NULL;
gchar* FILE_HASH_ON_LAST_OPEN_OR_SAVE =  (char*) NULL;
short FILE_HASH_STORED = 0;

void tpad_control_cleanup(void)
{
	g_clear_pointer(&FILE_NAME_ON_LAST_OPEN_OR_SAVE, g_free);
	g_clear_pointer(&FILE_HASH_ON_LAST_OPEN_OR_SAVE, g_free);
	FILE_HASH_STORED = 0;
}



short tpad_control_store_hash_of_current_file_set(void) {
	gchar *current = tpad_fp_get_current();
	gchar *hash = NULL;

	if (current != NULL)
		hash = curFile2sha512();

	tpad_control_cleanup();

	/* A valid SHA-512 digest is exactly 128 hexadecimal characters. */
	if (current != NULL && hash != NULL && strlen(hash) == 128) {
		FILE_NAME_ON_LAST_OPEN_OR_SAVE = current;
		FILE_HASH_ON_LAST_OPEN_OR_SAVE = hash;
		FILE_HASH_STORED = 1;
	} else {
		g_free(current);
		g_free(hash);
	}

	return FILE_HASH_STORED;

}

short tpad_control_compare_stored_file_hash_to_current_ondisk_file_hash(void) {
	//Returns 1 if they do not match... implies that file may have been changed while open. 
	// A hash/read failure for a previously tracked path is also a change: the
	// file may have been deleted or made unreadable and must not be silently
	// recreated without the external-change decision.

	gchar *current;
	gchar *current_hash;
	gboolean changed;

	if (!FILE_HASH_STORED || FILE_NAME_ON_LAST_OPEN_OR_SAVE == NULL ||
	    FILE_HASH_ON_LAST_OPEN_OR_SAVE == NULL)
		return 0;

	current = tpad_fp_get_current();
	if (current == NULL)
		return 0;

	if (g_strcmp0(FILE_NAME_ON_LAST_OPEN_OR_SAVE, current) != 0) {
		g_free(current);
		return 0;
	}
	g_free(current);

	current_hash = curFile2sha512();
	if (current_hash == NULL || strlen(current_hash) != 128) {
		g_free(current_hash);
		return 1;
	}
	changed = g_strcmp0(FILE_HASH_ON_LAST_OPEN_OR_SAVE, current_hash) != 0;
	g_free(current_hash);
	return changed ? 1 : 0;

}
