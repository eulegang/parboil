
#include "parboil.h"
#include <gtest/gtest.h>

using namespace parboil;

TEST(Keyword, parse) {
  parboil::buffer buf{"hello, world"};
  const auto result = keyword<"hello">::parse(buf);

  EXPECT_EQ(*result, "hello");
  EXPECT_EQ(buf.slice(), ", world");
}

TEST(Keyword, parse_whole) {
  parboil::buffer buf{"hello"};
  const auto result = keyword<"hello">::parse(buf);

  ASSERT_TRUE((bool)result);
  EXPECT_EQ(*result, "hello");
  EXPECT_EQ(buf.slice(), "");
}
