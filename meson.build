project(
    'nexus',
    'cpp',
    version: '0.1.0',
    default_options: ['warning_level=3', 'cpp_std=gnu++20'],
)

gtest_dep = dependency('gtest_main', required: false)

lib_deps = []
lib_args = [
    '-DBUILDING_NEXUS',
    '-Wall',
    '-Wextra',
    '-Wno-pedantic',
    '-Werror',
]

subdir('include')
subdir('src')

lib = library(
    'nexus',
    lib_src,
    gnu_symbol_visibility: 'hidden',
    include_directories: lib_inc,
    dependencies: lib_deps,
    cpp_args: lib_args,
    install: true,
)

nexus_dep = declare_dependency(
    include_directories: lib_inc,
    link_with: lib,
    dependencies: lib_deps,
)

if gtest_dep.found()
    subdir('test')
else
    message('gtest not found, tests will not be built')
endif

pkg_mod = import('pkgconfig')

pkg_mod.generate(
    name: 'nexus',
    filebase: 'nexus',
    description: 'Basic components for C++20',
    libraries: [lib],
    version: '0.1.0',
)