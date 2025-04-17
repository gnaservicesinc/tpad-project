/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_file.c , is part of tpad.
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
extern GtkWidget *window;
extern gboolean save_locked;
extern char* tpad_fp;
extern int tpad_fp_state;

static int tpad_file_handel_dir_open(char* filepath);

static int doesFileExist(char *filepath);

static gchar *cRpath=NULL;

int tpad_touch_check_file(char* fp)
{
	int ir=-1;
	int ifexist = doesFileExist(clean_path(fp));
	switch (ifexist)
	{
	case 0:
		ir=1;
		//
		FILE *fFile = fopen(clean_path(fp), "ab+");
		if(fFile == NULL) return(-1);
		else fclose(fFile);

	break;
	case 1:
		ir=0;
	break;

	case -1:
		ir=-1;
	break;
	default:
		ir=-2;
	break;
	}
return((int) ir);
}
size_t tpad_get_file_size(char* filepath)
{

GError *tpad_get_file_size_error = NULL;
gsize *lSize = 0;
gchar *FContents_ptr=NULL;
gsize *bytes_written=0;
GError *coEvErr;

	gboolean bGetFileContentsReturnStatus= g_file_get_contents ( (const gchar *) g_filename_from_utf8 (filepath,-1,NULL,bytes_written,&coEvErr),&FContents_ptr,lSize,&tpad_get_file_size_error);

	g_free(FContents_ptr);

	if (coEvErr != NULL){
		print(g_strdup(coEvErr->message));
		// g_error_free(coEvErr);
		return(0);
	}
	if(tpad_get_file_size_error != NULL){
		print(g_strdup(tpad_get_file_size_error->message));
		// g_error_free(tpad_get_file_size_error);
		return(0);
	}

	if (bGetFileContentsReturnStatus){
		return(*lSize);
	}
	else {
	   return(0);
	}


}

gchar *get_currentfile_basename(){
	gchar *file_base_name=strrchr(g_strescape(tpad_fp_get_current(),"\\\?\'\""), '/');
	return(file_base_name ? file_base_name+1 : tpad_fp_get_current());
}

gchar *get_currentfile_dirname(){
	return(g_strdup(dirname(tpad_fp_get_current())));
}

void tpad_copy_file_name_to_clipboard(GtkWidget *caller){
	static char* cFile;
	cFile = (char*) g_strdup(tpad_fp_get_current());
	if (cFile != NULL){
		//fprintf(stdout,"Current File = %s\n",cFile);
 	gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),cFile ,strlen(cFile));
	gtk_clipboard_store (gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

	}

}

gchar *clean_path(gchar *path){
	GError *error = NULL;
	gchar *buf_word=NULL;
	gchar *bwords=NULL;
	bwords=g_strdup(path);
	gint bwordsize=str_size(bwords);
	if (bwordsize >= (gint)0)
		{
		buf_word=g_strdup(g_convert(bwords, (gint) -1, (gchar*)"UTF-8", g_get_codeset(),NULL, NULL, &error));
		if (error != NULL)
			{
			print(g_strdup(error->message));
			// g_error_free(error);
			return(NULL);
			}
		else return(g_strdup(buf_word));
		}
	return(NULL);
}

gchar* link_resolve(gchar* file)
{
	gint path_max=0;
           #ifdef PATH_MAX
             path_max = PATH_MAX;
           #else
             path_max = pathconf(path, _PC_PATH_MAX);
             if (path_max < 4096) path_max = 4096;
           #endif

		 gchar* c_rpath=NULL;

		c_rpath=g_file_read_link((const gchar *) g_strdup(file),NULL);





	if(c_rpath == NULL)
	{
		static gchar *OutputError=NULL;
		if (errno){
		OutputError=g_strconcat("Error resolving file path for:\n",file,"Reason:\n",g_strdup(strerror(errno)),"\n", NULL);
		}
		else{
		OutputError=g_strconcat("Error resolving file path for:\n",file,"Reason:\nUNK ERROR\n",NULL);
		}
		gerror_warn(OutputError,"file_system.c->link_resolve->realpath: c_rpath == NULL",TRUE,FALSE);
		return(NULL);
	}
	else {
		GError *bw_error=NULL;
		gsize *bytes_written=0;

		gchar* gc_bu_st_return_utf8 = (gchar*) g_strdup( g_filename_to_utf8(c_rpath,-1,NULL,bytes_written,&bw_error) );

		if(bw_error != NULL) {
		print(g_strdup(bw_error->message));
		// g_error_free(bw_error);
		return(NULL);
		}

		return(gc_bu_st_return_utf8);
	}
return(NULL);
}



