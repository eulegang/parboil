#include "parboil.h"
#include <gtest/gtest.h>

using namespace parboil;

TEST(opt, parse) {
  {
    buffer buf{"hello"};
    const auto expected = "hello";
    const auto actual = opt<keyword<"hello">>::parse(buf);
    ASSERT_TRUE((bool)actual);

    EXPECT_EQ(actual, expected);
    EXPECT_EQ(buf, "");
  }

  {
    buffer buf{"world"};
    const auto expected = std::nullopt;
    const auto actual = opt<keyword<"hello">>::parse(buf);
    ASSERT_TRUE((bool)actual);

    EXPECT_EQ(actual, expected);
    EXPECT_EQ(buf, "world");
  }
}
