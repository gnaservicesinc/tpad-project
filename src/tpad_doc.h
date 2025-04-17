/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_doc.h , is part of tpad.
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

#ifndef _METHOD_REVERSE
#define _METHOD_REVERSE 0
#endif
#ifndef _METHOD_UPCAP
#define _METHOD_UPCAP 1
#endif
#ifndef _METHOD_DOWNCAP
#define _METHOD_DOWNCAP 2
#endif
#ifndef _METHOD_MAX
#define _METHOD_MAX 2
#endif
#ifndef _METHOD_HEX
#define _METHOD_HEX 3
#endif
#ifndef _TPAD_DOC_
#define _TPAD_DOC_

#include "include.h"
gint mod_doc(int method);
void low_caps_document();
void up_caps_document();
void rev_document();
void hex_document();
#endif
