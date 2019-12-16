#include <iostream>
#include <variant>

#include <dga/barrier.h>
#include <dga/fast_variant_visit.h>
#include <dga/aliases.h>
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

    return 0;
}