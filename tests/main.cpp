#include <iostream>
#include <variant>

#include <dga/barrier.h>
#include <dga/fast_variant_visit.h>
#include <dga/aliases.h>
#include <dga/hash_combine.h>
#include <dga/semaphore.h>

#include <dga/enum_size.h>

struct Foo {
    int x;
};
struct Bar {
    float y;
};

using Var = std::variant<Foo, Bar>;

struct Visitor {
    void operator()(const Foo& foo) {
        std::cout << foo.x << std::endl;
    }
    void operator()(const Bar& bar) {
        std::cout << bar.y << std::endl;
    }
};

int main() {
    // Barrier.
    dga::Barrier barrier(3);
    (void)barrier;

    // Fast variant visit.
    dga::visit(Visitor{}, Var{Foo{123}});
    Var var = Bar{123.0f};
    dga::visit(Visitor{}, var);

    // Inttypes.
    dga::u8 some_byte;
    dga::i32 some_int;
    (void)some_byte;
    (void)some_int;

    // Semaphore.
    dga::Semaphore semaphore;
    (void)semaphore;

    // Hash combine.
    std::size_t hash = 0;
    dga::hashCombine(hash, 100, 200, std::string{"hello"});

    //std::cout << dga::count_v<SomeEnum>;
    dga::detail::n<SomeEnum, SomeEnum::Foo>();

    return 0;
}