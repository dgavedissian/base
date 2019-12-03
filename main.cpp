#include <iostream>
#include <variant>
#include "fast_variant_visit/fast_variant_visit.h"

struct Foo { int x; };
struct Bar { float y; };

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
    std::cout << "Hello, World!" << std::endl;

    dga::visit(Visitor{}, Var{Foo{123}});
    Var var = Bar{123.0f};
    dga::visit(Visitor{}, var);
    return 0;
}