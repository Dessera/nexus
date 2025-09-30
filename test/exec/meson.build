test_task_src = files('test_task.cpp')
test_task = executable(
    'test_task',
    test_task_src,
    dependencies: test_deps,
    cpp_args: test_args,
)

test('test_task', test_task)