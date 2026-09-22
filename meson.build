project('papers-view-local-fix', 'c', version: '50.2', default_options: ['buildtype=debugoptimized', 'warning_level=2'])
cc = meson.get_compiler('c')
incs = include_directories('papers-50.2', 'papers-50.2/libdocument', 'papers-50.2/libview', 'papers-50.2/libview/context', 'papers-50.2/libview/factory', 'generated', 'generated/libdocument', 'generated/libview')
deps = [dependency('gtk4'), dependency('libadwaita-1'), dependency('gthread-2.0'), dependency('gmodule-2.0'), dependency('gtk4-unix-print'), cc.find_library('m'), cc.find_library('ppsdocument-4.0', dirs: '/app/lib')]
args = ['-DHAVE_CONFIG_H', '-DPAPERS_COMPILATION', '-DI_KNOW_THE_PAPERS_LIBS_ARE_UNSTABLE_AND_HAVE_TALKED_WITH_THE_AUTHORS', '-DG_LOG_DOMAIN="PapersView"']
libview = both_libraries('ppsview-4.0',
  'papers-50.2/libview/context/pps-annotations-context.c',
  'papers-50.2/libview/context/pps-attachment-context.c',
  'papers-50.2/libview/context/pps-annotation-model.c',
  'papers-50.2/libview/context/pps-document-model.c',
  'papers-50.2/libview/context/pps-history.c',
  'papers-50.2/libview/context/pps-metadata.c',
  'papers-50.2/libview/context/pps-search-context.c',
  'papers-50.2/libview/context/pps-search-result.c',
  'papers-50.2/libview/context/pps-undo-context.c',
  'papers-50.2/libview/context/pps-undo-handler.c',
  'papers-50.2/libview/factory/pps-annotation-widget-factory.c',
  'papers-50.2/libview/factory/pps-element-widget-factory.c',
  'papers-50.2/libview/factory/pps-form-widget-factory.c',
  'papers-50.2/libview/pps-annotation-window.c',
  'papers-50.2/libview/pps-colors.c',
  'papers-50.2/libview/pps-debug.c',
  'papers-50.2/libview/pps-job.c',
  'papers-50.2/libview/pps-jobs.c',
  'papers-50.2/libview/pps-job-scheduler.c',
  'papers-50.2/libview/pps-overlay.c',
  'papers-50.2/libview/pps-form-overlay.c',
  'papers-50.2/libview/pps-page-cache.c',
  'papers-50.2/libview/pps-pixbuf-cache.c',
  'papers-50.2/libview/pps-print-operation.c',
  'papers-50.2/libview/pps-view.c',
  'papers-50.2/libview/pps-view-cursor.c',
  'papers-50.2/libview/pps-view-page.c',
  'papers-50.2/libview/pps-view-presentation.c',
  'papers-50.2/libview/pps-css-utils.c',
  'papers-50.2/libview/pps-annotation-overlay.c',
  'papers-50.2/libview/pps-annotation-layer.c',
  'papers-50.2/libview/pps-annotation-layer-objects.c',
  'papers-50.2/libview/pps-annotation-layer-ink.c',
  'generated/libview/pps-view-type-builtins.c',
  'generated/libview/pps-view-marshal.c',
  'generated/libview/pps-view-resources.c',
  version: '5.0.0', include_directories: incs, dependencies: deps, c_args: args, gnu_symbol_visibility: 'hidden')
regression = executable('test-annotation-lifecycle', 'test-annotation-lifecycle.c', include_directories: incs, dependencies: deps, c_args: args, link_with: libview.get_static_lib())
test('annotation-lifecycle', regression, env: ['PAPERS_TEST_PDF=' + meson.current_source_dir() / 'papers-50.2/test-data/utf16le-annot.pdf'])
save_regression = executable('test-save-reload', 'test-save-reload.c', include_directories: incs, dependencies: deps, c_args: args, link_with: libview.get_shared_lib())
test('save-reload', save_regression, args: [meson.current_source_dir() / 'papers-50.2/test-data/utf16le-annot.pdf'])

render_test = executable('test-render-text', 'test-render-text.c', include_directories: incs, dependencies: deps + [dependency('cairo-pdf')], c_args: args, link_with: libview.get_static_lib())
test('chinese-free-text-render-save', render_test)
