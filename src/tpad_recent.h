/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2026 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_recent.h, is part of tpad.
 *
 *   tpad is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 ********************************************************************************/

#ifndef TPAD_RECENT_H
#define TPAD_RECENT_H

#include "include.h"

#define TPAD_RECENT_FILES_DEFAULT 100
#define TPAD_RECENT_FILES_MAXIMUM 1000000

/* Returned paths are ordered newest first.  Free the array with
 * g_ptr_array_unref(). */
GPtrArray *tpad_recent_files_load(GError **error);

/* When enabled is FALSE this deliberately performs no I/O.  A maximum of
 * zero means that the list is unlimited. */
gboolean tpad_recent_files_add(const gchar *path, gboolean enabled,
                               guint maximum, GError **error);
gboolean tpad_recent_files_trim(guint maximum, GError **error);

#endif
