/********************************************************************************* 
 *     COPYRIGHT NOTICE:
 *     Copyright © 2013 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
 *     All Rights Reserved.
 *
 *   This file, tpad_save.c , is part of tpad.
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
#include <glib/gstdio.h>
extern GtkSourceBuffer *mBuff;
extern gboolean save_locked;
extern GtkWidget *window;
extern int tpad_wach_thread_set;
extern gboolean disable_file_watch;
extern short iforce_save;
extern gchar *origfile;
extern int madetemp;

static void set_file_error(GError **error, const gchar *operation,
                           const gchar *path, int error_number)
{
	if (error == NULL || *error != NULL)
		return;

	g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(error_number),
	            "%s '%s': %s", operation, path, g_strerror(error_number));
}

static gboolean write_all(int fd, const gchar *data, gsize length)
{
	gsize offset = 0;

	while (offset < length) {
		ssize_t written = write(fd, data + offset, length - offset);
		if (written > 0) {
			offset += (gsize) written;
			continue;
		}
		if (written < 0 && errno == EINTR)
			continue;
		if (written == 0)
			errno = EIO;
		return FALSE;
	}

	return TRUE;
}

/* Write beside the destination and rename only after the complete file is on
 * disk.  Resolving an existing symlink once means the temporary-file rename
 * updates its target without ever opening a swapped symlink for truncation. */
static gboolean atomic_replace_file(const gchar *path, const gchar *data,
                                    gsize length, GError **error)
{
	struct stat before;
	struct stat current;
	gchar *target = NULL;
	gchar *directory = NULL;
	gchar *basename = NULL;
	gchar *temporary = NULL;
	gboolean existed = FALSE;
	gboolean temporary_exists = FALSE;
	gboolean success = FALSE;
	int fd = -1;
	int directory_fd = -1;
	int saved_errno;

	if (path == NULL || *path == '\0') {
		set_file_error(error, "Unable to save", "(empty path)", EINVAL);
		return FALSE;
	}

	if (lstat(path, &before) == 0) {
		existed = TRUE;
		if (S_ISLNK(before.st_mode)) {
			char *resolved = realpath(path, NULL);
			if (resolved == NULL) {
				set_file_error(error, "Unable to resolve", path, errno);
				goto cleanup;
			}
			target = g_strdup(resolved);
			free(resolved);
			if (stat(target, &before) != 0) {
				set_file_error(error, "Unable to inspect", target, errno);
				goto cleanup;
			}
		} else {
			target = g_strdup(path);
		}

		if (!S_ISREG(before.st_mode)) {
			set_file_error(error, "Refusing to replace a non-regular file",
			               target, EINVAL);
			goto cleanup;
		}
	} else if (errno == ENOENT) {
		target = g_strdup(path);
	} else {
		set_file_error(error, "Unable to inspect", path, errno);
		goto cleanup;
	}

	directory = g_path_get_dirname(target);
	basename = g_path_get_basename(target);
	temporary = g_strdup_printf("%s/.%s.tpad-XXXXXX", directory, basename);

	fd = g_mkstemp_full(temporary, O_RDWR, existed ? 0600 : 0666);
	if (fd < 0) {
		set_file_error(error, "Unable to create a temporary file for",
		               target, errno);
		goto cleanup;
	}
	temporary_exists = TRUE;
	(void) fcntl(fd, F_SETFD, FD_CLOEXEC);

	if (!write_all(fd, data, length)) {
		set_file_error(error, "Unable to write", target, errno);
		goto cleanup;
	}

	if (fsync(fd) != 0) {
		set_file_error(error, "Unable to synchronize", target, errno);
		goto cleanup;
	}

	/* Do not re-enable setuid/setgid bits on newly written content. */
	if (existed && fchmod(fd, before.st_mode & 0777) != 0) {
		set_file_error(error, "Unable to preserve permissions for", target,
		               errno);
		goto cleanup;
	}

	if (close(fd) != 0) {
		fd = -1;
		set_file_error(error, "Unable to close", target, errno);
		goto cleanup;
	}
	fd = -1;

	/* Detect replacement of the destination while the temporary file was
	 * written.  rename() itself never follows the final path as a symlink. */
	if (existed) {
		if (lstat(target, &current) != 0) {
			set_file_error(error, "Unable to inspect", target, errno);
			goto cleanup;
		}
		if (!S_ISREG(current.st_mode) || current.st_dev != before.st_dev ||
		    current.st_ino != before.st_ino ||
		    current.st_size != before.st_size ||
		    current.st_mtime != before.st_mtime ||
		    current.st_mtim.tv_nsec != before.st_mtim.tv_nsec ||
		    current.st_ctime != before.st_ctime ||
		    current.st_ctim.tv_nsec != before.st_ctim.tv_nsec) {
			saved_errno = EAGAIN;
			set_file_error(error, "File changed while saving", target,
			               saved_errno);
			goto cleanup;
		}
	} else if (lstat(target, &current) == 0) {
		set_file_error(error, "File appeared while saving", target, EEXIST);
		goto cleanup;
	} else if (errno != ENOENT) {
		set_file_error(error, "Unable to inspect", target, errno);
		goto cleanup;
	}

	if (rename(temporary, target) != 0) {
		set_file_error(error, "Unable to replace", target, errno);
		goto cleanup;
	}
	temporary_exists = FALSE;

	/* Best effort directory synchronization.  Some filesystems do not
	 * support fsync on a directory, but the completed rename is still valid. */
	directory_fd = open(directory, O_RDONLY);
	if (directory_fd >= 0) {
		(void) fcntl(directory_fd, F_SETFD, FD_CLOEXEC);
		(void) fsync(directory_fd);
		(void) close(directory_fd);
		directory_fd = -1;
	}

	success = TRUE;

cleanup:
	saved_errno = errno;
	if (fd >= 0)
		(void) close(fd);
	if (directory_fd >= 0)
		(void) close(directory_fd);
	if (temporary_exists)
		(void) unlink(temporary);
	g_free(temporary);
	g_free(basename);
	g_free(directory);
	g_free(target);
	errno = saved_errno;
	return success;
}

