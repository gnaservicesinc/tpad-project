/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014, 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
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


#ifndef _TPAD_HASHA_H
#define _TPAD_HASHA_H
#include "include.h"
char *str2sha512(const char *str);
char *str2sha256(const char *str);
char *str2md5(const char *str);
char *file2sha256();
char *file2md5();
char *file2base64();
char *filefrombase64();
char *file2sha512();
char* tpad_hash_read_in_file(char* fp);
char *str2base64(const char *str);
char *strFrombase64(const char *str);
char* tpad_hash_get_file();
char *data_to_hex(void* pdata);
char* tpad_hash_choose_file_and_get_contents();
char *curTxtbuff2sha512();
char *curFile2sha512();
char *curBin2sha512();
#endif