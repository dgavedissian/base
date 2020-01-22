/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#pragma once

#include <type_traits>
#include <exception>
#include "../dga/remove_cvref.h"

/*
 * Implementation of http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0052r10.pdf
 *
 * The API here is a subset of the proposal, but types have been renamed to CamelCase to match the
 * rest of this repo.
 *
 * References:
 * - Andrei Alexandrescu "Systematic Error Handling in C++":
 * https://www.youtube.com/watch?v=kaI4R0Ng4E8
 * - Andrei Alexandrescu “Declarative Control Flow": https://www.youtube.com/watch?v=WjTrfoiB0MQ
 *
 * The difference between the 3 scope types are the following:
 * * `ScopeExit` will always call EF at the end of the scope.
 * * `ScopeFail` will not call EF at the end of the scope _unless_ it is constructed inside a
 * block and an exception is thrown.
 * * `ScopeSuccess` will call EF at the end of the scope _unless_ it is constructed inside a
 * block and an exception is thrown.
 */

namespace dga {
namespace detail {
class OnExitPolicy {
public:
    explicit OnExitPolicy(bool execute) : execute_(execute) {
    }

    void release() noexcept {
        execute_ = false;
    }

    bool shouldExecute() const noexcept {
        return execute_;
    }

    constexpr static bool execute_on_constructor_failure = true;

private:
    bool execute_;
};

class OnFailPolicy {
public:
    explicit OnFailPolicy(bool execute)
        : uncaught_on_creation_(execute ? std::uncaught_exceptions() : -1) {
    }

    void release() noexcept {
        uncaught_on_creation_ = -1;
    }

    bool shouldExecute() const noexcept {
        return uncaught_on_creation_ != -1 && uncaught_on_creation_ < std::uncaught_exceptions();
    }

    constexpr static bool execute_on_constructor_failure = true;

private:
    int uncaught_on_creation_;
};

class OnSuccessPolicy {
public:
    explicit OnSuccessPolicy(bool execute)
        : uncaught_on_creation_(execute ? std::uncaught_exceptions() : -1) {
    }

    void release() noexcept {
        uncaught_on_creation_ = -1;
    }

    bool shouldExecute() const noexcept {
        return uncaught_on_creation_ != -1 && uncaught_on_creation_ >= std::uncaught_exceptions();
    }

    constexpr static bool execute_on_constructor_failure = false;

private:
    int uncaught_on_creation_;
};

template <typename EF, typename Policy> class ScopeGuardBase {
public:
    template <
        typename EFP, std::enable_if_t<!std::is_same_v<remove_cvref_t<EFP>, Policy>, int> = 0,
        std::enable_if_t<std::is_constructible_v<EF, EFP&>, int> = 0,
        std::enable_if_t<
            !std::is_lvalue_reference_v<EFP> && std::is_nothrow_constructible_v<EF, EFP>, int> = 0>
    explicit ScopeGuardBase(EFP&& f) noexcept(std::is_nothrow_constructible_v<EF, EFP> ||
                                              std::is_nothrow_constructible_v<EF, EFP&>)
        : exit_function(std::forward<EFP>(f)), execution_policy(true) {
    }

    template <typename EFP, std::enable_if_t<!std::is_same_v<remove_cvref_t<EFP>, Policy>, int> = 0,
              std::enable_if_t<std::is_constructible_v<EF, EFP&>, int> = 0,
              std::enable_if_t<std::is_lvalue_reference_v<EFP>, int> = 0>
    explicit ScopeGuardBase(EFP&& f) try : exit_function(f), execution_policy(true) {
    } catch (...) {
        // If initialisation of exit_function throws, call f() depending on the policy, then
        // rethrow.
        if constexpr (Policy::execute_on_constructor_failure) {
            f();
        }
        throw;
    }

    template <
        std::enable_if_t<
            std::is_nothrow_move_constructible_v<EF> || std::is_copy_constructible_v<EF>, int> = 0>
    ScopeGuardBase(ScopeGuardBase&& rhs) noexcept(std::is_nothrow_move_constructible_v<EF> ||
                                                  std::is_nothrow_copy_constructible_v<EF>)
        : exit_function(std::move_if_noexcept(rhs.exit_function)), execution_policy(false) {
        execution_policy = std::move(rhs.execution_policy);
        rhs.release();
    }

    ~ScopeGuardBase() noexcept(noexcept((std::declval<EF>())())) {
        if (execution_policy.shouldExecute()) {
            exit_function();
        }
    }

    void release() noexcept {
        execution_policy.release();
    }

    ScopeGuardBase(const ScopeGuardBase&) = delete;
    ScopeGuardBase& operator=(const ScopeGuardBase&) = delete;
    ScopeGuardBase& operator=(ScopeGuardBase&&) = delete;

private:
    EF exit_function;
    Policy execution_policy;
};
}  // namespace detail

template <typename EF> class ScopeExit : public detail::ScopeGuardBase<EF, detail::OnExitPolicy> {
public:
    using detail::ScopeGuardBase<EF, detail::OnExitPolicy>::ScopeGuardBase;
};
template <typename EF> ScopeExit(EF)->ScopeExit<EF>;

template <typename EF> class ScopeFail : public detail::ScopeGuardBase<EF, detail::OnFailPolicy> {
public:
    using detail::ScopeGuardBase<EF, detail::OnFailPolicy>::ScopeGuardBase;
};
template <typename EF> ScopeFail(EF)->ScopeFail<EF>;

template <typename EF>
class ScopeSuccess : public detail::ScopeGuardBase<EF, detail::OnSuccessPolicy> {
public:
    using detail::ScopeGuardBase<EF, detail::OnSuccessPolicy>::ScopeGuardBase;
};
template <typename EF> ScopeSuccess(EF)->ScopeSuccess<EF>;
}  // namespace dga
