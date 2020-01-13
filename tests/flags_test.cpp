/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#include <gtest/gtest.h>
#include <dga/flags.h>

namespace {
enum class TestEnum {
    Value0, Value1, Value2, _Count
};
}  // namespace

using dga::FlagsImpl;

TEST(Flags, Set) {

}
