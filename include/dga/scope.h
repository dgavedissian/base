/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#pragma once

#include <type_traits>

// Implementation of http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0052r10.pdf

namespace dga {
namespace detail {
// remove_cvref_t backported from C++20.
template <typename T> struct remove_cvref {
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};
template <typename T> using remove_cvref_t = typename remove_cvref<T>::type;

// general scope guard.
template <typename EF> class scope_guard {
public:
    template <
        typename EFP,
        typename std::enable_if_t<!std::is_same_v<remove_cvref_t<EFP>, scope_guard>, int> = 0,
        typename std::enable_if_t<std::is_constructible_v<EF, EFP&>, int> = 0,
        typename std::enable_if_t<
            !std::is_lvalue_reference_v<EFP> && std::is_nothrow_constructible_v<EF, EFP>, int> = 0>
    explicit scope_guard(EFP&& f) noexcept(std::is_nothrow_constructible_v<EF, EFP> ||
                                           std::is_nothrow_constructible_v<EF, EFP&>)
        : exit_function{std::forward<EFP>(f)} {
    }

    template <typename EFP,
              typename std::enable_if_t<!std::is_same_v<remove_cvref_t<EFP>, scope_guard>, int> = 0,
              typename std::enable_if_t<std::is_constructible_v<EF, EFP&>, int> = 0,
              typename std::enable_if_t<std::is_lvalue_reference_v<EFP>, int> = 0>
    explicit scope_guard(EFP&& f) try : exit_function(f) {
    } catch (...) {
        // If initialisation of exit_function throws, call f() then rethrow.
        f();
        throw;
    }

    template <typename = std::enable_if_t<std::is_nothrow_move_constructible_v<EF>>,
              typename = std::enable_if_t<std::is_copy_constructible_v<EF>>>
    scope_guard(scope_guard&& rhs) noexcept(std::is_nothrow_move_constructible_v<EF> ||
                                            std::is_nothrow_copy_constructible_v<EF>) {
        if constexpr (std::is_nothrow_move_constructible_v<EF>) {
            exit_function = std::forward<EF>(rhs.exit_function);
        } else {
            exit_function = rhs.exit_function;
        }
        execute_on_destruction = rhs.execute_on_destruction;
    }

    ~scope_guard() noexcept {
        if (execute_on_destruction) {
            exit_function();
        }
    }

    void release() noexcept {
        execute_on_destruction = false;
    }

    scope_guard(const scope_guard&) = delete;
    scope_guard& operator=(const scope_guard&) = delete;
    scope_guard& operator=(scope_guard&&) = delete;

private:
    EF exit_function;
    bool execute_on_destruction{true};
};
}  // namespace detail

template <typename EF> class scope_exit : public detail::scope_guard<EF> {
public:
    using detail::scope_guard<EF>::scope_guard;
};

template <typename EF> scope_exit(EF)->scope_exit<EF>;

// template <typename EF> class scope_fail;

// template <typename EF> class scope_success;

template <typename R, typename D> class unique_resource;

template <typename R, typename D, typename S = std::decay_t<R>>
unique_resource<std::decay_t<R>, std::decay_t<D>> make_unique_resource_checked(R&& r,
                                                                               const S& invalid,
                                                                               D&& d) noexcept {
}
}  // namespace dga
