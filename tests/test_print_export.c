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

#ifdef TPAD_USE_GTK3
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
#else
	gtk_init();
	gtk_source_init();
	window = gtk_window_new();
#endif
	mBuff = gtk_source_buffer_new(NULL);
	view = GTK_SOURCE_VIEW(gtk_source_view_new_with_buffer(mBuff));
#ifdef TPAD_USE_GTK3
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(view));
#else
	gtk_window_set_child(GTK_WINDOW(window), GTK_WIDGET(view));
#endif
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(mBuff),
		"Tpad print integration test\n\n"
		"This page is rendered by GtkSourcePrintCompositor.\n", -1);

	success = tpad_print_export_for_test(argv[1]);
#ifdef TPAD_USE_GTK3
	gtk_widget_destroy(window);
#else
	gtk_window_destroy(GTK_WINDOW(window));
#endif
	g_object_unref(mBuff);
#ifndef TPAD_USE_GTK3
	gtk_source_finalize();
#endif
	window = NULL;
	mBuff = NULL;
	view = NULL;

	return success ? 0 : 1;
}
