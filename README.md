# base

Various utility libraries.

* [aliases.h]() - Rust-like type aliases, such as u8 - u32, and i8 - i32.
* [barrier.h]() - Thread barrier.
* [fast_variant_visit.h]() - Alternative implementation of std::visit that has better codegen in GCC and Clang.
* [platform.h]() - Defines common platform flags (such as `DGA_WIN32` or `DGA_64_BIT`).
* [scope.h]() - Implementation of proposal [p0052r10](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0052r10.pdf) "Generic Scope Guard and RAII Wrapper for the Standard Library"
* [semaphore.h]() - Semaphore.
* [string_algorithms.h]() - Various useful string algorithms, such as `join`, `split` and `replace`.