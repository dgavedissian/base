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

// Helper class to increment a counter when its destructor is called.
class DestructorCounter {
public:
    explicit DestructorCounter(int& destructor_calls) : destructor_calls_(&destructor_calls) {
    }

    DestructorCounter(DestructorCounter&& other) noexcept {
        std::swap(destructor_calls_, other.destructor_calls_);
    }

    DestructorCounter(const DestructorCounter&) = delete;

    DestructorCounter& operator=(DestructorCounter&& other) noexcept {
        std::swap(destructor_calls_, other.destructor_calls_);
        return *this;
    }

    DestructorCounter& operator=(const DestructorCounter&) = delete;

    ~DestructorCounter() {
        if (destructor_calls_) {
            (*destructor_calls_)++;
        }
    }

private:
    int* destructor_calls_ = nullptr;
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
    EXPECT_EQ(result.value(), 300);

    // Assign from lvalue ref U that's convertible to T.
    {
        auto source = ConvertibleTo{400};
        result = source;
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(*result, 400);
    }
}

TEST(Result, MissingResultValueException) {
    Result<int, int> result{Error<int>{123}};
    try {
        result.value();
        ADD_FAILURE() << "Exception was not thrown by value()";
    } catch (const dga::MissingResultValue<int>& exception) {
        EXPECT_EQ(exception.error(), 123);
    }
}

TEST(Result, Destructor) {
    // Destructor of value type should be called.
    {
        int destructor_calls = 0;
        { Result<DestructorCounter, int> result{DestructorCounter{destructor_calls}}; }
        EXPECT_EQ(destructor_calls, 1);
    }

    // Destructor of error type should be called.
    {
        int destructor_calls = 0;
        { Result<int, DestructorCounter> result{Error{DestructorCounter{destructor_calls}}}; }
        EXPECT_EQ(destructor_calls, 1);
    }

    // Assignment destroys the existing value or error.
    // value = value
    {
        int destructor1_calls = 0;
        int destructor2_calls = 0;
        {
            Result<DestructorCounter, DestructorCounter> result{
                DestructorCounter{destructor1_calls}};
            result = DestructorCounter{destructor2_calls};
            EXPECT_EQ(destructor1_calls, 1);
        }
        EXPECT_EQ(destructor2_calls, 1);
    }
    // error = value
    {
        int destructor1_calls = 0;
        int destructor2_calls = 0;
        {
            Result<DestructorCounter, DestructorCounter> result{
                Error<DestructorCounter>(DestructorCounter{destructor1_calls})};
            result = DestructorCounter{destructor2_calls};
            EXPECT_EQ(destructor1_calls, 1);
        }
        EXPECT_EQ(destructor2_calls, 1);
    }
    // value = error
    {
        int destructor1_calls = 0;
        int destructor2_calls = 0;
        {
            Result<DestructorCounter, DestructorCounter> result{
                DestructorCounter{destructor1_calls}};
            result = Error<DestructorCounter>{DestructorCounter{destructor2_calls}};
            EXPECT_EQ(destructor1_calls, 1);
        }
        EXPECT_EQ(destructor2_calls, 1);
    }
    // error = error
    {
        int destructor1_calls = 0;
        int destructor2_calls = 0;
        {
            Result<DestructorCounter, DestructorCounter> result{
                Error<DestructorCounter>(DestructorCounter{destructor1_calls})};
            result = Error<DestructorCounter>{DestructorCounter{destructor2_calls}};
            EXPECT_EQ(destructor1_calls, 1);
        }
        EXPECT_EQ(destructor2_calls, 1);
    }

    // Emplace destroys the existing value or error.
    {
        int destructor1_calls = 0;
        int destructor2_calls = 0;
        {
            Result<DestructorCounter, int> result{DestructorCounter{destructor1_calls}};
            result.emplace(DestructorCounter{destructor2_calls});
            EXPECT_EQ(destructor1_calls, 1);
        }
        EXPECT_EQ(destructor2_calls, 1);
    }
    {
        int destructor1_calls = 0;
        int destructor2_calls = 0;
        {
            Result<DestructorCounter, DestructorCounter> result{
                Error<DestructorCounter>(DestructorCounter{destructor1_calls})};
            result.emplace(DestructorCounter{destructor2_calls});
            EXPECT_EQ(destructor1_calls, 1);
        }
        EXPECT_EQ(destructor2_calls, 1);
    }
}

TEST(Result, VoidResult) {
    Result<void, int> result;
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(bool(result));

    result = {};
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(bool(result));

    result = Error<int>{123};
    EXPECT_FALSE(result.has_value());
    EXPECT_FALSE(bool(result));
    EXPECT_EQ(result.error(), 123);

    result.emplace();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(bool(result));
}