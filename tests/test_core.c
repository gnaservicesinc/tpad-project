#include "tpad_headers.h"

#include <glib/gstdio.h>

extern gchar *origfile;
extern int madetemp;

static void test_replace(void)
{
	gchar *result;

	result = tpad_replace_str("one fish, two fish", "fish", "cat");
	g_assert_cmpstr(result, ==, "one cat, two cat");
	g_free(result);

	result = tpad_replace_str("aaaa", "aa", "b");
	g_assert_cmpstr(result, ==, "bb");
	g_free(result);

	result = tpad_replace_str("unchanged", "", "ignored");
	g_assert_cmpstr(result, ==, "unchanged");
	g_free(result);

	result = tpad_replace_str_full("One one ONE", "one", "cat", FALSE);
	g_assert_cmpstr(result, ==, "cat cat cat");
	g_free(result);

	result = tpad_replace_str_full("One one ONE", "one", "cat", TRUE);
	g_assert_cmpstr(result, ==, "One cat ONE");
	g_free(result);

	result = tpad_replace_str_full("a.b A.B", "a.b", "x", FALSE);
	g_assert_cmpstr(result, ==, "x x");
	g_free(result);
}

static void test_text_data_validation(void)
{
	static const gchar text[] = "plain text";
	static const gchar binary[] = { 'a', '\0', 'b' };

	g_assert_true(tpad_string_is_text_data(text, sizeof(text) - 1));
	g_assert_false(tpad_string_is_text_data(binary, sizeof(binary)));
	g_assert_false(tpad_string_is_text_data(NULL, 0));
}

static void test_file_path_boundaries(void)
{
	gchar *directory = g_dir_make_tmp("tpad-path-test-XXXXXX", NULL);
	gchar *filename;
	gchar *utf8_directory;
	gchar *utf8_filename;
	gchar *round_trip;

	g_assert_nonnull(directory);
	utf8_directory = tpad_filename_to_utf8(directory);
	g_assert_nonnull(utf8_directory);
	filename = g_build_filename(directory, "caf\303\251.txt", NULL);
	utf8_filename = tpad_filename_to_utf8(filename);
	g_assert_nonnull(utf8_filename);
	round_trip = tpad_filename_from_utf8(utf8_filename);
	g_assert_cmpstr(round_trip, ==, filename);
	g_assert_cmpint(tpad_touch_check_file(utf8_filename), ==, 1);
	g_assert_cmpint(tpad_touch_check_file(utf8_filename), ==, 0);
	g_assert_cmpint(tpad_touch_check_file(utf8_directory), ==, -1);

	g_assert_cmpint(g_unlink(filename), ==, 0);
	g_assert_cmpint(g_rmdir(directory), ==, 0);
	g_free(round_trip);
	g_free(utf8_filename);
	g_free(utf8_directory);
	g_free(filename);
	g_free(directory);
}

static void test_drop_path_formatting(void)
{
	static const gchar *paths[] = {
		"/tmp/plain.txt",
		"/tmp/with space/image.png",
		"/tmp/caf\303\251",
		"/tmp/a\"quote\\slash"
	};
	static const gchar *with_null[] = { NULL, "/tmp/directory", NULL };
	gchar *formatted;

	formatted = tpad_string_format_drop_paths(paths, G_N_ELEMENTS(paths));
	g_assert_cmpstr(formatted, ==,
	                "\"/tmp/plain.txt\" \"/tmp/with space/image.png\" "
	                "\"/tmp/caf\303\251\" \"/tmp/a\\\"quote\\\\slash\"");
	g_free(formatted);

	formatted = tpad_string_format_drop_paths(with_null,
	                                          G_N_ELEMENTS(with_null));
	g_assert_cmpstr(formatted, ==, "\"/tmp/directory\"");
	g_free(formatted);

	formatted = tpad_string_format_drop_paths(NULL, 0);
	g_assert_cmpstr(formatted, ==, "");
	g_free(formatted);
}

