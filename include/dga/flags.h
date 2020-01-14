/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#pragma once

#include "../dga/aliases.h"

#include <bitset>
#include <type_traits>

/*
 * A type-safe collection of flags, that avoids having to work with bitmasking and shifting to
 * assign and retrieve flags. Additionally, enums that use this class don't require to be specified
 * in powers of two.
 *
 * An unchecked precondition of this class is that all flag values in `EnumType` are numbered from 0
 * to n, where n is the number of flags.
 */

namespace dga {
namespace detail {
// An enum type is a flag type if it has a _Count value
template <typename EnumType, typename = void> struct is_flags_enum : std::false_type {};

template <typename EnumType>
struct is_flags_enum<EnumType, decltype(static_cast<void>(EnumType::_Count))>
    : std::is_enum<EnumType> {
    static constexpr auto size = static_cast<std::size_t>(EnumType::_Count);
};
}  // namespace detail

template <typename EnumType> class Flags {
public:
    using mask_type = std::make_unsigned_t<std::underlying_type_t<EnumType>>;
    static_assert(detail::is_flags_enum<EnumType>::value,
                  "EnumType must be an 'enum' or 'enum class' that contains a _Count member.");

    static constexpr Flags all() {
        return Flags(
            mask_type((mask_type(1) << detail::is_flags_enum<EnumType>::size) - mask_type(1)));
    }

    static constexpr Flags none() {
        return Flags(mask_type(0));
    }

    // constructors.
    constexpr Flags() noexcept : value_(0) {
    }

    constexpr explicit Flags(EnumType e) noexcept : value_(mask(e)) {
    }

    constexpr Flags(const Flags& rhs) noexcept : value_(rhs.value_) {
    }

    constexpr explicit Flags(mask_type flags) noexcept : value_(flags) {
    }

    // operations.
    void set(EnumType e) {
        value_ |= mask(e);
    }

    void reset(EnumType e) {
        value_ &= ~mask(e);
    }

    void toggle(EnumType e) {
        value_ ^= mask(e);
    }

    void toggleAll() {
        value_ ^= all().value_;
    }

    constexpr bool isSet(EnumType e) const {
        return (value_ & mask(e)) != 0;
    }

    constexpr mask_type value() const noexcept {
        return value_;
    }

    // operators.
    Flags& operator|=(const Flags& rhs) noexcept {
        value_ |= rhs.value_;
        return *this;
    }

    Flags& operator&=(const Flags& rhs) noexcept {
        value_ &= rhs.value_;
        return *this;
    }

    Flags& operator^=(const Flags& rhs) noexcept {
        value_ ^= rhs.value_;
        return *this;
    }

    constexpr Flags operator|(const Flags& rhs) const noexcept {
        return Flags(value_ | rhs.value_);
    }

    constexpr Flags operator|(EnumType rhs) const noexcept {
        return Flags(value_ | mask(rhs));
    }

    constexpr Flags operator&(const Flags& rhs) const noexcept {
        return Flags(value_ & rhs.value_);
    }

    constexpr Flags operator&(EnumType rhs) const noexcept {
        return Flags(value_ & mask(rhs));
    }

    constexpr Flags operator^(const Flags& rhs) const noexcept {
        return Flags(value_ ^ rhs.value_);
    }

    constexpr Flags operator^(EnumType rhs) const noexcept {
        return Flags(value_ ^ mask(rhs));
    }

    constexpr bool operator!() const noexcept {
        return !value_;
    }

    constexpr Flags operator~() const noexcept {
        return Flags(value_ ^ all().value_);
    }

    constexpr bool operator==(const Flags& rhs) const noexcept {
        return value_ == rhs.value_;
    }

    constexpr bool operator==(EnumType rhs) const noexcept {
        return value_ == mask(rhs);
    }

    constexpr bool operator!=(const Flags& rhs) const noexcept {
        return value_ != rhs.value_;
    }

    constexpr bool operator!=(EnumType rhs) const noexcept {
        return value_ != mask(rhs);
    }

private:
    mask_type value_;

    static mask_type mask(EnumType e) {
        return mask_type(mask_type(1u) << std::size_t(e));
    }
};

template <typename EnumType>
constexpr Flags<EnumType> operator|(EnumType e, Flags<EnumType> const& flags) noexcept {
    return flags | e;
}

template <typename EnumType>
constexpr Flags<EnumType> operator&(EnumType e, Flags<EnumType> const& flags) noexcept {
    return flags & e;
}

template <typename EnumType>
constexpr Flags<EnumType> operator^(EnumType e, Flags<EnumType> const& flags) noexcept {
    return flags ^ e;
}

template <typename EnumType>
constexpr bool operator==(EnumType e, Flags<EnumType> const& flags) noexcept {
    return flags == e;
}

template <typename EnumType>
constexpr bool operator!=(EnumType e, Flags<EnumType> const& flags) noexcept {
    return flags != e;
}
}  // namespace dga
