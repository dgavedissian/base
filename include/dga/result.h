/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#pragma once

/*
 * Result type equivalent to Rust's Result<T, E>.
 *
 * Loosely based off the spec of std::expected:
 * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0323r4.html
 *
 * This object can be in two states, either in the OK state storing a T, or the error state storing
 * an E. It's also possible to construct an object Error<E> to distinguish between OK values and
 * error values in the case where the type is the same.
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

// Result type.

namespace detail {
struct uninitialised_tag {};

// Implements the storage of the result type. If either the value or error is trivially
// destructable, then ResultStorage should be trivially destructable.
template <class T, class E, bool = std::is_trivially_destructible_v<T>,
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
    constexpr explicit ResultStorage(const Error<G>& error)
        : error_(error.value()), has_value_(false) {
    }

    template <typename G, std::enable_if_t<std::is_convertible_v<G, E>>* = nullptr>
    constexpr explicit ResultStorage(Error<G>&& error)
        : error_(std::move(error).value()), has_value_(false) {
    }

    ~ResultStorage() {
        if (has_value_) {
            value_.~T();
        } else {
            error_.~Error<E>();
        }
    }

    union {
        T value_;
        Error<E> error_;
        char no_value_;
    };
    bool has_value_;
};

// Common operations used by storage types.
template <typename T, typename E> struct ResultStorageWithOperations : detail::ResultStorage<T, E> {
    using detail::ResultStorage<T, E>::ResultStorage;

    template <typename... Args> void constructValue(Args&&... args) noexcept {
        new (std::addressof(this->value_)) T(std::forward<Args>(args)...);
        this->has_value_ = true;
    }

    template <typename... Args> void constructError(Args&&... args) noexcept {
        new (std::addressof(this->error_)) Error<E>(std::forward<Args>(args)...);
        this->has_value_ = false;
    }
};
}  // namespace detail

template <typename T, typename E> class Result : detail::ResultStorageWithOperations<T, E> {
public:
    using value_type = T;
    using error_type = E;

    // Constructors.
    using detail::ResultStorageWithOperations<T, E>::ResultStorageWithOperations;

    constexpr Result(const Result& other) = default;
    constexpr Result(Result&& other) = default;

    template <typename U, typename G>
    explicit constexpr Result(const Result<U, G>& other)
        : detail::ResultStorageWithOperations<T, E>(detail::uninitialised_tag{}) {
        if (other.has_value()) {
            this->construct(*other);
        } else {
            this->constructError(other.error());
        }
    }

    template <typename U, typename G>
    explicit constexpr Result(Result<U, G>&& other)
        : detail::ResultStorageWithOperations<T, E>(detail::uninitialised_tag{}) {
        if (other.has_value()) {
            this->construct(std::move(*other));
        } else {
            this->constructError(std::move(other.error()));
        }
    }

    // Destructor.
    ~Result() = default;

    // Assignment.
    /*
    Result& operator=(const Result&);
    Result& operator=(Result&&) noexcept(true);
    template <class U = T> Result& operator=(U&&);
    template <class G = E> Result& operator=(const Error<G>&);
    template <class G = E> Result& operator=(Error<G>&&) noexcept(true);

    template <class... Args> void emplace(Args&&...);
    template <class U, class... Args> void emplace(std::initializer_list<U>, Args&&...);

    // �.�.4.4, swap
    void swap(Result&) noexcept(true);
     */

    // �.�.4.5, observers
    constexpr const T* operator->() const {
        // TODO: Throw exception if error.
        return &this->value_;
    }

    constexpr T* operator->() {
        // TODO: Throw exception if error.
        return &this->value_;
    }

    constexpr const T& operator*() const& {
        // TODO: Throw exception if error.
        return this->value_;
    }

    constexpr T& operator*() & {
        // TODO: Throw exception if error.
        return this->value_;
    }

    constexpr const T&& operator*() const&& {
        // TODO: Throw exception if error.
        return std::move(this->value_);
    }

    constexpr T&& operator*() && {
        // TODO: Throw exception if error.
        return std::move(this->value_);
    }

    constexpr explicit operator bool() const noexcept {
        return this->has_value_;
    }

    constexpr bool has_value() const noexcept {
        return this->has_value_;
    }

    constexpr const T& value() const& {
        // TODO: Throw exception if error.
        return this->value_;
    }

    constexpr T& value() & {
        // TODO: Throw exception if error.
        return this->value_;
    }

    constexpr const T&& value() const&& {
        // TODO: Throw exception if error.
        return std::move(this->value_);
    }

    constexpr T&& value() && {
        // TODO: Throw exception if error.
        return std::move(this->value_);
    }

    constexpr const E& error() const& {
        // TODO: Throw exception if value.
        return this->error_.value();
    }

    constexpr E& error() & {
        // TODO: Throw exception if value.
        return this->error_.value();
    }

    constexpr const E&& error() const&& {
        // TODO: Throw exception if value.
        return std::move(this->error_).value();
    }

    constexpr E&& error() && {
        // TODO: Throw exception if value.
        return std::move(this->error_).value();
    }

    template <class U> constexpr T value_or(U&& other_value) const& {
        static_assert(std::is_copy_constructible<T>::value && std::is_convertible<U&&, T>::value,
                      "T must be copy-constructible and convertible to from U&&");
        if (has_value()) {
            return value();
        } else {
            return static_cast<T>(std::forward<U>(other_value));
        }
    }

    template <class U> T value_or(U&& other_value) && {
        static_assert(std::is_copy_constructible<T>::value && std::is_convertible<U&&, T>::value,
                      "T must be copy-constructible and convertible to from U&&");
        if (has_value()) {
            return std::move(value());
        } else {
            return static_cast<T>(std::forward<U>(other_value));
        }
    }
};

// template <typename E> class Result<void, E> {};
}  // namespace dga
