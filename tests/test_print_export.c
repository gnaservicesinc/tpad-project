#include "tpad_headers.h"

extern GtkWidget *window;
extern GtkSourceBuffer *mBuff;
extern GtkSourceView *view;

int main(int argc, char **argv)
{
	gboolean success;

	if (argc != 2) {
		g_printerr("Usage: %s OUTPUT.pdf\n", argv[0]);
		return 2;
	}

	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	mBuff = gtk_source_buffer_new(NULL);
	view = GTK_SOURCE_VIEW(gtk_source_view_new_with_buffer(mBuff));
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(view));
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(mBuff),
		"Tpad print integration test\n\n"
		"This page is rendered by GtkSourcePrintCompositor.\n", -1);

	success = tpad_print_export_for_test(argv[1]);
	gtk_widget_destroy(window);
	g_object_unref(mBuff);
	window = NULL;
	mBuff = NULL;
	view = NULL;

	return success ? 0 : 1;
}
