#include "parboil.h"
#include <gtest/gtest.h>

using namespace parboil;

bool ident(char ch, size_t index) {
  if ('a' <= ch && ch <= 'z') {
    return true;
  } else if ('A' <= ch && ch <= 'Z') {
    return true;
  } else if (ch == '_') {
    return true;
  } else if ('0' <= ch && ch <= '9') {
    return index != 0;
  } else {
    return false;
  }
}

TEST(pred, parse) {
  {
    buffer buf{"answer = 42"};
    const auto actual = pred<ident>::parse(buf);

    ASSERT_TRUE((bool)actual);
    EXPECT_EQ(actual, "answer");
    EXPECT_EQ(buf, " = 42");
  }

  {
    buffer buf{"*answer = 42"};
    const auto actual = pred<ident>::parse(buf);

    ASSERT_FALSE((bool)actual);
    EXPECT_EQ(buf, "*answer = 42");
  }
}
