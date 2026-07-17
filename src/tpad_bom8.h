/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2019 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_bom8.h , is part of tpad.
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

#ifndef TPAD_BOM8_H
#define  TPAD_BOM8_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "tpad_headers.h"

gint tpad_string_has_bom8(gchar** string);
gint tpad_string_has_bom8_len(const gchar *string, gsize length);
gchar *tpad_utf8_bom_decode(const gchar *bytes, gsize length);
gchar *tpad_utf8_bom_encode(const gchar *utf8, gsize length,
	                         gsize *output_length);

#endif
