#include <papers-view.h>
#include <papers-document.h>
static gboolean loaded;
static void on_loaded (PpsAnnotationsContext *ctx, gpointer data) { loaded = TRUE; }
static void wait_loaded (void) {
    gint64 deadline = g_get_monotonic_time () + 10 * G_TIME_SPAN_SECOND;
    while (!loaded && g_get_monotonic_time () < deadline) {
        while (g_main_context_iteration (NULL, FALSE));
        g_usleep (1000);
    }
    g_assert_true (loaded);
}
static PpsDocument *open_doc (const char *uri) {
    GError *error = NULL;
    PpsDocument *doc = pps_document_factory_get_document (uri, &error);
    g_assert_no_error (error);
    g_assert_nonnull (doc);
    g_assert_true (pps_document_load (doc, uri, &error));
    g_assert_no_error (error);
    return g_object_ref_sink (doc);
}
int main (int argc, char **argv) {
    g_assert_cmpint (argc, ==, 2);
    g_assert_true (pps_init ());
    GError *error = NULL;
    g_autofree char *dir = g_dir_make_tmp ("papers-save-regression-XXXXXX", &error);
    g_assert_no_error (error);
    g_autofree char *path = g_build_filename (dir, "copy.pdf", NULL);
    g_autoptr (GFile) source = g_file_new_for_path (argv[1]);
    g_autoptr (GFile) copy = g_file_new_for_path (path);
    g_assert_true (g_file_copy (source, copy, G_FILE_COPY_NONE, NULL, NULL, NULL, &error));
    g_assert_no_error (error);
    g_autofree char *uri = g_file_get_uri (copy);
    PpsAnnotationModel *annotation_model = pps_annotation_model_new ();
    PpsDocumentModel *model = g_object_new (PPS_TYPE_DOCUMENT_MODEL, "annotation-model", annotation_model, NULL);
    PpsUndoContext *undo = pps_undo_context_new (model);
    PpsAnnotationsContext *ctx = pps_annotations_context_new (model, undo);
    g_signal_connect (ctx, "annots-loaded", G_CALLBACK (on_loaded), NULL);
    PpsDocument *doc = open_doc (uri);
    pps_document_model_set_document (model, doc);
    wait_loaded ();
    GListModel *annots = pps_annotations_context_get_annots_model (ctx);
    guint count = g_list_model_get_n_items (annots);
    for (int cycle = 1; cycle <= 3; cycle++) {
        PpsPoint p = { 40, 40 + cycle * 25 };
        GdkRGBA color = { 0, 0, 0, 1 };
        PpsAnnotation *a = g_object_ref (pps_annotations_context_add_annotation_sync (ctx, 0, PPS_ANNOTATION_TYPE_FREE_TEXT, &p, &p, &color, NULL));
        pps_annotation_set_contents (a, "save regression");
        PpsJob *job = pps_job_save_new (doc, uri, uri);
        pps_job_run (job);
        g_assert_true (pps_job_is_succeeded (job, &error));
        g_assert_no_error (error);
        PpsDocument *next = open_doc (uri);
        loaded = FALSE;
        pps_document_model_set_document (model, next);
        wait_loaded ();
        g_assert_cmpuint (g_list_model_get_n_items (annots), ==, count + cycle);
        /* A delayed old-widget removal must not alter the reloaded document. */
        pps_annotations_context_remove_annotation (ctx, a);
        g_assert_false (pps_document_get_modified (next));
        g_assert_cmpuint (g_list_model_get_n_items (annots), ==, count + cycle);
        g_object_unref (a);
        g_object_unref (doc);
        doc = next;
        g_object_unref (job);
    }
    g_assert_true (g_file_delete (copy, NULL, &error));
    g_assert_no_error (error);
    g_autoptr (GFile) directory = g_file_new_for_path (dir);
    g_assert_true (g_file_delete (directory, NULL, &error));
    g_assert_no_error (error);
    g_print ("PASS: three in-place JobSave/reopen cycles; annotation counts and stale-removal protection verified\n");
    return 0;
}
