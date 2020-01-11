/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dga/string_algorithms.h>

using testing::ElementsAre;
using testing::IsEmpty;

TEST(StrSplit, Empty) {
    std::vector<std::string> strings;
    dga::strSplit("", '-', std::back_inserter(strings));
    EXPECT_THAT(strings, IsEmpty());
}

TEST(StrSplit, Split) {
    std::vector<std::string> strings;
    dga::strSplit("100-200", '-', std::back_inserter(strings));
    EXPECT_THAT(strings, ElementsAre("100", "200"));
}

TEST(StrSplit, SplitLeadingAndTrailingDelim) {
    std::vector<std::string> strings;
    dga::strSplit("-100-200-", '-', std::back_inserter(strings));
    EXPECT_THAT(strings, ElementsAre("", "100", "200"));
}

TEST(StrSplit, SplitMultipleDelim) {
    std::vector<std::string> strings;
    dga::strSplit("100--200", '-', std::back_inserter(strings));
    EXPECT_THAT(strings, ElementsAre("100", "", "200"));
}

TEST(StrJoin, Empty) {
    std::vector<std::string> strings;
    EXPECT_EQ(dga::strJoin(strings.begin(), strings.end(), "-"), "");
}

TEST(StrJoin, SingleElement) {
    std::vector<std::string> strings;
    strings.emplace_back("100");
    EXPECT_EQ(dga::strJoin(strings.begin(), strings.end(), "-"), "100");
}

TEST(StrJoin, MultipleElements) {
    std::vector<std::string> strings;
    strings.emplace_back("100");
    strings.emplace_back("200");
    strings.emplace_back("300");
    EXPECT_EQ(dga::strJoin(strings.begin(), strings.end(), "-"), "100-200-300");
}

TEST(StrReplaceAll, Empty) {
    std::string input;
    EXPECT_EQ(dga::strReplaceAll(input, "a", "the"), input);
}

TEST(StrReplaceAll, NoMatch) {
    std::string input = "1234567890";
    EXPECT_EQ(dga::strReplaceAll(input, "a", "the"), input);
}

TEST(StrReplaceAll, SingleMatch) {
    std::string input = "this is a sentence";
    EXPECT_EQ(dga::strReplaceAll(input, "a", "the"), "this is the sentence");
}

TEST(StrReplaceAll, MultipleMatches) {
    std::string input = "ababababa";
    EXPECT_EQ(dga::strReplaceAll(input, "a", "c"), "cbcbcbcbc");
}

TEST(StrReplaceAll, ConsecutiveMatches) {
    std::string input = "aaaaa";
    EXPECT_EQ(dga::strReplaceAll(input, "a", "bb"), "bbbbbbbbbb");
}