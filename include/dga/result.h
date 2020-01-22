/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#pragma once

#include <type_traits>
#include <utility>
#include <cassert>
#include <exception>
#include "../dga/remove_cvref.h"

/*
 * Result type similar to Rust's Result<T, E>.
 *
 * Loosely based off the spec of std::expected:
 * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0323r4.html
 *
 * Note: Not really fully exception safe yet, and is probably still missing a lot of std::enable_if
 * logic to guard against strange conversions. However, it's good enough for now.
 *
 * This object can be in two states, either in the value state storing a T, or the error state
 * storing an E. It's also possible to construct an object Error<E> to distinguish between OK values
 * and error values in the case where the type is the same.
 */

namespace dga {
// Error type.

template <typename E> class Error {
public:
    static_assert(!std::is_same_v<E, void>, "E must not be void.");

    Error() = delete;

    constexpr explicit Error(const E& value) : value_(value) {
    }

    constexpr explicit Error(E&& value) : value_(std::move(value)) {
    }

    Error& operator=(const E& value) {
        value_ = value;
        return *this;
    }

    Error& operator=(E&& value) {
        value_ = std::move(value);
        return *this;
    }

    constexpr const E& value() const& {
        return value_;
    }

    constexpr E& value() & {
        return value_;
    }

    constexpr const E&& value() const&& {
        return std::move(value_);
    }

    constexpr E&& value() && {
        return std::move(value_);
    }

private:
    E value_;
};

template <typename E> constexpr bool operator==(const Error<E>& lhs, const Error<E>& rhs) {
    return lhs.value() == rhs.value();
}

template <typename E> constexpr bool operator!=(const Error<E>& lhs, const Error<E>& rhs) {
    return lhs.value() != rhs.value();
}

template <typename T> struct is_error : std::false_type {};
template <typename E> struct is_error<Error<E>> : std::true_type {};
template <typename T> constexpr bool is_error_v = is_error<T>::value;

// Result type.
template <typename T, typename E> class Result;

// is_result type trait.
template <typename R> struct is_result : std::false_type {};
template <typename T, typename E> struct is_result<Result<T, E>> : std::true_type {};
template <typename R> constexpr auto is_result_v = is_result<R>::value;

namespace detail {
struct uninitialised_tag {};

struct void_placeholder {};

// Implements the storage of the result type. If either the value or error is trivially
// destructable, then ResultStorage should be trivially destructable.
template <typename T, typename E, bool = std::is_trivially_destructible_v<T>,
          bool = std::is_trivially_destructible_v<E>>
struct ResultStorage {
    constexpr ResultStorage() : value_(T{}), has_value_(true) {
    }

    // Special constructor in the case where we want to defer initialisation of the value to later
    // i.e. implementing a copy constructor of Result without having to initialise T or E first.
    constexpr ResultStorage(uninitialised_tag) : no_value_(), has_value_(false) {
    }

    template <typename U = T, std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
    constexpr explicit ResultStorage(U&& value) : value_(std::forward<U>(value)), has_value_(true) {
    }

    template <typename G, std::enable_if_t<std::is_convertible_v<G, E>>* = nullptr>
    constexpr ResultStorage(const Error<G>& error) : error_(error.value()), has_value_(false) {
    }

    template <typename G, std::enable_if_t<std::is_convertible_v<G, E>>* = nullptr>
    constexpr ResultStorage(Error<G>&& error)
        : error_(std::move(error).value()), has_value_(false) {
    }

    constexpr ResultStorage(const ResultStorage& other) {
        if (other.has_value_) {
        }
    }

    ~ResultStorage() {
        destruct();
    }

    void destruct() {
        if (has_value_) {
            destructValue();
        } else {
            destructError();
        }
    }

    void destructValue() {
        assert(has_value_);
        value_.~T();
    }

    void destructError() {
        assert(!has_value_);
        error_.~Error<E>();
    }

