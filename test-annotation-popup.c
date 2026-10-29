// SPDX-License-Identifier: GPL-2.0-or-later
#include <adwaita.h>
#include <glib/gstdio.h>
#include <papers-document.h>
#include <papers-view.h>

static gboolean loaded;

static void
annots_loaded (PpsAnnotationsContext *context, gpointer data)
{
	(void) context;
	(void) data;
	loaded = TRUE;
}

static void
wait_loaded (void)
{
	gint64 deadline = g_get_monotonic_time () + 10 * G_TIME_SPAN_SECOND;
	while (!loaded && g_get_monotonic_time () < deadline) {
		while (g_main_context_iteration (NULL, FALSE))
			;
		g_usleep (1000);
	}
	g_assert_true (loaded);
}

static PpsDocument *
open_document (const char *uri)
{
	g_autoptr (GError) error = NULL;
	PpsDocument *document = pps_document_factory_get_document (uri, &error);
	g_assert_no_error (error);
	g_assert_nonnull (document);
	g_assert_true (pps_document_load (document, uri, &error));
	g_assert_no_error (error);
	return g_object_ref_sink (document);
}

static GtkWidget *
open_popup (PpsAnnotation *annot, GtkWidget **parent)
{
	*parent = gtk_window_new ();
	PpsView *view = pps_view_new ();
	gtk_window_set_child (GTK_WINDOW (*parent), GTK_WIDGET (view));
	pps_view_open_annotation_editor (view, annot);
	/* Drain the deferred present before testing an explicit close. */
	while (g_main_context_iteration (NULL, FALSE))
		;
	GtkWidget *popup = g_object_get_data (G_OBJECT (annot), "popup");
	g_assert_nonnull (popup);
	g_assert_true (gtk_widget_get_visible (popup));
	return popup;
}

int
main (int argc, char **argv)
{
	g_assert_true (argc == 2 || argc == 3);
	const char *contents = argc == 3 ? argv[2] : "popup state regression";
	g_assert_true (pps_init ());
	adw_init ();
	g_autoptr (GError) error = NULL;
	g_autofree char *dir = g_dir_make_tmp ("papers-popup-XXXXXX", &error);
	g_assert_no_error (error);
	g_autofree char *path = g_build_filename (dir, "copy.pdf", NULL);
	g_autoptr (GFile) source = g_file_new_for_path (argv[1]);
	g_autoptr (GFile) copy = g_file_new_for_path (path);
	g_assert_true (g_file_copy (source, copy, G_FILE_COPY_NONE, NULL, NULL, NULL, &error));
	g_assert_no_error (error);
	g_autofree char *uri = g_file_get_uri (copy);
	g_autoptr (PpsDocument) document = open_document (uri);
	PpsAnnotationModel *annotation_model = pps_annotation_model_new ();
	PpsDocumentModel *model = g_object_new (PPS_TYPE_DOCUMENT_MODEL,
	                                      "annotation-model", annotation_model, NULL);
	PpsUndoContext *undo = pps_undo_context_new (model);
	PpsAnnotationsContext *context = pps_annotations_context_new (model, undo);
	g_signal_connect (context, "annots-loaded", G_CALLBACK (annots_loaded), NULL);
	pps_document_model_set_document (model, document);
	wait_loaded ();
	GListModel *annots = pps_annotations_context_get_annots_model (context);
	guint initial_count = g_list_model_get_n_items (annots);
	g_autoptr (PpsAnnotation) annot = NULL;
	if (argc == 3) {
		for (guint i = 0; i < initial_count; i++) {
			g_autoptr (PpsAnnotation) candidate = g_list_model_get_item (annots, i);
			if (g_strcmp0 (pps_annotation_get_contents (candidate), contents) == 0) {
				annot = g_object_ref (candidate);
				break;
			}
		}
	} else {
		PpsPoint point = { 40, 40 };
		GdkRGBA color = { 1, 1, 0, 1 };
		annot = g_object_ref (pps_annotations_context_add_annotation_sync (
		    context, 0, PPS_ANNOTATION_TYPE_TEXT, &point, &point, &color, NULL));
		pps_annotation_set_contents (annot, contents);
		PpsRectangle rectangle = { 50, 50, 250, 200 };
		g_object_set (annot, "has-popup", TRUE, "rectangle", &rectangle, NULL);
	}
	g_assert_nonnull (annot);
	PpsAnnotationMarkup *markup = PPS_ANNOTATION_MARKUP (annot);
	pps_annotation_markup_set_popup_is_open (markup, TRUE);

	GtkWidget *parent;
	GtkWidget *popup = open_popup (annot, &parent);

	/* Scrolling the page away only hides the window temporarily. */
	pps_document_set_modified (document, FALSE);
	gtk_widget_set_visible (popup, FALSE);
	g_assert_true (pps_annotation_markup_get_popup_is_open (markup));
	g_assert_false (pps_document_get_modified (document));
	gtk_widget_set_visible (popup, TRUE);

	/* The explicit close request must update the PDF state as well. */
	gtk_window_close (GTK_WINDOW (popup));
	g_assert_false (gtk_widget_get_visible (popup));
	g_assert_false (pps_annotation_markup_get_popup_is_open (markup));
	g_assert_true (pps_document_get_modified (document));
	g_assert_cmpstr (pps_annotation_get_contents (annot), ==, contents);

	g_autoptr (PpsJob) save = pps_job_save_new (document, uri, uri);
	pps_job_run (save);
	g_assert_true (pps_job_is_succeeded (save, &error));
	g_assert_no_error (error);
	gtk_window_destroy (GTK_WINDOW (popup));
	gtk_window_destroy (GTK_WINDOW (parent));

	/* Reopening must preserve the note and its closed popup. */
	g_autoptr (PpsDocument) reopened = open_document (uri);
	loaded = FALSE;
	pps_document_model_set_document (model, reopened);
	wait_loaded ();
	g_assert_cmpuint (g_list_model_get_n_items (annots), ==, initial_count + (argc == 2));
	gboolean found = FALSE;
	for (guint i = 0; i < g_list_model_get_n_items (annots); i++) {
		g_autoptr (PpsAnnotation) saved = g_list_model_get_item (annots, i);
		if (g_strcmp0 (pps_annotation_get_contents (saved), contents) != 0)
			continue;
		found = TRUE;
		g_assert_false (pps_annotation_markup_get_popup_is_open (PPS_ANNOTATION_MARKUP (saved)));
		/* The saved note can still be explicitly opened and closed again. */
		popup = open_popup (saved, &parent);
		g_assert_true (pps_annotation_markup_get_popup_is_open (PPS_ANNOTATION_MARKUP (saved)));
		gtk_window_close (GTK_WINDOW (popup));
		g_assert_false (gtk_widget_get_visible (popup));
		g_assert_false (pps_annotation_markup_get_popup_is_open (PPS_ANNOTATION_MARKUP (saved)));
		gtk_window_destroy (GTK_WINDOW (popup));
		gtk_window_destroy (GTK_WINDOW (parent));
	}
	g_assert_true (found);
	g_assert_cmpint (g_remove (path), ==, 0);
	g_assert_cmpint (g_rmdir (dir), ==, 0);
	g_print ("PASS: temporary hide preserves open state; explicit close survives save/reopen with note contents intact\n");
	return 0;
}
