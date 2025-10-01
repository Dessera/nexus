test_src += files(
    'test_pool.cpp',
    'test_queue.cpp',
    'test_task.cpp',
    'test_worker.cpp',
)

test_stress_pool_src = files(
    'test_stress_pool.cpp',
)
executable(
    'test_stress_pool',
    test_stress_pool_src,
    dependencies: test_deps_not_unit,
    cpp_args: test_args,
    install: false,
)