/*********************************************************************************
 *     COPYRIGHT NOTICE:
 *     Copyright © 2014, 2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_hash.c , is part of tpad.
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
#ifndef _Space_
#define _Space_ 32
#endif

extern GtkSourceBuffer *mBuff;

char *curBin2sha512(){
	char bbuffer[BUFSIZ];
  	readlink("/proc/self/exe", bbuffer, BUFSIZ);
	return ( (char *)str2sha512( tpad_hash_read_in_file (bbuffer) ) );
}

char *curTxtbuff2sha512(){
	GtkTextIter start,end;
	gtk_text_buffer_get_bounds( GTK_TEXT_BUFFER(mBuff),&start,&end);
	return ( (char *)str2sha512( gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff),&start,&end,TRUE) ) );
	}

char *curFile2sha512(){

	if (tpad_fp_get_current() != NULL){ return ( g_strdup((char *)str2sha512( tpad_hash_read_in_file (tpad_fp_get_current()) )) );
	}
	else return ("--\n'\0'");
	}

char *file2sha512(){
	return ( (char *) g_strdup(str2sha512(tpad_hash_choose_file_and_get_contents())));

}
char *file2sha256(){
	return ( (char *) g_strdup(str2sha256(tpad_hash_choose_file_and_get_contents())));
}
char *file2md5(){
	return ( (char *) g_strdup(str2md5(tpad_hash_choose_file_and_get_contents())));
}
char *file2base64(){
	return( (char *) g_strdup( str2base64((char *)tpad_hash_choose_file_and_get_contents())) );
}
char *filefrombase64(){
	return( (char *) g_strdup(strFrombase64((char *) tpad_hash_choose_file_and_get_contents())) );
}
char *str2sha512(const char *str) {
	if(str == NULL) return("--\n'\0'");

	if(strlen(str) <=1) return("--\n'\0'");

	// SHA-512 produces a 64-byte (512-bit) hash
	unsigned char *ichr = (unsigned char*)malloc(64 * sizeof(unsigned char));
	if (ichr == NULL) return("--\n'\0'");

	mbedtls_sha512(str, strlen(str), ichr, 0);

	char *result = g_strdup(data_to_hex(ichr));
	free(ichr);
	return result;
}
char *str2sha256(const char *str) {
	if(str == NULL) return("--\n'\0'");
	if(strlen(str) <=1) return("--\n\0");

	// SHA-256 produces a 32-byte (256-bit) hash
	unsigned char *ichr = (unsigned char*)malloc(32 * sizeof(unsigned char));
	if (ichr == NULL) return("--\n'\0'");

	mbedtls_sha256(str, strlen(str), ichr, 0);

	char *result = g_strdup(data_to_hex(ichr));
	free(ichr);
	return result;
}
char *str2md5(const char *str) {
	if(str == NULL) return("--\n'\0'");

	if(strlen(str) <=1) return("--\n'\0'");

	// MD5 produces a 16-byte (128-bit) hash
	unsigned char *ichr = (unsigned char*)malloc(16 * sizeof(unsigned char));
	if (ichr == NULL) return("--\n'\0'");

	mbedtls_md5(str, strlen(str), ichr);

	char *result = g_strdup(data_to_hex(ichr));
	free(ichr);
	return result;
}

char* tpad_hash_read_in_file(char* fp){

 if(access(fp, R_OK ) == -1) return("--\n'\0'");
 else {
  FILE * pFile;
  unsigned int lSize;

  size_t result;

  pFile = fopen ( fp , "rb" );
  if (pFile==NULL) return("--\n'\0'");

  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);

  char* buffer = (char*) calloc (lSize+1, sizeof(char));
   if (buffer == NULL) return("--\n'\0'");
  //char  buffer[lSize+1];
  result = fread (buffer,1,lSize,pFile);
  if (result != lSize) {
    free(buffer);
    return("--\n'\0'");
  }

  fclose (pFile);
  return (buffer);
 }
}
char *strFrombase64(const char *str) {
	if(str == NULL) return("--\n'\0'");

	if(strlen(str) <=1) return("--\n'\0'");

	size_t ssize = strlen(str) + 1;
	size_t dlen=0,olen=0;
	int ret = 0;

	// First call to get required buffer size
	ret = mbedtls_base64_decode(NULL, 0, &dlen, str, ssize-1);
	if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
		return("--\n'\0'");
	}

	// Allocate buffer with appropriate size
	unsigned char *ichr = (unsigned char*)malloc((dlen * 3) * sizeof(unsigned char));
	if (ichr == NULL) return("--\n'\0'");

	// Actual decoding
	ret = mbedtls_base64_decode(ichr, dlen, &olen, str, ssize-1);

	char *result;
	switch(ret) {
		case MBEDTLS_ERR_BASE64_INVALID_CHARACTER:
			free(ichr);
			return(str2base64(str));

		case MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL:
			free(ichr);
			return("Too much!\nSelect less / a smaller file and try again.\n");

		case 0:
			// Ensure null-termination
			ichr[olen] = '\0';
			result = g_strdup((char*)ichr);
			free(ichr);
			return result;

		default:
			free(ichr);
			return("--\n");
	}
}
char *str2base64(const char *str) {
	if(str == NULL) return("--\n'\0'");

	if(strlen(str) <=1) return("--\n'\0'");

	size_t ssize = strlen(str) + 1;

	size_t dlen=0, olen=0;

	// First call to get required buffer size
	int ret = mbedtls_base64_encode(NULL, ssize, &dlen, (const unsigned char*)str, ssize);
	if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
		return("--\n'\0'");
	}

	// Allocate buffer with appropriate size
	unsigned char *ichr = (unsigned char*)malloc((dlen + 1) * sizeof(unsigned char));
	if (ichr == NULL) return("--\n'\0'");

	// Actual encoding
	ret = mbedtls_base64_encode(ichr, dlen, &olen, (const unsigned char*)str, ssize);

	char *result;
	if(ret == 0) {
		// Ensure null-termination
		ichr[olen] = '\0';
		result = g_strdup((char*)ichr);
		free(ichr);
		return result;
	}

	free(ichr);
	if (ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
		return("Too much!\nSelect less / a smaller file and try again.\n");
	}

	return("--\n");
}
char* tpad_hash_get_file(){
    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new("File hash to clipboard",GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         "Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         "Hash File",
                                         GTK_RESPONSE_ACCEPT,NULL);
    gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER(dialog),TRUE);
    gtk_file_chooser_set_show_hidden (GTK_FILE_CHOOSER(dialog),TRUE);
    gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER(dialog),FALSE);
	if(tpad_fp_get_current()){
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), (gchar*)get_currentfile_basename());
        gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(dialog),(const gchar *)tpad_fp_get_current());
	}
		char *ptrStr=NULL;


	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_CANCEL) {
		return(NULL);
	}

	if (gtk_dialog_run(GTK_DIALOG(dialog)) ==  GTK_RESPONSE_ACCEPT)
	{
	ptrStr = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	if(ptrStr == NULL) return("'\0'");

	}



	gtk_widget_destroy(GTK_WIDGET(dialog));
	return(g_strdup(ptrStr));

}
char *data_to_hex(void *pdata) {
	if(pdata == NULL) return("--\n");

	unsigned char* data = (unsigned char*) pdata;

	// For SHA-512: 64 bytes, SHA-256: 32 bytes, MD5: 16 bytes
	// Determine the hash size based on the first few bytes
	size_t hash_size = 0;

	// Check if it's SHA-512 (64 bytes)
	if (data[0] != 0 || data[63] != 0) {
		hash_size = 64; // SHA-512
	} else if (data[0] != 0 || data[31] != 0) {
		hash_size = 32; // SHA-256
	} else {
		hash_size = 16; // MD5 or other
	}

	// Allocate buffer for hex output (each byte becomes 2 hex chars + null terminator)
	char *out = (char*)malloc((hash_size * 2 + 1) * sizeof(char));
	if (out == NULL) return("--\n");

	// Convert to hex
	for (int n = 0; n < hash_size; ++n) {
		if ((unsigned int)data[n] <= 255) {
			snprintf(&(out[n*2]), 3, "%02x", (unsigned int)data[n]);
		}
	}

	// Ensure null termination
	out[hash_size * 2] = '\0';

	char *result = g_strdup(out);
	free(out);
	return result;
}
char* tpad_hash_choose_file_and_get_contents(){
	return(g_strdup(tpad_hash_read_in_file(tpad_hash_get_file())));
}



