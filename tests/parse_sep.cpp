
#include "parboil.h"
#include <gtest/gtest.h>

using namespace parboil;

TEST(Sep, parse) {
  parboil::buffer buf{"1,1,1"};
  const auto result = sep<keyword<"1">, keyword<",">>::parse(buf);

  std::vector<std::string_view> expected = {"1", "1", "1"};

  ASSERT_TRUE((bool)result);
  EXPECT_EQ(*result, expected);
  EXPECT_EQ(buf.slice(), "");
}

TEST(SepEmpty, empty) {
  parboil::buffer buf{""};
  const auto result = sep<keyword<"1">, keyword<",">, false>::parse(buf);

  std::vector<std::string_view> expected = {};

  ASSERT_TRUE((bool)result);
  EXPECT_EQ(*result, expected);
  EXPECT_EQ(buf.slice(), "");
}

TEST(SepNonempty, empty) {
  parboil::buffer buf{""};
  const auto result = sep<keyword<"1">, keyword<",">, true>::parse(buf);

  ASSERT_FALSE((bool)result);
  EXPECT_EQ(buf.slice(), "");
}
