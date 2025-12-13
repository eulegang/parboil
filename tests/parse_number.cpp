#include "parboil.h"
#include <gtest/gtest.h>

using namespace parboil;

TEST(number, dec) {
  buffer buf{"42"};
  const uint64_t expected = 42;
  const auto actual = number<byte_size::qword, base::dec, false>::parse(buf);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(actual, expected);
}

TEST(number, dec_neg) {
  {
    buffer buf{"42"};
    const int64_t expected = 42;
    const auto actual = number<byte_size::qword, base::dec, true>::parse(buf);

    ASSERT_TRUE((bool)actual);
    EXPECT_EQ(actual, expected);
  }

  {
    buffer buf{"-42"};
    const int64_t expected = -42;
    const auto actual = number<byte_size::qword, base::dec, true>::parse(buf);

    ASSERT_TRUE((bool)actual);
    EXPECT_EQ(actual, expected);
  }
}

TEST(number, oct) {
  buffer buf{"31"};
  const uint64_t expected = 25;
  const auto actual = number<byte_size::qword, base::oct, false>::parse(buf);

  ASSERT_TRUE((bool)actual);
  EXPECT_EQ(actual, expected);
}

TEST(number, overflow) {
  buffer buf{"300"};
  const auto actual = number<byte_size::byte, base::dec, false>::parse(buf);

  ASSERT_FALSE((bool)actual);
}
