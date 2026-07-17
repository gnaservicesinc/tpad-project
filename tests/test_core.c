#include "tpad_headers.h"

#include <glib/gstdio.h>

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
}

static void test_text_data_validation(void)
{
	static const gchar text[] = "plain text";
	static const gchar binary[] = { 'a', '\0', 'b' };

	g_assert_true(tpad_string_is_text_data(text, sizeof(text) - 1));
	g_assert_false(tpad_string_is_text_data(binary, sizeof(binary)));
	g_assert_false(tpad_string_is_text_data(NULL, 0));
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

int main(int argc, char **argv)
{
	g_test_init(&argc, &argv, NULL);
	g_test_add_func("/tpad/replace", test_replace);
	g_test_add_func("/tpad/text-data-validation", test_text_data_validation);
	g_test_add_func("/tpad/hash/vectors", test_hash_vectors);
	g_test_add_func("/tpad/hash/binary-file", test_binary_file_hash);
	g_test_add_func("/tpad/base64", test_base64);
	g_test_add_func("/tpad/bom", test_bom);
	g_test_add_func("/tpad/copy", test_copy);
	g_test_add_func("/tpad/open-guard/lifecycle", test_open_guard_lifecycle);
	return g_test_run();
}
