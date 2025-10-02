# Nexus

Basic components for C++20

## Performance

> Test with script `test/exec/test_stress_pool.cpp` with `10000` batch size.

Command: `test_stress_pool default <tester> 10000 <workers>`

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

### Host Info

- Processor: 16 × 11th Gen Intel® Core™ i7-11800H @ 2.30GHz
- Memory: 16 GiB DDR4
- OS: NixOS 25.11 (Linux 6.16.9)
- Arch: x86_64
- Compiler: GCC 14.3.0
- Debug Args: -O0 -g
- Release Args: -O3

### Testers

- `sleep`: Sleep `2ms` use `std::this_thread::sleep_for`
- `tinyloop`: Do `num = 0` for `120000` times.
- `midloop`: Do `num = 0` for `1200000` times.
- `largeloop`: Do `num = 0` for `12000000` times.

### Thread Pool Configuration

- Max Workers: 16
- Min Workers: 1
- Init Workers: 8
