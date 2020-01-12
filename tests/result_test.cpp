/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#include <gtest/gtest.h>
#include <dga/result.h>

namespace {
// Helper class to create a wrapper of a T that is convertible to T.
// Useful to test functions that take a U and have a precondition that U is convertible to T.
template <typename T> struct ConvertibleTo {
    explicit ConvertibleTo(T value) : value_(value) {
    }

    operator T() const {
        return value_;
    }

private:
    T value_;
};
}  // namespace

using dga::Error;
using dga::Result;

TEST(Result, Error) {
    Error<int> error{100};
    EXPECT_EQ(error.value(), 100);
}

TEST(Result, DefaultInitialise) {
    Result<int, int> default_result;
    EXPECT_TRUE(default_result.has_value());
    EXPECT_TRUE(bool(default_result));
    EXPECT_EQ(default_result.value(), 0);
}

TEST(Result, ResultValue) {
    // Construct from T.
    {
        Result<float, int> result{1.0f};
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(bool(result));
        EXPECT_EQ(result.value(), 1.0f);
    }

    // Construct from U (convertible to T).
    {
        Result<float, int> result{1.0};
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(bool(result));
        EXPECT_EQ(result.value(), 1.0f);
    }
    {
        Result<float, int> result{ConvertibleTo{1.0f}};
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(bool(result));
        EXPECT_EQ(result.value(), 1.0f);
    }

    // Construct with error type but as value.
    {
        Result<float, int> result{1};
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(bool(result));
        EXPECT_EQ(result.value(), 1.0f);
    }

    // Error.
    {
        Result<float, int> error{Error<int>{1}};
        EXPECT_FALSE(error.has_value());
        EXPECT_FALSE(bool(error));
        EXPECT_EQ(error.error(), 1);
    }
}

TEST(Result, ValueOr) {
    {
        Result<float, int> result{1.0f};
        EXPECT_EQ(result.value_or(300.0f), 1.0f);
    }
    {
        Result<float, int> result{};
        EXPECT_EQ(result.value_or(300.0f), 0.0f);
    }
    {
        Result<float, int> result{Error<int>(100)};
        EXPECT_EQ(result.value_or(300.0f), 300.0f);
    }
    {
        Result<float, int> result{Error<int>(100)};
        EXPECT_EQ(result.value_or(ConvertibleTo{300.0f}), 300.0f);
    }
}

// TODO: Support void results.
/*
TEST(Result, DefaultVoidResult) {
    Result<void, int> default_result;
    EXPECT_TRUE(default_result.has_value());
    EXPECT_TRUE(bool(default_result));
    static_assert(std::is_void_v<default_result.value()>);
}
 */