static void test_hash_vectors(void)
{
	gchar *result;

	result = str2sha512("");
	g_assert_cmpstr(result, ==,
		"cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce"
		"47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
	g_free(result);

	result = str2sha256("");
	g_assert_cmpstr(result, ==,
		"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
	g_free(result);

	result = str2md5("");
	g_assert_cmpstr(result, ==, "d41d8cd98f00b204e9800998ecf8427e");
	g_free(result);
}

static void test_binary_file_hash(void)
{
	static const gchar expected[] =
		"48dd66f05b49586e072c9f3485a10982231e246b46fd5eb1765721c855610c5a8"
		"1744d49b1cc7ffeeed783f6819fd3702d659ce14b5b9b4f5d14f2e05cc375b5";
	static const gchar data[] = { 'a', '\0', 'b' };
	gchar *path = NULL;
	gchar *result;
	gint descriptor;

	descriptor = g_file_open_tmp("tpad-hash-test-XXXXXX", &path, NULL);
	g_assert_cmpint(descriptor, >=, 0);
	g_assert_cmpint(close(descriptor), ==, 0);
	g_assert_true(g_file_set_contents(path, data, sizeof(data), NULL));

	result = tpad_hash_file_sha512(path);
	g_assert_cmpstr(result, ==, expected);
	g_free(result);
	g_assert_cmpint(g_unlink(path), ==, 0);
	g_free(path);
}

static void test_base64(void)
{
	gchar *encoded = str2base64("hello");
	gchar *decoded;

	g_assert_cmpstr(encoded, ==, "aGVsbG8=");
	decoded = strFrombase64(encoded);
	g_assert_cmpstr(decoded, ==, "hello");
	g_free(decoded);
	g_free(encoded);
}

static void test_bom(void)
{
	static const gchar bom[] = { (gchar) 0xef, (gchar) 0xbb, (gchar) 0xbf, 'x' };
	static const gchar short_input[] = { (gchar) 0xef, (gchar) 0xbb };
	static const gchar non_ascii_bom[] = {
		(gchar) 0xef, (gchar) 0xbb, (gchar) 0xbf,
		'c', 'a', 'f', (gchar) 0xc3, (gchar) 0xa9
	};
	gchar *original_locale;
	gchar *decoded;
	gchar *encoded;
	gsize encoded_length = 0;

	g_assert_true(tpad_string_has_bom8_len(bom, sizeof(bom)));
	g_assert_false(tpad_string_has_bom8_len(short_input, sizeof(short_input)));
	g_assert_false(tpad_string_has_bom8_len(NULL, 0));

	original_locale = g_strdup(setlocale(LC_CTYPE, NULL));
	g_assert_nonnull(setlocale(LC_CTYPE, "C"));
	decoded = tpad_utf8_bom_decode(non_ascii_bom, sizeof(non_ascii_bom));
	g_assert_cmpstr(decoded, ==, "caf\303\251");
	encoded = tpad_utf8_bom_encode(decoded, strlen(decoded), &encoded_length);
	g_assert_nonnull(encoded);
	g_assert_cmpuint(encoded_length, ==, sizeof(non_ascii_bom));
	g_assert_cmpint(memcmp(encoded, non_ascii_bom,
	                       sizeof(non_ascii_bom)), ==, 0);
	g_free(encoded);
	g_free(decoded);
	if (original_locale != NULL)
		g_assert_nonnull(setlocale(LC_CTYPE, original_locale));
	g_free(original_locale);
}

static void test_copy(void)
{
	static const gchar source_contents[] = "private guard copy\n";
	gchar *source_path = NULL;
	gchar *destination_path;
	gchar *copied_contents = NULL;
	gint descriptor;

	descriptor = g_file_open_tmp("tpad-copy-test-XXXXXX", &source_path, NULL);
	g_assert_cmpint(descriptor, >=, 0);
	g_assert_cmpint(close(descriptor), ==, 0);
	g_assert_true(g_file_set_contents(source_path, source_contents, -1, NULL));
	destination_path = g_strconcat(source_path, ".copy", NULL);

	g_assert_cmpint(tpad_copy(source_path, destination_path), ==, 0);
	g_assert_true(g_file_get_contents(destination_path, &copied_contents,
	                                NULL, NULL));
	g_assert_cmpstr(copied_contents, ==, source_contents);
	/* The destination is deliberately exclusive and cannot be overwritten. */
	g_assert_cmpint(tpad_copy(source_path, destination_path), ==, -1);

	g_free(copied_contents);
	g_assert_cmpint(g_unlink(destination_path), ==, 0);
	g_assert_cmpint(g_unlink(source_path), ==, 0);
	g_free(destination_path);
	g_free(source_path);
}

static void test_open_guard_lifecycle(void)
{
	static const gchar contents[] = "guarded\n";
	gchar *source_path = NULL;
	gchar *guard_path;
	gint descriptor;

	descriptor = g_file_open_tmp("tpad-guard-test-XXXXXX", &source_path, NULL);
	g_assert_cmpint(descriptor, >=, 0);
	g_assert_cmpint(close(descriptor), ==, 0);
	g_assert_true(g_file_set_contents(source_path, contents, -1, NULL));
	guard_path = g_strconcat(source_path, "~", NULL);

	g_assert_true(tpad_open_guard_track_path(source_path));
	g_assert_true(g_file_test(guard_path, G_FILE_TEST_IS_REGULAR));
	tpad_open_guard_cleanup();
	g_assert_false(g_file_test(guard_path, G_FILE_TEST_EXISTS));

	g_assert_cmpint(g_unlink(source_path), ==, 0);
	g_free(guard_path);
	g_free(source_path);
}

static void test_open_guard_runtime_toggle(void)
{
	static const gchar contents[] = "runtime guard\n";
	gchar *source_path = NULL;
	gchar *guard_path;
	gint descriptor;

	descriptor = g_file_open_tmp("tpad-runtime-guard-test-XXXXXX",
	                             &source_path, NULL);
	g_assert_cmpint(descriptor, >=, 0);
	g_assert_cmpint(close(descriptor), ==, 0);
	g_assert_true(g_file_set_contents(source_path, contents, -1, NULL));
	guard_path = g_strconcat(source_path, "~", NULL);

	g_assert_true(tpad_open_guard_apply_enabled(TRUE, source_path));
	g_assert_true(g_file_test(guard_path, G_FILE_TEST_IS_REGULAR));
	g_assert_cmpint(madetemp, ==, 1);
	g_assert_cmpstr(origfile, ==, source_path);

	g_assert_false(tpad_open_guard_apply_enabled(FALSE, source_path));
	g_assert_false(g_file_test(guard_path, G_FILE_TEST_EXISTS));
	g_assert_cmpint(madetemp, ==, 0);
	g_assert_null(origfile);

	/* Refusing an existing guard must roll the setting back without deleting
	 * a file owned by another editor. */
	g_assert_true(g_file_set_contents(guard_path, "external guard\n", -1,
	                                NULL));
	g_assert_false(tpad_open_guard_apply_enabled(TRUE, source_path));
	g_assert_true(g_file_test(guard_path, G_FILE_TEST_IS_REGULAR));
	g_assert_cmpint(madetemp, ==, 0);
	g_assert_null(origfile);
	g_assert_false(tpad_open_guard_apply_enabled(FALSE, source_path));
	g_assert_true(g_file_test(guard_path, G_FILE_TEST_IS_REGULAR));
	g_assert_cmpint(g_unlink(guard_path), ==, 0);

	g_assert_cmpint(g_unlink(source_path), ==, 0);
	g_free(guard_path);
	g_free(source_path);
}

static void test_recent_files(void)
{
	gchar *old_override = g_strdup(g_getenv("TPAD_RECENT_FILE"));
	gchar *directory = g_dir_make_tmp("tpad-recent-test-XXXXXX", NULL);
	gchar *list_path;
	gchar *lock_path;
	gchar *alpha;
	gchar *newline;
	gchar *gamma;
	gchar *before_disabled = NULL;
	gchar *after_disabled = NULL;
	gchar *serialized = NULL;
	GPtrArray *files;
	GError *error = NULL;
	struct stat status;

	g_assert_nonnull(directory);
	list_path = g_build_filename(directory, ".tpad.recent", NULL);
	lock_path = g_strconcat(list_path, ".lock", NULL);
	alpha = g_build_filename(directory, "alpha.txt", NULL);
	newline = g_build_filename(directory, "line\nbreak.txt", NULL);
	gamma = g_build_filename(directory, "gamma.txt", NULL);
	g_assert_true(g_setenv("TPAD_RECENT_FILE", list_path, TRUE));
	g_assert_true(tpad_recent_files_add(alpha, FALSE, 2, &error));
	g_assert_no_error(error);
	g_assert_false(g_file_test(list_path, G_FILE_TEST_EXISTS));
	g_assert_false(g_file_test(lock_path, G_FILE_TEST_EXISTS));

	g_assert_true(tpad_recent_files_add(alpha, TRUE, 2, &error));
	g_assert_no_error(error);
	g_assert_true(tpad_recent_files_add(newline, TRUE, 2, &error));
	g_assert_no_error(error);
	g_assert_true(tpad_recent_files_add(gamma, TRUE, 2, &error));
	g_assert_no_error(error);
	files = tpad_recent_files_load(&error);
	g_assert_no_error(error);
	g_assert_cmpuint(files->len, ==, 2);
	g_assert_cmpstr(g_ptr_array_index(files, 0), ==, gamma);
	g_assert_cmpstr(g_ptr_array_index(files, 1), ==, newline);
	g_ptr_array_unref(files);

	/* A duplicate moves to the front without growing the list. */
	g_assert_true(tpad_recent_files_add(newline, TRUE, 2, &error));
	g_assert_no_error(error);
	files = tpad_recent_files_load(&error);
	g_assert_no_error(error);
	g_assert_cmpuint(files->len, ==, 2);
	g_assert_cmpstr(g_ptr_array_index(files, 0), ==, newline);
	g_assert_cmpstr(g_ptr_array_index(files, 1), ==, gamma);
	g_ptr_array_unref(files);

	/* Disabled recording must perform no recent-list I/O. */
	g_assert_true(g_file_get_contents(list_path, &before_disabled, NULL, NULL));
	g_assert_true(tpad_recent_files_add(alpha, FALSE, 2, &error));
	g_assert_no_error(error);
	g_assert_true(g_file_get_contents(list_path, &after_disabled, NULL, NULL));
	g_assert_cmpstr(before_disabled, ==, after_disabled);

	/* Zero is unlimited and the URI format safely round-trips newlines. */
	g_assert_true(tpad_recent_files_add(alpha, TRUE, 0, &error));
	g_assert_no_error(error);
	files = tpad_recent_files_load(&error);
	g_assert_no_error(error);
	g_assert_cmpuint(files->len, ==, 3);
	g_assert_cmpstr(g_ptr_array_index(files, 0), ==, alpha);
	g_assert_cmpstr(g_ptr_array_index(files, 1), ==, newline);
	g_assert_cmpstr(g_ptr_array_index(files, 2), ==, gamma);
	g_ptr_array_unref(files);
	g_assert_true(g_file_get_contents(list_path, &serialized, NULL, NULL));
	g_assert_nonnull(strstr(serialized, "# Tpad recent files v1\n"));
	g_assert_nonnull(strstr(serialized, "%0A"));

	g_assert_true(tpad_recent_files_trim(1, &error));
	g_assert_no_error(error);
	files = tpad_recent_files_load(&error);
	g_assert_no_error(error);
	g_assert_cmpuint(files->len, ==, 1);
	g_assert_cmpstr(g_ptr_array_index(files, 0), ==, alpha);
	g_ptr_array_unref(files);
	g_assert_cmpint(g_stat(list_path, &status), ==, 0);
	g_assert_cmpuint((guint) status.st_mode & 0777U, ==, 0600U);

	g_free(serialized);
	g_free(after_disabled);
	g_free(before_disabled);
	g_assert_cmpint(g_unlink(list_path), ==, 0);
	g_assert_cmpint(g_unlink(lock_path), ==, 0);
	if (old_override != NULL)
		g_assert_true(g_setenv("TPAD_RECENT_FILE", old_override, TRUE));
	else
		g_unsetenv("TPAD_RECENT_FILE");
	g_assert_cmpint(g_rmdir(directory), ==, 0);
	g_free(gamma);
	g_free(newline);
	g_free(alpha);
	g_free(lock_path);
	g_free(list_path);
	g_free(directory);
	g_free(old_override);
}

static void test_config_recent_files_migration(void)
{
	typedef struct {
		int ibitmask;
		int default_window_width;
		int default_window_height;
		int undo_level;
		int screen_width;
		int screen_height;
	} LegacyConfig;
	const LegacyConfig legacy = {
		(1 << POS_SHOW_FULL_PATH) | (1 << POS_LINE_WRAP),
		777, 888, 12, 1920, 1080
	};
	gchar *old_override = g_strdup(g_getenv("TPAD_CONFIG_FILE"));
	gchar *directory = g_dir_make_tmp("tpad-config-test-XXXXXX", NULL);
	gchar *config_path;
	gchar *stored_contents = NULL;
	gsize stored_length = 0;
	cfgSet external;
	struct stat status;

	g_assert_nonnull(directory);
	config_path = g_build_filename(directory, ".tpad.cfg", NULL);
	g_assert_true(g_file_set_contents(config_path, (const gchar *) &legacy,
	                                sizeof(legacy), NULL));
	g_assert_true(g_setenv("TPAD_CONFIG_FILE", config_path, TRUE));
	config_setup();
	g_assert_cmpint(cfg_wWidth(), ==, 777);
	g_assert_cmpint(cfg_wHeight(), ==, 888);
	g_assert_cmpint(cfg_undo(), ==, 12);
	g_assert_true(cfg_recent_files_enabled());
	g_assert_cmpint(cfg_recent_files_limit(), ==, TPAD_RECENT_FILES_DEFAULT);
	g_assert_cmpint(g_stat(config_path, &status), ==, 0);
	g_assert_cmpuint((guint) status.st_size, ==, (guint) sizeof(cfgSet));

	/* Emulate another GTK process disabling history.  A stale process saving
	 * an unrelated preference must preserve the current on-disk choice. */
	g_assert_true(g_file_get_contents(config_path, &stored_contents,
	                                &stored_length, NULL));
	g_assert_cmpuint(stored_length, ==, sizeof(external));
	memcpy(&external, stored_contents, sizeof(external));
	g_clear_pointer(&stored_contents, g_free);
	external.ibitmask &= ~(1 << POS_RECENT_FILES);
	external.recent_files_limit = 23;
	g_assert_true(g_file_set_contents(config_path, (const gchar *) &external,
	                                sizeof(external), NULL));
	cfg_set_show_lang(TRUE);
	cfg_save();
	g_assert_false(cfg_recent_files_enabled());
	g_assert_cmpint(cfg_recent_files_limit(), ==, 23);

	cfg_set_recent_files_enabled(FALSE);
	cfg_set_recent_files_limit(0);
	cfg_on_exit();
	config_setup();
	g_assert_false(cfg_recent_files_enabled());
	g_assert_cmpint(cfg_recent_files_limit(), ==, 0);
	cfg_on_exit();

	if (old_override != NULL)
		g_assert_true(g_setenv("TPAD_CONFIG_FILE", old_override, TRUE));
	else
		g_unsetenv("TPAD_CONFIG_FILE");
	g_assert_cmpint(g_unlink(config_path), ==, 0);
	g_assert_cmpint(g_rmdir(directory), ==, 0);
	g_free(config_path);
	g_free(directory);
	g_free(old_override);
}

static void test_config_defaults(void)
{
	gchar *old_override = g_strdup(g_getenv("TPAD_CONFIG_FILE"));
	gchar *directory = g_dir_make_tmp("tpad-config-defaults-XXXXXX", NULL);
	gchar *config_path;
	struct stat status;

	g_assert_nonnull(directory);
	config_path = g_build_filename(directory, ".tpad.cfg", NULL);
	g_assert_true(g_setenv("TPAD_CONFIG_FILE", config_path, TRUE));
	config_setup();
	g_assert_true(cfg_line());
	g_assert_cmpint(cfg_undo(), ==, 0);
	cfg_on_exit();
	g_assert_cmpint(g_stat(config_path, &status), ==, 0);
	g_assert_cmpuint((guint) status.st_size, ==, (guint) sizeof(cfgSet));
	g_assert_cmpuint((guint) status.st_mode & 0777U, ==, 0600U);

	if (old_override != NULL)
		g_assert_true(g_setenv("TPAD_CONFIG_FILE", old_override, TRUE));
	else
		g_unsetenv("TPAD_CONFIG_FILE");
	g_assert_cmpint(g_unlink(config_path), ==, 0);
	g_assert_cmpint(g_rmdir(directory), ==, 0);
	g_free(config_path);
	g_free(directory);
	g_free(old_override);
}

int main(int argc, char **argv)
{
	g_test_init(&argc, &argv, NULL);
	g_test_add_func("/tpad/replace", test_replace);
	g_test_add_func("/tpad/text-data-validation", test_text_data_validation);
	g_test_add_func("/tpad/file-path-boundaries", test_file_path_boundaries);
	g_test_add_func("/tpad/drop-path-formatting", test_drop_path_formatting);
	g_test_add_func("/tpad/hash/vectors", test_hash_vectors);
	g_test_add_func("/tpad/hash/binary-file", test_binary_file_hash);
	g_test_add_func("/tpad/base64", test_base64);
	g_test_add_func("/tpad/bom", test_bom);
	g_test_add_func("/tpad/copy", test_copy);
	g_test_add_func("/tpad/open-guard/lifecycle", test_open_guard_lifecycle);
	g_test_add_func("/tpad/open-guard/runtime-toggle",
	                test_open_guard_runtime_toggle);
	g_test_add_func("/tpad/recent-files", test_recent_files);
	g_test_add_func("/tpad/config/recent-files-migration",
	                test_config_recent_files_migration);
	g_test_add_func("/tpad/config/defaults", test_config_defaults);
	return g_test_run();
}
