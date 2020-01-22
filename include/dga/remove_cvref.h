/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#pragma once

#include <type_traits>

// A simple alias for std::remove_cv_t<std::remove_reference<T>>, backported from C++20.

namespace dga {
template <typename T> struct remove_cvref : std::remove_cv<std::remove_reference<T>> {};
template <typename T> using remove_cvref_t = typename remove_cvref<T>::type;
}  // namespace dga