static gboolean save_buffer_to_path(const gchar *path)
{
	GtkTextIter start;
	GtkTextIter end;
	gchar *utf8;
	gchar *converted;
	gchar *output;
	gsize utf8_length;
	gsize text_offset;
	gsize converted_length = 0;
	gsize output_length;
	GError *error = NULL;
	const gchar *codeset;
	gboolean has_bom;
	gboolean saved;

	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(mBuff), &start, &end);
	utf8 = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(mBuff), &start, &end,
	                                FALSE);
	utf8_length = strlen(utf8);
	has_bom = utf8_length >= 3 &&
	          (guchar) utf8[0] == 0xef && (guchar) utf8[1] == 0xbb &&
	          (guchar) utf8[2] == 0xbf;
	text_offset = has_bom ? 3 : 0;
	converted = NULL;
	if (cfg_ut8bom()) {
		output = tpad_utf8_bom_encode(utf8, utf8_length, &output_length);
	} else {
		(void) g_get_charset(&codeset);
		converted = g_convert(utf8 + text_offset, utf8_length - text_offset,
		                      codeset, "UTF-8", NULL, &converted_length,
		                      &error);
		output = converted;
		output_length = converted_length;
	}
	if (output == NULL) {
		gerror_warn(error != NULL ? error->message : _CONVERT_FAILED,
		            _CONVERT_FAILED, TRUE, FALSE);
		g_clear_error(&error);
		g_free(utf8);
		return FALSE;
	}

	saved = atomic_replace_file(path, output, output_length, &error);
	if (!saved) {
		gerror_warn(_FAILED_SAVE_FILE,
		            error != NULL ? error->message : path, TRUE, FALSE);
		g_clear_error(&error);
	}

	g_free(output);
	g_free(utf8);
	return saved;
}

static gboolean paths_refer_to_same_file(const gchar *left,
                                         const gchar *right)
{
	struct stat left_stat;
	struct stat right_stat;
	char *left_resolved;
	char *right_resolved;
	GFile *left_file;
	GFile *right_file;
	gboolean equal;

	if (left == NULL || right == NULL)
		return FALSE;
	if (stat(left, &left_stat) == 0 && stat(right, &right_stat) == 0 &&
	    left_stat.st_dev == right_stat.st_dev &&
	    left_stat.st_ino == right_stat.st_ino)
		return TRUE;

	left_resolved = realpath(left, NULL);
	right_resolved = realpath(right, NULL);
	if (left_resolved != NULL && right_resolved != NULL) {
		equal = strcmp(left_resolved, right_resolved) == 0;
		free(left_resolved);
		free(right_resolved);
		return equal;
	}
	free(left_resolved);
	free(right_resolved);

	left_file = g_file_new_for_path(left);
	right_file = g_file_new_for_path(right);
	equal = g_file_equal(left_file, right_file);
	g_object_unref(left_file);
	g_object_unref(right_file);
	return equal;
}

gint save_watched(void){
return(-1);


}

