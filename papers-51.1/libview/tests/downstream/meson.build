fixture = source_root / 'test-data/utf16le-annot.pdf'
regression_env = environment()
regression_env.set('PPS_BACKENDS_DIR', build_root / 'libdocument/backend')
regression_env.set('PAPERS_TEST_PDF', fixture)

cairo_pdf_dep = dependency('cairo-pdf')
regressions = {
  'annotation-lifecycle': [libppsview_pair.get_static_lib(), []],
  'annotation-popup': [libppsview, [fixture]],
  'render-text': [libppsview_pair.get_static_lib(), []],
  'save-reload': [libppsview, [fixture]],
  'selection-render': [libppsview_pair.get_static_lib(), []],
}

foreach name, options : regressions
  regression = executable(
    'test-' + name,
    'test-' + name + '.c',
    sources: [enum_sources[1]],
    include_directories: [top_inc, libview_inc, context_inc, factory_inc],
    dependencies: libview_deps + [cairo_pdf_dep],
    link_with: options[0],
    c_args: ['-DPAPERS_COMPILATION'],
  )
  test(name, regression, args: options[1], env: regression_env)
endforeach
