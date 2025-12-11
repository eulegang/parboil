#include "buffer.h"
#include "parboil.h"
#include <gtest/gtest.h>

TEST(Buffer, empty_bool_conversion) {
  {
    parboil::buffer buf{""};
    EXPECT_FALSE((bool)buf);
  }

  {
    parboil::buffer buf{"a"};
    EXPECT_TRUE((bool)buf);
  }
}

TEST(Buffer, slice) {
  {
    parboil::buffer buf{"hello"};
    EXPECT_EQ(buf.slice(), "hello");
  }

  {
    parboil::buffer buf{"hello"};
    EXPECT_EQ(buf.slice(3), "hel");
  }

  {
    parboil::buffer buf{"hello"};
    EXPECT_EQ(buf.slice(64), std::unexpected(parboil::code::oom));
  }
}

TEST(Buffer, increament) {
  {
    parboil::buffer buf{"hello"};
    EXPECT_EQ((buf++).slice(), "hello");
    EXPECT_EQ(buf.slice(), "ello");
  }

  {
    parboil::buffer buf{"hello"};
    EXPECT_EQ((++buf).slice(), "ello");
    EXPECT_EQ(buf.slice(), "ello");
  }
}
