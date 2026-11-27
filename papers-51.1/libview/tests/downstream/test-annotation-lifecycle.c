#include <papers-view.h>
#include <papers-document.h>
#include "pps-annotation-overlay.h"

static const char *fixture;
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
static PpsDocument *load_doc (void) {
    g_autofree char *uri = g_filename_to_uri (fixture, NULL, NULL);
    GError *error = NULL;
    PpsDocument *doc = pps_document_factory_get_document (uri, &error);
    g_assert_no_error (error);
    g_assert_nonnull (doc);
    g_assert_true (pps_document_load (doc, uri, &error));
    g_assert_no_error (error);
    return g_object_ref_sink (doc);
}
static GtkWidget *find_text_view (GtkWidget *widget) {
    if (GTK_IS_TEXT_VIEW (widget)) return widget;
    for (GtkWidget *child = gtk_widget_get_first_child (widget); child; child = gtk_widget_get_next_sibling (child)) {
        GtkWidget *result = find_text_view (child);
        if (result) return result;
    }
    return NULL;
}
static void lifecycle (gconstpointer mode) {
    PpsAnnotationModel *annotation_model = pps_annotation_model_new ();
    PpsDocumentModel *model = g_object_new (PPS_TYPE_DOCUMENT_MODEL, "annotation-model", annotation_model, NULL);
    PpsUndoContext *undo = pps_undo_context_new (model);
    PpsAnnotationsContext *ctx = pps_annotations_context_new (model, undo);
    PpsDocument *doc = load_doc ();
    g_signal_connect (ctx, "annots-loaded", G_CALLBACK (on_loaded), NULL);
    loaded = FALSE;
    pps_document_model_set_document (model, doc);
    wait_loaded ();
    GListModel *annots = pps_annotations_context_get_annots_model (ctx);
    guint initial = g_list_model_get_n_items (annots);
    PpsPoint point = { 30, 30 };
    GdkRGBA color = { 0, 0, 0, 1 };
    PpsAnnotation *annot = g_object_ref (pps_annotations_context_add_annotation_sync (
        ctx, 0, PPS_ANNOTATION_TYPE_FREE_TEXT, &point, &point, &color, NULL));
    g_assert_cmpuint (g_list_model_get_n_items (annots), ==, initial + 1);
    if (g_str_has_prefix (mode, "idle-")) {
        GtkWidget *overlay = g_object_ref_sink (pps_overlay_annotation_entry_new (annot, ctx, model));
        GtkWidget *text = find_text_view (overlay);
        g_assert_nonnull (text);
        GListModel *controllers = gtk_widget_observe_controllers (text);
        gboolean emitted = FALSE;
        for (guint i = 0; i < g_list_model_get_n_items (controllers); i++) {
            GObject *controller = g_list_model_get_item (controllers, i);
            if (GTK_IS_EVENT_CONTROLLER_FOCUS (controller)) {
                g_signal_emit_by_name (controller, "leave");
                g_signal_emit_by_name (controller, "leave");
                emitted = TRUE;
            }
            g_object_unref (controller);
        }
        g_object_unref (controllers);
        g_assert_true (emitted);
        if (g_str_equal (mode, "idle-dispose")) {
            g_object_unref (overlay);
            overlay = NULL;
        } else if (g_str_equal (mode, "idle-refilled")) {
            gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (text)), "keep this annotation", -1);
        } else if (g_str_equal (mode, "idle-removed")) {
            pps_annotations_context_remove_annotation (ctx, annot);
        } else if (g_str_equal (mode, "idle-reload")) {
            PpsDocument *replacement = load_doc ();
            loaded = FALSE;
            pps_document_model_set_document (model, replacement);
            wait_loaded ();
        }
        while (g_main_context_iteration (NULL, FALSE));
        guint expected = (g_str_equal (mode, "idle-dispose") || g_str_equal (mode, "idle-refilled")) ? initial + 1 : initial;
        g_assert_cmpuint (g_list_model_get_n_items (annots), ==, expected);
        if (overlay) g_object_unref (overlay);
    } else if (g_str_equal (mode, "stale")) {
        PpsDocument *replacement = load_doc ();
        loaded = FALSE;
        pps_document_model_set_document (model, replacement);
        wait_loaded ();
        guint count = g_list_model_get_n_items (annots);
        gboolean modified = pps_document_get_modified (replacement);
        pps_annotations_context_remove_annotation (ctx, annot);
        g_assert_cmpuint (g_list_model_get_n_items (annots), ==, count);
        g_assert_cmpint (pps_document_get_modified (replacement), ==, modified);
        g_assert_null (pps_undo_context_get_last_action (undo));
    } else {
        pps_annotations_context_remove_annotation (ctx, annot);
        g_assert_cmpuint (g_list_model_get_n_items (annots), ==, initial);
        gpointer last = pps_undo_context_get_last_action (undo);
        if (g_str_equal (mode, "duplicate")) {
            pps_annotations_context_remove_annotation (ctx, annot);
            g_assert_true (last == pps_undo_context_get_last_action (undo));
        }
        pps_undo_context_undo (undo);
        g_assert_cmpuint (g_list_model_get_n_items (annots), ==, initial + 1);
        pps_undo_context_redo (undo);
        g_assert_cmpuint (g_list_model_get_n_items (annots), ==, initial);
    }
    g_object_unref (annot);
    /* Model/context signal lifetime is outside these short-lived regression cases. */
}
int main (int argc, char **argv) {
    fixture = g_getenv ("PAPERS_TEST_PDF");
    g_assert_nonnull (fixture);
    g_test_init (&argc, &argv, NULL);
    gtk_init ();
    g_assert_true (pps_init ());
    g_test_add_data_func ("/annotations/normal-remove-undo-redo", "normal", lifecycle);
    g_test_add_data_func ("/annotations/stale-after-reload", "stale", lifecycle);
    g_test_add_data_func ("/annotations/duplicate-remove", "duplicate", lifecycle);
    g_test_add_data_func ("/annotations/idle-empty", "idle-empty", lifecycle);
    g_test_add_data_func ("/annotations/idle-dispose", "idle-dispose", lifecycle);
    g_test_add_data_func ("/annotations/idle-refilled", "idle-refilled", lifecycle);
    g_test_add_data_func ("/annotations/idle-removed", "idle-removed", lifecycle);
    g_test_add_data_func ("/annotations/idle-reload", "idle-reload", lifecycle);
    return g_test_run ();
}