gint get_file_type( gchar *file){

gsize *bytes_written = 0;
	GError *convert_error;

	if( g_file_test(g_filename_from_utf8 (file,-1,NULL,bytes_written,&convert_error),G_FILE_TEST_IS_SYMLINK) ) return(_FILE_IS_A_LINK);

	else if( g_file_test(g_filename_from_utf8 (file,-1,NULL,bytes_written,&convert_error),G_FILE_TEST_IS_DIR) ) return(_FILE_IS_A_DIRECTORY);

	else if( g_file_test(g_filename_from_utf8 (file,-1,NULL,bytes_written,&convert_error),G_FILE_TEST_IS_EXECUTABLE
) ) return(_FILE_IS_A_EXE_FILE);

	else if( g_file_test(g_filename_from_utf8 (file,-1,NULL,bytes_written,&convert_error),G_FILE_TEST_IS_REGULAR
) ) return(_FILE_IS_A_REGRULAR_FILE);

	else if( g_file_test(g_filename_from_utf8 (file,-1,NULL,bytes_written,&convert_error),G_FILE_TEST_EXISTS
) ) return(_FILE_IS_OF_UNKOWN_TYPE);

	else return (_ERROR_FILE_GET_TYPE);

/*
	struct stat buf;
	if (g_stat(file, &buf) != 0) {
			 print(g_strconcat("ERROR  Getting Information on file:  ",file,"\n",NULL));
			return(ERR);
          }
	else{
			if (S_ISLNK(buf.st_mode)) return(LNK);
		    else if(S_ISDIR(buf.st_mode)) return(DIR);
			else if(S_ISREG(buf.st_mode)) return(REG);
			else return(ERR);
	}
*/
}

gchar* getcRpath(void){
	if(cRpath!=NULL) return(g_strdup(cRpath));
	else return((gchar*)NULL);
}
 gchar* check_file(gchar *afile)
{
 static gchar* file=NULL;
 static gchar* nfile=NULL;

	file= g_strdup(clean_path( g_strdup(afile)));
	/* Removed not working. [FIXME] */
		/* Bypassing */
		return(g_strdup(file));
		/* Bypassing */
	/* Removed Code Start
switch(get_file_type(file))
	{

	case _FILE_IS_A_LINK:
			return(check_file(link_resolve( g_strdup(file))));
	break;
	case _FILE_IS_A_EXE_FILE:
		return((gchar*) NULL);
	break;

	case _FILE_IS_OF_UNKOWN_TYPE:
		return((gchar*) g_strdup(file));
	break;
	case _FILE_IS_A_DIRECTORY:

			cRpath=g_strdup(file);

			//tpad_file_handel_dir_open(file);
			return((gchar*) NULL);
			break;
	case _FILE_IS_A_REGRULAR_FILE:
				return(g_strdup(file));
			break;
	case _ERROR_FILE_GET_TYPE:
						// Holding spot for now
			return(g_strdup(file));
			break;
	default:
							// Holding spot for now
			return(g_strdup(file));
			break;

	}
	 Removed Code End */

}

static int tpad_file_handel_dir_open(char* filepath) {
 GError *dir_open_error;
GDir * gCdir = (GDir*) g_dir_open(filepath,0,&dir_open_error);
char* returned_name = NULL;

if (dir_open_error != NULL){
			gerror_warn(dir_open_error->message,"ERROR while opening a directory. The following information might be useful.\n",TRUE,FALSE);
			// g_error_free (dir_open_error);

			return(-1);
		}

do{

	returned_name = g_strdup(g_dir_read_name(gCdir));

	if (returned_name == NULL){
		 if(errno) continue;
		 else break;
	}

	int iFileResult = get_file_type(returned_name);

	if(iFileResult == _FILE_IS_A_REGRULAR_FILE) {
		if (check_magic(returned_name) == 1) {
			tpad_spawn_command(returned_name);
		}

	}



} while(returned_name != NULL);

g_dir_close(gCdir);

return(1);
}

static int doesFileExist(char *filepath) {
	if (filepath == NULL) return(-1);
	if (strlen(filepath) <= 1) return (-1);
	struct stat st;
	int result = stat(filepath, &st);
   	if(result == 0) return (1);
   	else if (errno == ENOENT) return(0);
   	else return (-1);


}
