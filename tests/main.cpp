#include <iostream>
#include <variant>

#include <dga/barrier.h>
#include <dga/fast_variant_visit.h>
#include <dga/aliases.h>
#include <dga/hash_combine.h>
#include <dga/scope.h>
#include <dga/semaphore.h>
#include <dga/string_algorithms.h>

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

    // String algorithms.
    std::string foo = "100-200-300";
    std::vector<std::string> collection;
    dga::strSplit("100-200-300", '-', std::back_inserter(collection));
    foo = dga::strJoin(collection.begin(), collection.end(), ",");
    foo = dga::strReplaceAll(foo, "200", "150");
    (void)foo;

    // Scope guards.
    {
        auto on_exit = dga::scope_exit{[&] { std::cout << "Called on exit." << std::endl; }};
        std::cout << "Called before exit." << std::endl;
    }

    // Hash combine.
    std::size_t hash = 0;
    dga::hashCombine(hash, 100, 200, std::string{"hello"});

    return 0;
}