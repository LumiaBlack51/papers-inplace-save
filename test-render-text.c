#include <papers-view.h>
#include <papers-document.h>
#include "pps-annotation-overlay.h"
#include <cairo-pdf.h>
#include <glib/gstdio.h>
static GtkWidget *find_text (GtkWidget *w) {
 if(GTK_IS_TEXT_VIEW(w)) return w;
 for(GtkWidget *c=gtk_widget_get_first_child(w);c;c=gtk_widget_get_next_sibling(c)) {
  GtkWidget *r=find_text(c); if(r) return r;
 }
 return NULL;
}
static unsigned dark_pixels (PpsDocument *doc) {
 PpsPage *page = pps_document_get_page (doc,0);
 PpsRenderContext *rc = pps_render_context_new (page,0,1.,PPS_RENDER_ANNOTS_ALL);
 cairo_surface_t *s = pps_document_render (doc,rc);
 cairo_surface_flush(s);
 unsigned count=0;
 unsigned char *bytes=cairo_image_surface_get_data(s);
 int stride=cairo_image_surface_get_stride(s);
 for(int y=0;y<200;y++) for(int x=0;x<300;x++) {
  unsigned char *p=bytes+y*stride+4*x;
  if(p[0]<240 || p[1]<240 || p[2]<240) count++;
 }
 cairo_surface_destroy(s); g_object_unref(rc); g_object_unref(page);
 return count;
}
int main(void) {
 pps_init();gtk_init();
 GError *error=NULL;
 char *dir=g_dir_make_tmp("papers-render-test-XXXXXX",&error);
 char *path=g_build_filename(dir,"blank.pdf",NULL);
 cairo_surface_t *s=cairo_pdf_surface_create(path,300,200);
 cairo_t *cr=cairo_create(s); cairo_show_page(cr); cairo_destroy(cr); cairo_surface_destroy(s);
 char *uri=g_filename_to_uri(path,NULL,NULL);
 PpsDocument *doc=pps_document_factory_get_document(uri,&error);g_assert_no_error(error);
 g_assert_true(pps_document_load(doc,uri,&error));g_assert_no_error(error);
 PpsPage *page=pps_document_get_page(doc,0);
 PpsAnnotation *a=pps_annotation_free_text_new(page);
 PpsRectangle rect={30,30,230,100};
 GdkRGBA black={0,0,0,1},transparent={0,0,0,0};
 pps_annotation_set_area(a,&rect);pps_annotation_set_rgba(a,&transparent);
 pps_annotation_free_text_set_font_rgba(PPS_ANNOTATION_FREE_TEXT(a),&black);
 PangoFontDescription *font=pango_font_description_from_string("Liberation Sans 18");
 pps_annotation_free_text_set_font_description(PPS_ANNOTATION_FREE_TEXT(a),font);
 pps_annotation_set_contents(a,"");
 pps_document_annotations_add_annotation(PPS_DOCUMENT_ANNOTATIONS(doc),a);
 g_print("blank=%u\n",dark_pixels(doc));
 pps_annotation_set_contents(a,"Hello"); g_print("latin=%u\n",dark_pixels(doc));
 pps_annotation_set_contents(a,"中文测试"); g_print("han-liberation=%u\n",dark_pixels(doc));
 PpsAnnotationModel *am=pps_annotation_model_new();
 PpsDocumentModel *model=g_object_new(PPS_TYPE_DOCUMENT_MODEL,"annotation-model",am,"document",doc,NULL);
 PpsUndoContext *undo=pps_undo_context_new(model);
 PpsAnnotationsContext *ctx=pps_annotations_context_new(model,undo);
 GtkWidget *overlay=g_object_ref_sink(pps_overlay_annotation_entry_new(a,ctx,model));
 unsigned visible=dark_pixels(doc);
 g_print("han-after-editor-font-fallback=%u\n",visible);g_assert_cmpuint(visible,>,0);
 PangoFontDescription *actual=pps_annotation_free_text_get_font_description(PPS_ANNOTATION_FREE_TEXT(a));
 g_assert_cmpint(pango_font_description_get_size(actual),==,18*PANGO_SCALE);
 GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(find_text(overlay)));
 gtk_text_buffer_set_text(buffer,"",-1);
 pps_annotation_free_text_set_font_description(PPS_ANNOTATION_FREE_TEXT(a),font);
 gtk_text_buffer_set_text(buffer,"Hello",-1);
 g_assert_cmpuint(dark_pixels(doc),>,0);
 PangoFontDescription *latin=pps_annotation_free_text_get_font_description(PPS_ANNOTATION_FREE_TEXT(a));
 g_assert_cmpstr(pango_font_description_get_family(latin),==,"Liberation Sans");
 gtk_text_buffer_set_text(buffer,"中文测试",-1);
 g_assert_cmpuint(dark_pixels(doc),>,0);
 PpsJob *job=pps_job_save_new(doc,uri,uri);pps_job_run(job);
 g_assert_true(pps_job_is_succeeded(job,&error));g_assert_no_error(error);
 PpsDocument *reopened=pps_document_factory_get_document(uri,&error);g_assert_no_error(error);
 g_assert_true(pps_document_load(reopened,uri,&error));g_assert_no_error(error);
 unsigned persisted=dark_pixels(reopened);
 g_print("han-after-save-reopen=%u\n",persisted);g_assert_cmpuint(persisted,>,0);
 PpsPage *rp=pps_document_get_page(reopened,0);
 PpsRenderContext *rc=pps_render_context_new(rp,0,1.,PPS_RENDER_ANNOTS_ALL);
 cairo_surface_t *image=pps_document_render(reopened,rc);
 cairo_surface_write_to_png(image,"chinese-save-check.png");
 cairo_surface_destroy(image);
 g_remove(path);g_rmdir(dir);
 return 0;
}
