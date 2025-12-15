#include "parboil.h"
#include <gtest/gtest.h>

using namespace parboil;

using parser = keyword<"hello">;

TEST(parse, full) {
  const auto expected = "hello";
  const auto actual = parse<parser>("hello");
  EXPECT_EQ(actual, expected);
}

TEST(parse, empty) {
  EXPECT_THROW({ parse<parser>(""); }, parboil::error_t);
}

TEST(parse, partial) {
  EXPECT_THROW({ parse<parser>("hello, world"); }, parboil::error_t);
}
