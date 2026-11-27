// SPDX-License-Identifier: GPL-2.0-or-later
#include <adwaita.h>
#include <cairo-pdf.h>
#include <glib/gstdio.h>
#include "pps-pixbuf-cache.h"
#include "pps-view-private.h"

static gboolean page_ready;

static void
page_finished (PpsPixbufCache *cache, gint page, gpointer data)
{
	page_ready = TRUE;
}

static void
wait_for_page (PpsPixbufCache *cache)
{
	gint64 deadline = g_get_monotonic_time () + 10 * G_TIME_SPAN_SECOND;
	while (!page_ready && g_get_monotonic_time () < deadline) {
		while (g_main_context_iteration (NULL, FALSE))
			;
		g_usleep (1000);
	}
	g_assert_true (page_ready);
	g_assert_nonnull (pps_pixbuf_cache_get_texture (cache, 0));
}

static void
check_selection (PpsPixbufCache *cache, double scale, const char *label)
{
	GdkTexture *selection = pps_pixbuf_cache_get_selection_texture (cache, 0, scale);
	g_assert_nonnull (selection);
	int width = gdk_texture_get_width (selection);
	int height = gdk_texture_get_height (selection);
	g_autofree guchar *pixels = g_malloc (width * height * 4);
	gdk_texture_download (selection, pixels, width * 4);
	guint selected = 0, max_alpha = 0;
	for (int i = 0; i < width * height; i++) {
		guint alpha = pixels[i * 4 + 3];
		selected += alpha > 0;
		max_alpha = MAX (max_alpha, alpha);
	}
	g_print ("%s: %u selected pixels, maximum alpha %u/255\n", label, selected, max_alpha);
	g_assert_cmpuint (selected, >, 100);
	/* Both focused (30%) and unfocused (10%) overlays must preserve the page. */
	g_assert_cmpuint (max_alpha, >, 0);
	g_assert_cmpuint (max_alpha, <=, 80);
}

int
main (void)
{
	g_assert_true (pps_init ());
	adw_init ();
	g_autoptr (GError) error = NULL;
	g_autofree char *dir = g_dir_make_tmp ("papers-selection-XXXXXX", &error);
	g_assert_no_error (error);
	g_autofree char *path = g_build_filename (dir, "selection.pdf", NULL);
	g_autofree char *uri = g_filename_to_uri (path, NULL, NULL);
	cairo_surface_t *pdf = cairo_pdf_surface_create (path, 400, 200);
	cairo_t *cr = cairo_create (pdf);
	PangoLayout *layout = pango_cairo_create_layout (cr);
	PangoFontDescription *font = pango_font_description_from_string ("Sans 20");
	pango_layout_set_font_description (layout, font);
	pango_layout_set_text (layout, "Selection stays readable\n中文选区保持清晰", -1);
	cairo_move_to (cr, 20, 30);
	pango_cairo_show_layout (cr, layout);
	g_object_unref (layout);
	pango_font_description_free (font);
	cairo_destroy (cr);
	cairo_surface_destroy (pdf);

	g_autoptr (PpsDocument) doc = pps_document_factory_get_document (uri, &error);
	g_assert_no_error (error);
	g_assert_true (pps_document_load (doc, uri, &error));
	g_assert_no_error (error);
	pps_document_setup_cache (doc);
	g_autoptr (PpsAnnotationModel) annots = pps_annotation_model_new ();
	g_autoptr (PpsDocumentModel) model = g_object_new (PPS_TYPE_DOCUMENT_MODEL,
	                                                "annotation-model", annots,
	                                                "document", doc, "scale", 1.0, NULL);
	GtkWidget *window = gtk_window_new ();
	GtkWidget *view = gtk_drawing_area_new ();
	gtk_widget_set_focusable (view, TRUE);
	gtk_window_set_child (GTK_WINDOW (window), view);
	gtk_window_present (GTK_WINDOW (window));
	gtk_widget_grab_focus (view);
	g_autoptr (GObject) cache_object = G_OBJECT (pps_pixbuf_cache_new (view, model, 4 * 1024 * 1024));
	PpsPixbufCache *cache = PPS_PIXBUF_CACHE (cache_object);
	g_signal_connect (cache, "job-finished", G_CALLBACK (page_finished), NULL);
	PpsViewSelection selected = { .page = 0, .style = PPS_SELECTION_STYLE_GLYPH,
	                              .rect = { 0, 0, 400, 200 } };
	GList list = { .data = &selected };

	/* Selection present when the background render job starts. */
	pps_pixbuf_cache_set_page_range (cache, 0, 0, &list);
	wait_for_page (cache);
	check_selection (cache, 1.0, "background render");
	g_autoptr (PpsPage) page = pps_document_get_page (doc, 0);
	g_autofree char *text = pps_selection_get_selected_text (PPS_SELECTION (doc), page,
	                                                       PPS_SELECTION_STYLE_GLYPH, &selected.rect);
	g_assert_nonnull (strstr (text, "Selection stays readable"));
	g_assert_nonnull (strstr (text, "中文选区保持清晰"));

	/* Drag updates use the synchronous path, then zoom triggers another job. */
	selected.rect.y2 = 65;
	pps_pixbuf_cache_set_selection_list (cache, &list);
	check_selection (cache, 1.0, "drag update");
	pps_document_model_set_scale (model, 1.5);
	page_ready = FALSE;
	pps_pixbuf_cache_set_page_range (cache, 0, 0, &list);
	wait_for_page (cache);
	check_selection (cache, 1.5, "zoom");
	gtk_window_set_focus (GTK_WINDOW (window), NULL);
	pps_pixbuf_cache_style_changed (cache);
	check_selection (cache, 1.5, "unfocused");
	pps_pixbuf_cache_set_selection_list (cache, NULL);
	g_assert_null (pps_pixbuf_cache_get_selection_texture (cache, 0, 1.5));
	g_clear_object (&cache_object);
	gtk_window_destroy (GTK_WINDOW (window));
	g_assert_cmpint (g_remove (path), ==, 0);
	g_assert_cmpint (g_rmdir (dir), ==, 0);
	return 0;
}
