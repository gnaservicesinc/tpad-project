/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013, 2014, 2015, 2016, 2017 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_spawn.c , is part of tpad.
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

static gchar* path_self=NULL;

static gboolean spawn_tpad(const gchar *file)
{
	GError *error = NULL;
	gchar *argv[3];
	gboolean spawned;

	if (path_self == NULL || *path_self == '\0') {
		g_warning("Unable to start tpad: executable path is not set");
		return FALSE;
	}

	argv[0] = path_self;
	argv[1] = (file != NULL && *file != '\0') ? (gchar *) file : NULL;
	argv[2] = NULL;

	spawned = g_spawn_async(NULL, argv, NULL, G_SPAWN_SEARCH_PATH,
	                        NULL, NULL, NULL, &error);
	if (!spawned) {
		g_warning("Unable to start tpad: %s",
		          error != NULL ? error->message : "unknown error");
		g_clear_error(&error);
	}

	return spawned;
}

void runn(void){
	new_thread_tpad(NULL);
}
void set_path_self_cleanup(void) {
	g_clear_pointer(&path_self, g_free);
}

void set_path_self( gchar *c_self_path){
	g_free(path_self);
	path_self=g_strdup(c_self_path);
}

void *tpad_spawn_command(void *ptr){
	spawn_tpad((const gchar *) ptr);
	return NULL;
}
gint new_thread_tpad (gchar *file){
	return spawn_tpad(file) ? 0 : -1;
}
