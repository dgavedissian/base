/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#include <gtest/gtest.h>
#include <dga/flags.h>

namespace {
enum class TestEnum { A, B, C, _Count };

enum TestUnscopedEnum { A, B, C, _Count };

enum class IntEnum : short { A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, _Count };
}  // namespace

using dga::Flags;

TEST(Flags, DefaultConstruct) {
    Flags<TestEnum> flags;
    EXPECT_FALSE(flags.isSet(TestEnum::A));
    EXPECT_FALSE(flags.isSet(TestEnum::B));
    EXPECT_FALSE(flags.isSet(TestEnum::C));
}

TEST(Flags, ConstructWithEnum) {
    Flags<TestEnum> flags{TestEnum::A};
    EXPECT_TRUE(flags.isSet(TestEnum::A));
    EXPECT_FALSE(flags.isSet(TestEnum::B));
    EXPECT_FALSE(flags.isSet(TestEnum::C));
}

TEST(Flags, CopyConstructWithEnum) {
    // enum value constructor is explicit, so we cannot use copy-initialization directly.
    Flags<TestEnum> flags = Flags{TestEnum::A};
    EXPECT_TRUE(flags.isSet(TestEnum::A));
    EXPECT_FALSE(flags.isSet(TestEnum::B));
    EXPECT_FALSE(flags.isSet(TestEnum::C));
}

TEST(Flags, All) {
    auto all_flags = Flags<TestEnum>::all();
    EXPECT_TRUE(all_flags.isSet(TestEnum::A));
    EXPECT_TRUE(all_flags.isSet(TestEnum::B));
    EXPECT_TRUE(all_flags.isSet(TestEnum::C));
    EXPECT_EQ(all_flags, Flags<TestEnum>{TestEnum::A} | TestEnum::B | TestEnum::C);
}

TEST(Flags, None) {
    auto none_flags = Flags<TestEnum>::none();
    EXPECT_FALSE(none_flags.isSet(TestEnum::A));
    EXPECT_FALSE(none_flags.isSet(TestEnum::B));
    EXPECT_FALSE(none_flags.isSet(TestEnum::C));
    EXPECT_EQ(none_flags, Flags<TestEnum>());
}

TEST(Flags, Set) {
    Flags<TestEnum> flags;
    EXPECT_FALSE(flags.isSet(TestEnum::A));
    EXPECT_FALSE(flags.isSet(TestEnum::B));
    EXPECT_FALSE(flags.isSet(TestEnum::C));

    flags.set(TestEnum::B);
    EXPECT_FALSE(flags.isSet(TestEnum::A));
    EXPECT_TRUE(flags.isSet(TestEnum::B));
    EXPECT_FALSE(flags.isSet(TestEnum::C));

    flags.set(TestEnum::A);
    EXPECT_TRUE(flags.isSet(TestEnum::A));
    EXPECT_TRUE(flags.isSet(TestEnum::B));
    EXPECT_FALSE(flags.isSet(TestEnum::C));

    flags.set(TestEnum::A);
    EXPECT_TRUE(flags.isSet(TestEnum::A));
    EXPECT_TRUE(flags.isSet(TestEnum::B));
    EXPECT_FALSE(flags.isSet(TestEnum::C));
}

TEST(Flags, Reset) {
    Flags<TestEnum> flags{TestEnum::B};
    EXPECT_FALSE(flags.isSet(TestEnum::A));
    EXPECT_TRUE(flags.isSet(TestEnum::B));
    EXPECT_FALSE(flags.isSet(TestEnum::C));

    flags.reset(TestEnum::A);
    EXPECT_FALSE(flags.isSet(TestEnum::A));
    EXPECT_TRUE(flags.isSet(TestEnum::B));
    EXPECT_FALSE(flags.isSet(TestEnum::C));

    flags.reset(TestEnum::B);
    EXPECT_FALSE(flags.isSet(TestEnum::A));
    EXPECT_FALSE(flags.isSet(TestEnum::B));
    EXPECT_FALSE(flags.isSet(TestEnum::C));
}

TEST(Flags, Toggle) {
    Flags<TestEnum> flags{TestEnum::C};
    EXPECT_FALSE(flags.isSet(TestEnum::A));
    EXPECT_FALSE(flags.isSet(TestEnum::B));
    EXPECT_TRUE(flags.isSet(TestEnum::C));

    flags.toggle(TestEnum::B);
    EXPECT_FALSE(flags.isSet(TestEnum::A));
    EXPECT_TRUE(flags.isSet(TestEnum::B));
    EXPECT_TRUE(flags.isSet(TestEnum::C));

    flags.toggleAll();
    EXPECT_TRUE(flags.isSet(TestEnum::A));
    EXPECT_FALSE(flags.isSet(TestEnum::B));
    EXPECT_FALSE(flags.isSet(TestEnum::C));
}

TEST(Flags, Operators) {
    {
        auto flags1 = Flags{TestEnum::A} | TestEnum::B;
        auto flags2 = TestEnum::A | Flags{TestEnum::B};
        EXPECT_TRUE(flags1.isSet(TestEnum::A));
        EXPECT_TRUE(flags1.isSet(TestEnum::B));
        EXPECT_FALSE(flags1.isSet(TestEnum::C));
        EXPECT_TRUE(flags2.isSet(TestEnum::A));
        EXPECT_TRUE(flags2.isSet(TestEnum::B));
        EXPECT_FALSE(flags2.isSet(TestEnum::C));
        EXPECT_EQ(flags1, flags2);
    }
    {
        auto conjunction = Flags<TestEnum>::all() & TestEnum::B;
        EXPECT_EQ(conjunction, TestEnum::B);
        EXPECT_EQ(TestEnum::B, conjunction);
    }
    {
        auto toggle = Flags<TestEnum>::all() ^ TestEnum::B;
        EXPECT_EQ(toggle, Flags{TestEnum::A} | TestEnum::C);
    }
    {
        auto flags = Flags{TestEnum::A} | TestEnum::C;
        EXPECT_EQ(~flags, TestEnum::B);
        EXPECT_EQ(~Flags<TestEnum>::all(), Flags<TestEnum>::none());
    }
}

TEST(Flags, UnscopedEnum) {
    Flags<TestUnscopedEnum> flags;
    flags.set(A);
    flags.set(B);
    EXPECT_TRUE(flags.isSet(TestUnscopedEnum::A));
    EXPECT_TRUE(flags.isSet(TestUnscopedEnum::B));
    EXPECT_FALSE(flags.isSet(TestUnscopedEnum::C));
}

TEST(Flags, IntOverflow) {
    static_assert(sizeof(Flags<IntEnum>) == 2);
    static_assert(std::is_same_v<typename Flags<IntEnum>::mask_type, unsigned short>);

    Flags<IntEnum> flags;
    flags.set(IntEnum::A15);
    EXPECT_TRUE(flags.isSet(IntEnum::A15));
}