    union {
        T value_;
        Error<E> error_;
        char no_value_;
    };
    bool has_value_;
};

// TODO: Consider implementing specialisations for when T or E (or both) are trivially destructable.

// Void specialisation.
template <typename E> struct ResultStorage<void, E, false, false> {
    constexpr ResultStorage() : value_(), has_value_(true) {
    }

    // Special constructor in the case where we want to defer initialisation of the value to later
    // i.e. implementing a copy constructor of Result without having to initialise T or E first.
    constexpr ResultStorage(uninitialised_tag) : no_value_(), has_value_(false) {
    }

    template <typename G, std::enable_if_t<std::is_convertible_v<G, E>>* = nullptr>
    constexpr ResultStorage(const Error<G>& error) : error_(error.value()), has_value_(false) {
    }

    template <typename G, std::enable_if_t<std::is_convertible_v<G, E>>* = nullptr>
    constexpr ResultStorage(Error<G>&& error)
        : error_(std::move(error).value()), has_value_(false) {
    }

    ~ResultStorage() {
        destruct();
    }

    void destruct() {
        if (!has_value_) {
            destructError();
        }
    }

    void destructValue() {
    }

    void destructError() {
        assert(!has_value_);
        error_.~Error<E>();
    }

    union {
        void_placeholder value_;
        Error<E> error_;
        char no_value_;
    };
    bool has_value_;
};

template <typename E> struct ResultStorage<void, E, false, true> {
    constexpr ResultStorage() : value_(), has_value_(true) {
    }

    // Special constructor in the case where we want to defer initialisation of the value to later
    // i.e. implementing a copy constructor of Result without having to initialise T or E first.
    constexpr ResultStorage(uninitialised_tag) : no_value_(), has_value_(false) {
    }

    template <typename G, std::enable_if_t<std::is_convertible_v<G, E>>* = nullptr>
    constexpr ResultStorage(const Error<G>& error) : error_(error.value()), has_value_(false) {
    }

    template <typename G, std::enable_if_t<std::is_convertible_v<G, E>>* = nullptr>
    constexpr ResultStorage(Error<G>&& error)
        : error_(std::move(error).value()), has_value_(false) {
    }

    ~ResultStorage() {
    }

    void destruct() {
    }

    void destructValue() {
    }

    void destructError() {
    }

    union {
        void_placeholder value_;
        Error<E> error_;
        char no_value_;
    };
    bool has_value_;
};

// Controls whether the result is default constructible. The default constructor should be deleted
// if the type is not default constructable and not void.
template <typename T, typename E, bool = std::is_default_constructible_v<T> || std::is_void_v<T>>
struct ResultStorageDefaultConstructible : ResultStorage<T, E> {
    using ResultStorage<T, E>::ResultStorage;
};

template <typename T, typename E>
struct ResultStorageDefaultConstructible<T, E, false> : ResultStorage<T, E> {
    using ResultStorage<T, E>::ResultStorage;

    // Delete default constructor.
    ResultStorageDefaultConstructible() = delete;
};

// Common operations used by storage types.
template <typename T, typename E>
struct ResultStorageWithOperations : ResultStorageDefaultConstructible<T, E> {
    using ResultStorageDefaultConstructible<T, E>::ResultStorageDefaultConstructible;

    template <typename... Args>
    void constructValue(Args&&... args) noexcept(std::is_nothrow_constructible_v<T>) {
        if constexpr (std::is_void_v<T>) {
            this->value_ = void_placeholder{};
        } else {
            new (std::addressof(this->value_)) T(std::forward<Args>(args)...);
        }
        this->has_value_ = true;
    }

    template <typename... Args>
    void constructError(Args&&... args) noexcept(std::is_nothrow_constructible_v<Error<E>>) {
        new (std::addressof(this->error_)) Error<E>(std::forward<Args>(args)...);
        this->has_value_ = false;
    }

    template <typename U, std::enable_if_t<!std::is_void_v<U>>* = nullptr>
    void assignValue(U&& value) {
        this->value_ = std::forward<U>(value);
        this->has_value_ = true;
    }

    void assignError(const E& value) {
        this->error_ = value;
        this->has_value_ = false;
    }

