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
