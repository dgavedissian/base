/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#pragma once

#include "../dga/aliases.h"

namespace dga {
namespace detail {
template <typename Enum> struct EnumTraits {
    static constexpr std::size_t size() {
        return 1;
    }
};

template <int Size> struct FlagSetMaskType { using type = u32; };

template <typename Enum> class FlagsImpl {
public:
    using enum_traits = detail::EnumTraits<Enum>;
    using mask_type = typename detail::FlagSetMaskType<enum_traits::size()>::type;

    static constexpr FlagsImpl all() {
        return FlagsImpl(mask_type((mask_type(1) << enum_traits::size()) - mask_type(1)));
    }

    static constexpr FlagsImpl none() {
        return FlagsImpl(mask_type(0));
    }

    constexpr explicit FlagsImpl(Enum e) noexcept : value_(mask(e)) {
    }

    constexpr FlagsImpl(const FlagsImpl& rhs) noexcept : value_(rhs.value_) {
    }

    constexpr explicit FlagsImpl(mask_type flags) noexcept : value_(flags) {
    }

    constexpr FlagsImpl set(Enum e) const {
        return FlagsImpl(value_ | mask(e));
    }

    constexpr FlagsImpl reset(Enum e) const {
        return FlagsImpl(value_ & ~mask(e));
    }

    constexpr FlagsImpl toggle(Enum e) const {
        return FlagsImpl(value_ ^ mask(e));
    }

    constexpr FlagsImpl toggle_all() const {
        return FlagsImpl(value_ ^ all().to_int());
    }

    constexpr mask_type to_int() const {
        return value_;
    }

    constexpr bool is_set(Enum e) const {
        return (value_ & mask(e)) != mask_type(0u);
    }

    constexpr FlagsImpl bitwise_or(const FlagsImpl& other) const {
        return FlagsImpl(value_ | other.value_);
    }

    constexpr FlagsImpl bitwise_xor(const FlagsImpl& other) const {
        return FlagsImpl(value_ ^ other.value_);
    }

    constexpr FlagsImpl bitwise_and(const FlagsImpl& other) const {
        return FlagsImpl(value_ & other.value_);
    }

    /*
    FlagsImpl& operator|=(FlagsImpl const& rhs) noexcept {
        value_ |= rhs.value_;
        return *this;
    }

    FlagsImpl& operator&=(FlagsImpl const& rhs) noexcept {
        value_ &= rhs.value_;
        return *this;
    }

    FlagsImpl& operator^=(FlagsImpl const& rhs) noexcept {
        value_ ^= rhs.value_;
        return *this;
    }

    constexpr FlagsImpl operator|(FlagsImpl const& rhs) const noexcept {
        return FlagsImpl(value_ | rhs.value_);
    }

    constexpr FlagsImpl operator&(FlagsImpl const& rhs) const noexcept {
        return FlagsImpl(value_ & rhs.value_);
    }

    constexpr FlagsImpl operator^(FlagsImpl const& rhs) const noexcept {
        return FlagsImpl(value_ ^ rhs.value_);
    }

    constexpr bool operator!() const noexcept {
        return !value_;
    }

    constexpr FlagsImpl operator~() const noexcept {
        return FlagsImpl(value_ ^ mask_type(all()));
    }

    constexpr bool operator==(FlagsImpl const& rhs) const noexcept {
        return value_ == rhs.value_;
    }

    constexpr bool operator!=(FlagsImpl const& rhs) const noexcept {
        return value_ != rhs.value_;
    }
     */

private:
    mask_type value_;

    static mask_type mask(Enum e) {
        return mask_type(mask_type(1u) << std::size_t(e));
    }
};
}  // namespace detail

template <typename Enum> class FlagCombination : public detail::FlagsImpl<Enum> {
    using detail::FlagsImpl<Enum>::FlagsImpl;
};

template <typename Enum> class FlagMask : public detail::FlagsImpl<Enum> {
    using detail::FlagsImpl<Enum>::FlagsImpl;
};

template <typename Enum>
FlagCombination<Enum> operator|(const FlagCombination<Enum>& lhs,
                                const FlagCombination<Enum>& rhs) {
    return lhs.bitwise_or(rhs);
}

template <typename Enum>
FlagCombination<Enum> operator|(const FlagCombination<Enum>& lhs, const Enum& rhs) {
    return lhs.bitwise_or(FlagCombination<Enum>(rhs));
}

template <typename Enum>
FlagCombination<Enum> operator&(const FlagCombination<Enum>& lhs,
                                const FlagCombination<Enum>& rhs) {
    return lhs.bitwise_and(rhs);
}

template <typename Enum>
FlagCombination<Enum> operator&(const FlagCombination<Enum>& lhs, const Enum& rhs) {
    return lhs.bitwise_and(FlagCombination<Enum>(rhs));
}

template <typename Enum>

/*
template <typename Enum>
constexpr Flags<Enum> operator|(Enum bit, Flags<Enum> const& flags) noexcept {
    return flags | bit;
}

template <typename Enum>
constexpr Flags<Enum> operator&(Enum bit, Flags<Enum> const& flags) noexcept {
    return flags & bit;
}

template <typename Enum>
constexpr Flags<Enum> operator^(Enum bit, Flags<Enum> const& flags) noexcept {
    return flags ^ bit;
}

template <typename Enum>
constexpr bool operator==(Enum bit, Flags<Enum> const& flags) noexcept {
    return flags == bit;
}

template <typename Enum>
constexpr bool operator!=(Enum bit, Flags<Enum> const& flags) noexcept {
    return flags != bit;
}
 */
}  // namespace dga
