test_deps = [nexus_dep, gtest_dep]
test_deps_not_unit = [nexus_dep]
test_args = [
    '-Wall',
    '-Wextra',
    '-Wno-pedantic',
    '-Werror',
]

test_src = files(
    'test_curried.cpp',
    'test_lazy.cpp',
)

subdir('exec')
subdir('sync')
subdir('utils')

subdir('stress')

test_nexus = executable(
    'test_nexus',
    test_src,
    dependencies: test_deps,
    cpp_args: test_args,
    install: false,
)

test('test_nexus', test_nexus)