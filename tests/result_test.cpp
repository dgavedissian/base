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

template <typename T> ConvertibleTo(T)->ConvertibleTo<T>;
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
        EXPECT_EQ(*result, 1.0f);
    }

    // Construct from U (convertible to T).
    {
        Result<float, int> result{1.0};
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(bool(result));
        EXPECT_EQ(*result, 1.0f);
    }
    {
        Result<float, int> result{ConvertibleTo{1.0f}};
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(bool(result));
        EXPECT_EQ(*result, 1.0f);
    }

    // Construct with error type but as value.
    {
        Result<float, int> result{1};
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(bool(result));
        EXPECT_EQ(*result, 1.0f);
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

TEST(Result, Assignment) {
    Result<int, int> result;
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0);

    // Assign from rvalue ref result.
    result = Result<int, int>{100};
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 100);

    // Assign from lvalue ref result.
    {
        Result<int, int> source_result{200};
        result = source_result;
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(*result, 200);
    }

    // Assign from rvalue ref error.
    result = Error<int>{100};
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), 100);

    // Assign from lvalue ref error.
    {
        Error<int> source_error{200};
        result = source_error;
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), 200);
    }

    // Assign from rvalue ref U that's convertible to T.
    result = ConvertibleTo{300};
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.error(), 300);

    // Assign from lvalue ref U that's convertible to T.
    {
        auto source = ConvertibleTo{400};
        result = source;
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(*result, 400);
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