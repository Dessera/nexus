# Nexus Format

`Format` is a set of utilities for format and print, which is the expansion of `std::format`.

## Prints

Use `nexus::print` or `nexus::println` to print format messages:

```cpp
#include <nexus/utils/format.hpp>

// To stdout
nexus::print("{}", 42);     // 42
nexus::println("{}", 42);   // 42\n
nexus::println();           // \n

// To any FILE*
nexus::print(stderr, "{}", 42);     // 42
nexus::println(stderr, "{}", 42);   // 42\n
nexus::println(stderr);             // \n
```

> Any object in the arguments list should implement the `std::formatter`.

## `to_formattable`

`to_formattable` converts any type to a formattable type, if type implements `std::formatter`, the function returns the const reference of it, otherwise, the function returns the pointer of it (`const void*`).

Example:

```cpp
std::format("Object: {}", to_formattable(obj));
```

## ANSI Support

Use `nexus::ansi` to wrap ANSI command for a formattable object, or print some ANSI commands. This function is a wrapper for `nexus::format::ANSI` and `nexus::format::TextStyle`.

Example

```cpp
using namespace nexus;

print("{}", ansi(42, format::BGRed));   // \033[41m42\033[0m
```
