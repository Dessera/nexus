project(
  'fp',
  'cpp',
  version: '0.1.0',
  default_options: ['warning_level=3', 'cpp_std=gnu++20'],
)

lib_deps = []
lib_args = [
  '-DBUILDING_FP',
  '-Wall',
  '-Wextra',
  '-Wno-pedantic',
  '-Werror',
]

subdir('include')
subdir('src')

lib = library(
  'fp',
  lib_src,
  gnu_symbol_visibility: 'hidden',
  include_directories: lib_inc,
  dependencies: lib_deps,
  cpp_args: lib_args,
  install: true,
)

fp_dep = declare_dependency(
  include_directories: lib_inc,
  link_with: lib,
  dependencies: lib_deps,
)

subdir('test')

pkg_mod = import('pkgconfig')

pkg_mod.generate(
  name: 'fp',
  filebase: 'fp',
  description: 'FP-like utilities.',
  libraries: [lib],
  version: '0.1.0',
)