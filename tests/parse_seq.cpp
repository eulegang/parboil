#include "parboil.h"
#include <gtest/gtest.h>

using namespace parboil;

TEST(Seq, parse__hello_world) {
  parboil::buffer buf{"hello,world"};
  const auto result =
      seq<keyword<"hello">, keyword<",">, keyword<"world">>::parse(buf);

  std::tuple<std::string_view, std::string_view, std::string_view> expected = {
      "hello", ",", "world"};

  ASSERT_TRUE((bool)result);
  EXPECT_EQ(*result, expected);
  EXPECT_EQ(buf, "");
}
