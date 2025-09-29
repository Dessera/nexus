test_deps = [fp_dep]
test_args = [
    '-Wall',
    '-Wextra',
    '-Wno-pedantic',
    '-Werror',
]

test_curried_src = files('test_curried.cpp')
test_curried = executable(
    'test_curried',
    test_curried_src,
    dependencies: test_deps,
    cpp_args: test_args,
)

test('test_curried', test_curried)

test_lazy_src = files('test_lazy.cpp')
test_lazy = executable(
    'test_lazy',
    test_lazy_src,
    dependencies: test_deps,
    cpp_args: test_args,
)

test('test_lazy', test_lazy)