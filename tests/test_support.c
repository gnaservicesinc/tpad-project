#include "tpad_headers.h"

GtkWidget *window = NULL;
GtkSourceBuffer *mBuff = NULL;
GtkSourceView *view = NULL;
gboolean searchCase = FALSE;
gboolean doCOVT = FALSE;
unsigned int sdone = 0;
GtkWidget *findentry = NULL;
GtkWidget *replaceentry = NULL;
GtkTextIter match_start;
GtkTextIter match_end;
gchar *content = NULL;
gchar *origfile = NULL;
int madetemp = 0;

gchar *tpad_fp_get_current(void)
{
	return NULL;
}

gint gerror_openguard_popup(gchar *file_name, gchar *found_file_path)
{
	(void) file_name;
	(void) found_file_path;
	return 0;
}

void gerror_warn(const gchar *message, const gchar *extra_information,
                 gboolean log_error, gboolean abort_program)
{
	(void) message;
	(void) extra_information;
	(void) log_error;
	(void) abort_program;
	g_assert_not_reached();
}
