/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2019 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_bom8.c , is part of tpad.
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



gint bom8heck (int a, int b, int c);

gint tpad_string_has_bom8(gchar** string){
	return ((gint) bom8heck((int)(*string)[0], (int)(*string)[1], (int)(*string)[2]));

}

gint bom8heck (int ia, int ib, int ic){
	if( (int) ia != (int) 239 || (int) ib != (int) 187 || (int) ic != (int) 191) return ((gint) 0);
	else return ((gint) 1);

}