# Nexus Exec

`Exec` is a set of utilties for "Execution", which helps user to execute
function in any context. For now, I only implements a legacy thread pool.

## Thread pool

### Build a thread pool

Use `ThreadPool::Builder` to build a `ThreadPool`:

```cpp
#include <nexus/exec/thread.h>

using nexus::exec::ThreadPool;

ThreadPool::Builder()
            .policy(TaskPolicy::FIFO)
            .max_workers(16)
            .min_workers(1)
            .init_workers(8)
            .remove_cancelled(false)
            .build();   // -> ThreadPool
```

Or you can use pre-defined provider:

```cpp
#include <nexus/exec/thread.h>

using namespace nexus::exec::thread_builder;

blank().build();        // With fixed max/min/init workers
common().build();       // Get workers with `hardware_concurrency`
cpu_bound().build();    // Get workers with `hardware_concurrency` but only use the real cpu count (half of the concurrency).
io_bound().build();     // With fixed max/min/init workers (but very large)
time_bound().build();   //Get workers with `hardware_concurrency` but only use the real cpu count (half of the concurrency).
```

### Tasks

`ThreadPool` provides `emplace` and `push` to add tasks, all task will be sent to `TaskQueue`.

```cpp
using nexus::exec::Task;

pool.emplace([]() { return 1; });   // with value
pool.emplace([]() { });             // void
pool.push(Task<>([]() { }))         // push void
```

You can pass arguments into the task, but neither arguments nor result do not support reference (decayed).

Result of the task will be passed by `std::future`:

```cpp
auto task = Task<>([](int lhs, int rhs) { return lhs + rhs; }, 1, 2);
auto fut = task.get_future(); // Get future from task.

pool.push(std::move(task));

auto res = fut.get(); // Get from task, note that error in task will be
                      // thrown here.
```

Or simply:

```cpp
auto fut = pool.emplace([](int lhs, int rhs) { return lhs + rhs; }, 1, 2);
auto res = fut.get();
```

### Performance

> Test with script `test/exec/test_stress_pool.cpp` with `10000` batch size.

Command: `test_stress_pool common <tester> 10000 <workers>`

Debug:

| Tester | TPS/Workers=2 | TPS/Workers=8 | TPS/Workers=16 |
| ---- | ---- | ---- | ---- |
| `sleep` | 957.533 | 3859.83 | 7709.7 |
| `tinyloop` | 10393.5 | 38222.1 | 61703.1 |
| `midloop` | 1104.71 | 4085.35 | 6421.77 |
| `largeloop` | 108.556 | 399.025 | 644.613 |

Release:

| Tester | TPS/Workers=2 | TPS/Workers=8 | TPS/Workers=16 |
| ---- | ---- | ---- | ---- |
| `sleep` | 961.138 | 3870.61 | 7737.4 |

> Do not conduct loop testing as they have been optimized.

#### Host Info

- Processor: 16 × 11th Gen Intel® Core™ i7-11800H @ 2.30GHz
- Memory: 16 GiB DDR4
- OS: NixOS 25.11 (Linux 6.16.9)
- Arch: x86_64
- Compiler: GCC 14.3.0
- Debug Args: -O0 -g
- Release Args: -O3

#### Testers

- `sleep`: Sleep `2ms` use `std::this_thread::sleep_for`
- `tinyloop`: Do `num = 0` for `120000` times.
- `midloop`: Do `num = 0` for `1200000` times.
- `largeloop`: Do `num = 0` for `12000000` times.

#### Thread Pool Configuration

- Max Workers: 16
- Min Workers: 1
- Init Workers: 8

## Queue Policy

TaskQueue supports four policies:

- `FIFO`: Always pop the first task in queue
- `LIFO`: Always pop the last task in queue
- `PRIO`: Pop task with the highest priority (`task.prio()`)
- `RAND`: Pop task randomly
