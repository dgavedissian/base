# base

Various C++ header only single file utility libraries.

* [aliases.h](include/dga/aliases.h) - Rust-like type aliases, such as `u8` - `u32`, and `i8` - `i32`.
* [barrier.h](include/dga/barrier.h) - Thread barrier.
* [platform.h](include/dga/platform.h) - Defines common platform flags (such as `DGA_WIN32` or `DGA_64_BIT`).
* [result.h](include/dga/result.h) - A type similar to `std::optional` that can store either a value or an error type. Similar to proposal [p0323r4](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0323r4.html) "std::expected".
* [scope.h](include/dga/scope.h) - Implementation of proposal [p0052r10](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0052r10.pdf) "Generic Scope Guard and RAII Wrapper for the Standard Library"
* [semaphore.h](include/dga/semaphore.h) - Semaphore.
* [string_algorithms.h](include/dga/string_algorithms.h) - Various useful string algorithms, such as join, split and replace.