    void assignError(E&& value) {
        this->error_ = std::move(value);
        this->has_value_ = false;
    }
};
}  // namespace detail

template <typename E> class MissingResultValue : public std::exception {
public:
    explicit MissingResultValue(E e) : error_(e) {
    }

    virtual const char* what() const noexcept override {
        return "Missing result value.";
    }

    E& error() & {
        return error_;
    }

    const E& error() const& {
        return error_;
    }

    E&& error() && {
        return std::move(error_);
    }

    const E&& error() const&& {
        return std::move(error_);
    }

private:
    E error_;
};

template <> class MissingResultValue<void> : public std::exception {
public:
    explicit MissingResultValue() {
    }

    virtual const char* what() const noexcept override {
        return "Missing result value.";
    }
};

template <typename T, typename E> class Result : detail::ResultStorageWithOperations<T, E> {
public:
    using value_type = T;
    using error_type = E;

    // Constructors.
    using detail::ResultStorageWithOperations<T, E>::ResultStorageWithOperations;

    constexpr Result(const Result& other)
        : detail::ResultStorageWithOperations<T, E>(detail::uninitialised_tag{}) {
        if (other.has_value()) {
            this->constructValue(*other);
        } else {
            this->constructError(other.error());
        }
    }

    constexpr Result(Result&& other) noexcept(
        std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_constructible_v<E>)
        : detail::ResultStorageWithOperations<T, E>(detail::uninitialised_tag{}) {
        if (other.has_value()) {
            this->constructValue(std::move(*other));
        } else {
            this->constructError(std::move(other.error()));
        }
    }

    template <typename U, typename G>
    explicit constexpr Result(const Result<U, G>& other)
        : detail::ResultStorageWithOperations<T, E>(detail::uninitialised_tag{}) {
        if (other.has_value()) {
            this->constructValue(*other);
        } else {
            this->constructError(other.error());
        }
    }

    template <typename U, typename G>
    explicit constexpr Result(Result<U, G>&& other) noexcept(
        std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_constructible_v<E>)
        : detail::ResultStorageWithOperations<T, E>(detail::uninitialised_tag{}) {
        if (other.has_value()) {
            this->constructValue(std::move(*other));
        } else {
            this->constructError(std::move(other.error()));
        }
    }

    // Destructor.
    ~Result() = default;

    // Assignment.
    Result& operator=(const Result& other) {
        this->destruct();
        if (other.has_value()) {
            if constexpr (!std::is_void_v<T>) {
                this->assignValue(other.value());
            } else {
                this->constructValue();
            }
        } else {
            this->assignError(other.error());
        }
        return *this;
    }

    Result& operator=(Result&& other) noexcept(
        std::is_nothrow_move_assignable_v<T>&& std::is_nothrow_move_assignable_v<E>) {
        this->destruct();
        if (other.has_value()) {
            if constexpr (!std::is_void_v<T>) {
                this->assignValue(std::move(other).value());
            } else {
                this->constructValue();
            }
        } else {
            this->assignError(std::move(other).error());
        }
        return *this;
    }

    template <typename U, typename G> Result& operator=(const Result<U, G>& other) {
        this->destruct();
        if (other.has_value()) {
            if constexpr (!std::is_void_v<T>) {
                this->assignValue(other.value());
            } else {
                this->constructValue();
            }
        } else {
            this->assignError(other.error());
        }
        return *this;
    }

    template <typename U, typename G>
    Result& operator=(Result<U, G>&& other) noexcept(
        std::is_nothrow_move_assignable_v<T>&& std::is_nothrow_move_assignable_v<E>) {
        this->destruct();
        if (other.has_value()) {
            if constexpr (!std::is_void_v<T>) {
                this->assignValue(std::move(other).value());
            } else {
                this->constructValue();
            }
        } else {
            this->assignError(std::move(other).error());
        }
        return *this;
    }

    // Construct from a universal reference.
    // Requirements:
    // * U must be convertible to T.
    // * remove_cvref_t<U> must be not equal Result<T, E>
    // * remove_cvref_t<U> must not be an Error type.
    template <class U = T, std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr,
              std::enable_if_t<!std::is_same_v<remove_cvref_t<U>, Result>>* = nullptr,
              std::enable_if_t<!is_error_v<remove_cvref_t<U>>>* = nullptr>
    Result& operator=(U&& value) {
        if (has_value()) {
            this->value_ = std::forward<U>(value);
        } else {
            this->destructError();
            this->constructValue(std::forward<U>(value));
        }
        return *this;
    }

    template <class G = E> Result& operator=(const Error<G>& error) {
        if (!has_value()) {
            this->error_ = error;
        } else {
            this->destructValue();
            this->constructError(error);
        }
        return *this;
    }

    template <class G = E> Result& operator=(Error<G>&& error) {
        if (!has_value()) {
            this->error_ = std::move(error);
        } else {
            this->destructValue();
            this->constructError(std::move(error));
        }
        return *this;
    }

    template <class... Args,
              typename = std::enable_if_t<std::is_nothrow_constructible_v<T, Args&&...>>>
    void emplace(Args&&... args) {
        if (has_value()) {
            this->value_ = T(std::forward<Args>(args)...);
        } else {
            this->destructError();
            this->constructValue(std::forward<Args>(args)...);
        }
    }

    template <typename U = T, std::enable_if_t<std::is_void_v<U>>* = nullptr> void emplace() {
        if (!has_value()) {
            this->destructError();
            this->constructValue();
        }
    }

    // Swap.
    template <
        std::enable_if_t<std::is_nothrow_move_constructible_v<T> ||
                         std::is_nothrow_move_constructible_v<E> || std::is_void_v<T>>* = nullptr>
    void swap(Result& other) {
        if (has_value() && other.has_value()) {
            std::swap(this->value_, other.value_);
        } else if (!has_value() && !other.has_value()) {
            std::swap(this->error_, other.error_);
        } else if (!has_value() && other.has_value()) {
            other.swap(*this);
        } else {
            if constexpr (std::is_void_v<T>) {
                this->constructError(std::move(other.error_));
                other.destructError();
                other.constructValue();
            } else if constexpr (std::is_nothrow_move_constructible_v<E>) {
                auto tmp = std::move(other.error_);
                other.destructError();
                other.constructValue(std::move(this->value_));
                this->destructValue();
                this->constructError(std::move(tmp));
            } else if constexpr (std::is_nothrow_move_constructible_v<T>) {
                auto tmp = std::move(this->value_);
                this->destructValue();
                this->constructError(std::move(other->error_));
                other.destructError();
                other.constructValue(std::move(tmp));
            }
        }
    }

    // Observers.
    template <typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    constexpr const T* operator->() const {
        assert(has_value());
        return &this->value_;
    }

    template <typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    constexpr T* operator->() {
        assert(has_value());
        return &this->value_;
    }

    template <typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    constexpr const U& operator*() const& {
        assert(has_value());
        return this->value_;
    }

    template <typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    constexpr U& operator*() & {
        assert(has_value());
        return this->value_;
    }

    template <typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    constexpr const U&& operator*() const&& {
        assert(has_value());
        return std::move(this->value_);
    }

    template <typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    constexpr U&& operator*() && {
        assert(has_value());
        return std::move(this->value_);
    }

    constexpr explicit operator bool() const noexcept {
        return this->has_value_;
    }

    constexpr bool has_value() const noexcept {
        return this->has_value_;
    }

    template <typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    constexpr const U& value() const& {
        if (!has_value()) {
            throw MissingResultValue(this->error_.value());
        }
        return this->value_;
    }

    template <typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    constexpr U& value() & {
        if (!has_value()) {
            throw MissingResultValue(this->error_.value());
        }
        return this->value_;
    }

    template <typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    constexpr const U&& value() const&& {
        if (!has_value()) {
            throw MissingResultValue(std::move(this->error_).value());
        }
        return std::move(this->value_);
    }

    template <typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    constexpr U&& value() && {
        if (!has_value()) {
            throw MissingResultValue(std::move(this->error_).value());
        }
        return std::move(this->value_);
    }

    constexpr const E& error() const& {
        assert(!has_value());
        return this->error_.value();
    }

    constexpr E& error() & {
        assert(!has_value());
        return this->error_.value();
    }

    constexpr const E&& error() const&& {
        assert(!has_value());
        return std::move(this->error_).value();
    }

    constexpr E&& error() && {
        assert(!has_value());
        return std::move(this->error_).value();
    }

    template <typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    constexpr T value_or(U&& other_value) const& {
        static_assert(std::is_copy_constructible<T>::value && std::is_convertible<U&&, T>::value,
                      "T must be copy-constructible and convertible to from U&&");
        if (has_value()) {
            return value();
        } else {
            return static_cast<T>(std::forward<U>(other_value));
        }
    }

    template <typename U = T, typename = std::enable_if_t<!std::is_void_v<U>>>
    T value_or(U&& other_value) && {
        static_assert(std::is_copy_constructible<T>::value && std::is_convertible<U&&, T>::value,
                      "T must be copy-constructible and convertible to from U&&");
        if (has_value()) {
            return std::move(value());
        } else {
            return static_cast<T>(std::forward<U>(other_value));
        }
    }

    // A workaround to access the Error<T> wrapper when we have a reference to another Result type.
    // Due to private inheritance rules, we can't access other.error_ directly when implementing a
    // copy constructor for Result<U, G>.
    constexpr const Error<E>& wrapped_error() const& {
        assert(!has_value());
        return this->error_;
    }

    constexpr Error<E>& wrapped_error() & {
        assert(!has_value());
        return this->error_;
    }

    constexpr const Error<E>&& wrapped_error() const&& {
        assert(!has_value());
        return std::move(this->error_);
    }

    constexpr Error<E>&& wrapped_error() && {
        assert(!has_value());
        return std::move(this->error_);
    }
};
}  // namespace dga
