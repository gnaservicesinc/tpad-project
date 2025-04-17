/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Parts of this document © 2015, 2016 Stack Exchange Inc
 *     cp function Copyright © 2010 "caf" http://stackoverflow.com/users/134633/caf?tab=profile and was published on stackoverflow at the URL http://stackoverflow.com/questions/2180079/how-can-i-copy-a-file-on-unix-using-c and released is under the (CC BY-SA 3.0 US) https://creativecommons.org/licenses/by-sa/3.0/us/  
 *
 * 	THIS IS NO WAY MEANS "caf", stackoverflow, or any one else has in any way shape or form endorsed tpad, GNA Services Inc. Andrew Smith or anyone or any thing else or the use of the work.
 *  
 *   cp function may be copied and used under the (CC BY-SA 3.0 US) https://creativecommons.org/licenses/by-sa/3.0/us/. Any changes made to it, if any, are also released under the (CC BY-SA 3.0 US) https://creativecommons.org/licenses/by-sa/3.0/us/
 *
 *     All other parts of this document are Copyright © 2015 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 * 
 *   This file, tpad_copy.c , is part of tpad.
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

extern gchar* path_temp_file;
volatile int imadeit;

void* imadeit_set();
void* imadeit_unset();
int imadeit_value();

static int cp(const char *to, const char *from);

gint tpad_copy(char* source, char* dest){
	if( tpad_touch_check_file(dest) < 0 ) return(-1);

	if(tpad_touch_check_file(source) < 0) return(-1);
	 
	int iRetun = (gint) cp( (const char*) clean_path(source), clean_path(dest)); 
	imadeit_set();
	return(iRetun);
}

void* tpad_cp_remove_temp(){

	if ( imadeit_value() == 1 ){
		if(path_temp_file){
			if(strlen(path_temp_file ) > 1 ){
				if(access(path_temp_file, W_OK ) != -1) {
					if (remove(path_temp_file) == 0) {
					imadeit_unset();
					}
				}
			}
		}
	}
}
static int cp(const char *to, const char *from)
{
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0)
        return -1;

    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return 0;
    }

  out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
    return -1;
}
int imadeit_value(){
	return( (int) imadeit );
}
void* imadeit_set(){
	imadeit=1;
}
void* imadeit_unset(){
	imadeit=0;
}