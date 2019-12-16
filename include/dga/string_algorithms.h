/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#pragma once

#include <string>
#include <sstream>

namespace dga {
/// Splits a string, delimited by character 'delim' into multiple strings. These strings are stored
/// in the 'output_iterator' iterator.
template <typename OutputIt>
void strSplit(const std::string& s, char delim, OutputIt output_iterator) {
    std::istringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *output_iterator++ = item;
    }
}

/// Joins a range of strings specified with iterators 'first' and 'last' into a single string, with
/// a separator between each string.
template <typename InputIt>
std::string strJoin(InputIt first, InputIt last, const std::string& separator) {
    std::ostringstream ss;
    if (first != last) {
        ss << *first++;
    }
    while (first != last) {
        ss << separator;
        ss << *first++;
    }
    return ss.str();
}

/// Performs an exhaustive search and replace in string 'subject'.
inline std::string strReplaceAll(std::string subject, const std::string& search,
                           const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}
}  // namespace dga
