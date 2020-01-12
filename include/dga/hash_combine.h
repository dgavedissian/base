/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#pragma once

/*
 * Extended version of boost::hash_combine that supports multiple values at once.
 */

namespace dga {
inline void hashCombine(std::size_t& seed) {
}

template <typename T, typename... Ts>
inline void hashCombine(std::size_t& seed, const T& v, const Ts&... vs) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hashCombine(seed, vs...);
}
}  // namespace dga