gint tpad_save_ondisk_change_detected(void) {

	// returns 1 if file save should be forced

	gint response=0;
	GtkWidget *dialog;

		dialog=gtk_message_dialog_new(GTK_WINDOW(window),
							GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_QUESTION,
							GTK_BUTTONS_NONE,
							_ON_DISK_CHANGES);
		gtk_dialog_add_button(GTK_DIALOG(dialog),_RELOAD_AND_DROP_UNSAVED_CHANGES,2);
        	gtk_dialog_add_button(GTK_DIALOG(dialog),_SAVE_FILE_ANYWAY,1);
        	gtk_dialog_add_button(GTK_DIALOG(dialog),_OPEN_FILE_SAVE_AS_INSTEAD,0);
		gtk_dialog_add_button(GTK_DIALOG(dialog), _CANCEL,
		                      GTK_RESPONSE_CANCEL);
		response=gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);



		switch (response)
			{

			case 1:
				return 1;

			case 0:
				return save_as() ? 2 : -1;
			case 2:
			{
				gchar *current = tpad_fp_get_current();
				new_thread_tpad(current);
				g_free(current);
				force_quit_program();
				break;
			}
			default:
				return(-1);
			break;

			}
	
	return(response);
}
gint save_modified(void){
        gint response=0;
	GtkWidget *dialog;	
	if(gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(mBuff))==TRUE) 
	{
		
       	 	dialog= (GtkWidget *) gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,GTK_BUTTONS_NONE,_SAVE_CHANGES);
	
        	gtk_dialog_add_button(GTK_DIALOG(dialog),_YES,1);
        	gtk_dialog_add_button(GTK_DIALOG(dialog),_NO,0);
		gtk_dialog_add_button(GTK_DIALOG(dialog), _CANCEL,
		                      GTK_RESPONSE_CANCEL);
	
        	response=gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);

		if(response == 0) return(1);
		if(response == 1) return save_file() ? 1 : 0;

		/* Cancel, delete-event, and every unexpected response keep the
		 * document open. */
		return(0);
	}
  return(1);
}
gboolean save_file(void) {
	gchar *path = tpad_fp_get_current();
	gint decision;

	if (path == NULL)
		return save_as();

	if (tpad_control_compare_stored_file_hash_to_current_ondisk_file_hash() != 0) {
		decision = tpad_save_ondisk_change_detected();
		if (decision == 2) {
			g_free(path);
			return TRUE;
		}
		if (decision != 1) {
			g_free(path);
			return FALSE;
		}
	}

	if (!save_buffer_to_path(path)) {
		g_free(path);
		return FALSE;
	}
	g_free(path);

	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(mBuff),FALSE);
	set_title();
	set_language();
	tpad_control_store_hash_of_current_file_set();
	return TRUE;
}

	

gboolean save_as(void){
	GtkWidget *dialog;
	gchar *current;
	gchar *selected = NULL;
	gint decision = 1;
	gboolean selected_is_current = FALSE;


    dialog = gtk_file_chooser_dialog_new(_SAVE_FILE,GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         _CANCEL,
                                         GTK_RESPONSE_CANCEL,
                                         _SAVE_TOOLBAR,
                                         GTK_RESPONSE_ACCEPT,NULL);
    gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER(dialog),TRUE);
    gtk_file_chooser_set_show_hidden (GTK_FILE_CHOOSER(dialog),TRUE);
    gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER(dialog),TRUE);
	current = tpad_fp_get_current();
	if(current != NULL){
		gchar *basename = g_path_get_basename(current);
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), basename);
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), current);
		g_free(basename);
	}
	else 
	{
		gchar *recent_path = getcRpath();
		const gchar *folder = recent_path != NULL ? recent_path : g_get_home_dir();

		if (folder != NULL)
			gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), folder);
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),
		                                  _FALLBACK_SAVE_FILE_NAME);
		g_free(recent_path);
	}
	
	gboolean saved = FALSE;
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT) {
		selected = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		selected_is_current = selected != NULL &&
		                      paths_refer_to_same_file(selected, current);
		/* Selecting the current file must retain the normal external-change
		 * check.  Save As used to route through save_file() and silently
		 * bypassing that check could overwrite another editor's changes. */
		if (selected_is_current &&
		    tpad_control_compare_stored_file_hash_to_current_ondisk_file_hash() != 0) {
			gtk_widget_hide(dialog);
			decision = tpad_save_ondisk_change_detected();
			if (decision == 2)
				saved = TRUE;
		}
		if (selected != NULL && !selected_is_current && decision == 1 &&
		    cfg_use_open_guard() && !tpad_open_guard_check_path(selected))
			decision = -1;
		if (selected != NULL && decision == 1 &&
		    save_buffer_to_path(selected)) {
			if (!selected_is_current) {
				tpad_open_guard_cleanup();
				madetemp = 0;
				g_clear_pointer(&origfile, g_free);
				if (cfg_use_open_guard() &&
				    tpad_open_guard_track_path(selected)) {
					madetemp = 1;
					origfile = g_strdup(selected);
				}
			}
			tpad_fp_set(selected);
			gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(mBuff),FALSE);
			set_title();
			set_language();
			tpad_control_store_hash_of_current_file_set();
			saved = TRUE;
		}
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	g_free(selected);
	g_free(current);
	return saved;
}
