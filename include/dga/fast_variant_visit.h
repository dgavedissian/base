/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#pragma once

#include <variant>
#include <type_traits>
#include <functional>

#if defined(_MSC_VER)
#define UNREACHABLE __assume(0)
#elif defined(__GNUC__)
#define UNREACHABLE __builtin_unreachable()
#else
#define UNREACHABLE void
#endif

namespace dga {
namespace detail {
template <typename Visitor, typename Variant> struct visit_helper {
    static constexpr std::size_t size = std::variant_size_v<std::decay_t<Variant>>;

    template <std::size_t I> static decltype(auto) invoke(Visitor&& visitor, Variant&& variant) {
        if constexpr (I < size) {
            return std::invoke(std::forward<Visitor>(visitor),
                               std::get<I>(std::forward<Variant>(variant)));
        } else {
            UNREACHABLE;
        }
    }

    template <std::size_t B> static decltype(auto) visit(Visitor&& visitor, Variant&& variant) {
        if constexpr (B < size) {
            switch (variant.index()) {
                case B + 0:
                    return invoke<B + 0>(std::forward<Visitor>(visitor),
                                         std::forward<Variant>(variant));
                case B + 1:
                    return invoke<B + 1>(std::forward<Visitor>(visitor),
                                         std::forward<Variant>(variant));
                case B + 2:
                    return invoke<B + 2>(std::forward<Visitor>(visitor),
                                         std::forward<Variant>(variant));
                case B + 3:
                    return invoke<B + 3>(std::forward<Visitor>(visitor),
                                         std::forward<Variant>(variant));
                case B + 4:
                    return invoke<B + 4>(std::forward<Visitor>(visitor),
                                         std::forward<Variant>(variant));
                case B + 5:
                    return invoke<B + 5>(std::forward<Visitor>(visitor),
                                         std::forward<Variant>(variant));
                case B + 6:
                    return invoke<B + 6>(std::forward<Visitor>(visitor),
                                         std::forward<Variant>(variant));
                case B + 7:
                    return invoke<B + 7>(std::forward<Visitor>(visitor),
                                         std::forward<Variant>(variant));
                case B + 8:
                    return invoke<B + 8>(std::forward<Visitor>(visitor),
                                         std::forward<Variant>(variant));
                case B + 9:
                    return invoke<B + 9>(std::forward<Visitor>(visitor),
                                         std::forward<Variant>(variant));
                case B + 10:
                    return invoke<B + 10>(std::forward<Visitor>(visitor),
                                          std::forward<Variant>(variant));
                case B + 11:
                    return invoke<B + 11>(std::forward<Visitor>(visitor),
                                          std::forward<Variant>(variant));
                case B + 12:
                    return invoke<B + 12>(std::forward<Visitor>(visitor),
                                          std::forward<Variant>(variant));
                case B + 13:
                    return invoke<B + 13>(std::forward<Visitor>(visitor),
                                          std::forward<Variant>(variant));
                case B + 14:
                    return invoke<B + 14>(std::forward<Visitor>(visitor),
                                          std::forward<Variant>(variant));
                case B + 15:
                    return invoke<B + 15>(std::forward<Visitor>(visitor),
                                          std::forward<Variant>(variant));
                default:
                    return visit<B + 16>(std::forward<Visitor>(visitor),
                                         std::forward<Variant>(variant));
            }
        } else {
            UNREACHABLE;
        }
    }
};
}  // namespace detail

template <typename Visitor, typename Variant>
decltype(auto) visit(Visitor&& visitor, Variant&& variant) {
    return ::dga::detail::visit_helper<Visitor, Variant>::template visit<0>(
        std::forward<Visitor>(visitor), std::forward<Variant>(variant));
}
}  // namespace dga
