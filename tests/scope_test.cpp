/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#include <gtest/gtest.h>
#include <dga/scope.h>

namespace {
class ThrowOnCopy {
public:
    ThrowOnCopy() = default;
    ThrowOnCopy(ThrowOnCopy&&) = default;
    ThrowOnCopy(const ThrowOnCopy&) {
        throw 42;
    }

    void operator()() {
        calls++;
    }

    int calls = 0;
};
}  // namespace

TEST(ScopeExit, EndOfScope) {
    int calls = 0;
    {
        auto guard = dga::ScopeExit{[&] { calls++; }};
    }
    EXPECT_EQ(calls, 1);
}

TEST(ScopeExit, Release) {
    int calls = 0;
    {
        auto guard = dga::ScopeExit{[&] { calls++; }};
        guard.release();
    }
    EXPECT_EQ(calls, 0);
}

TEST(ScopeExit, CalledOnThrow) {
    int calls = 0;
    try {
        auto guard = dga::ScopeExit{[&] { calls++; }};
        throw 42;
    } catch (...) {
        // empty handler.
    }
    EXPECT_EQ(calls, 1);
}

TEST(ScopeExit, MoveConstructor) {
    int calls = 0;
    {
        auto guard1 = dga::ScopeExit{[&] { calls++; }};
        auto guard2 = dga::ScopeExit{std::move(guard1)};
    }
    EXPECT_EQ(calls, 1);
}

TEST(ScopeExit, CallsFunctionWhenFunctionCopyThrows) {
    ThrowOnCopy f;
    bool exception_thrown = false;
    try {
        auto guard = dga::ScopeExit{f};
    } catch (...) {
        exception_thrown = true;
    }
    // f will throw an exception when copied into the scope object. When this happens, f should be
    // called.
    EXPECT_TRUE(exception_thrown);
    EXPECT_EQ(f.calls, 1);
}

TEST(ScopeFail, EndOfScope) {
    int calls = 0;
    {
        auto guard = dga::ScopeFail{[&] { calls++; }};
    }
    EXPECT_EQ(calls, 0);
}

TEST(ScopeFail, Release) {
    int calls = 0;
    try {
        auto guard = dga::ScopeFail{[&] { calls++; }};
        guard.release();
    } catch (...) {
        // empty handler.
    }
    EXPECT_EQ(calls, 0);
}

TEST(ScopeFail, CalledOnThrow) {
    int calls = 0;
    try {
        auto guard = dga::ScopeFail{[&] { calls++; }};
        throw 42;
    } catch (...) {
        // empty handler.
    }
    EXPECT_EQ(calls, 1);
}

TEST(ScopeFail, CallsFunctionWhenFunctionCopyThrows) {
    ThrowOnCopy f;
    bool exception_thrown = false;
    try {
        auto guard = dga::ScopeFail{f};
    } catch (...) {
        exception_thrown = true;
    }
    // f will throw an exception when copied into the scope object. When this happens, f should be
    // called.
    EXPECT_TRUE(exception_thrown);
    EXPECT_EQ(f.calls, 1);
}

TEST(ScopeSuccess, EndOfScope) {
    int calls = 0;
    {
        auto guard = dga::ScopeSuccess{[&] { calls++; }};
    }
    EXPECT_EQ(calls, 1);
}

TEST(ScopeSuccess, Release) {
    int calls = 0;
    {
        auto guard = dga::ScopeSuccess{[&] { calls++; }};
        guard.release();
    }
    EXPECT_EQ(calls, 0);
}

TEST(ScopeSuccess, CalledOnThrow) {
    int calls = 0;
    try {
        auto guard = dga::ScopeSuccess{[&] { calls++; }};
        throw 42;
    } catch (...) {
        // empty handler.
    }
    EXPECT_EQ(calls, 0);
}

TEST(ScopeSuccess, DoesntCallFunctionWhenFunctionCopyThrows) {
    ThrowOnCopy f;
    bool exception_thrown = false;
    try {
        auto guard = dga::ScopeSuccess{f};
    } catch (...) {
        exception_thrown = true;
    }
    // f will throw an exception when copied into the scope object. When this happens, f should not
    // have been called.
    EXPECT_TRUE(exception_thrown);
    EXPECT_EQ(f.calls, 0);
}
