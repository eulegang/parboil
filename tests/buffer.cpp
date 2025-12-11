#include "buffer.h"
#include "parboil.h"
#include <gtest/gtest.h>

using namespace parboil;

long find_position(const parboil::buffer &buf) {
  auto len = buf.slice()->size();
  auto err = buf.slice(len + 1);

  if (!err) {
    return err.error().position;
  } else {
    throw std::runtime_error("expected to over slice");
  }
}

TEST(Buffer, empty_bool_conversion) {
  {
    parboil::buffer buf{""};
    EXPECT_FALSE((bool)buf) << "empty buffer should be false";
  }

  {
    parboil::buffer buf{"a"};
    EXPECT_TRUE((bool)buf) << "nonempty buffer should be true";
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
    EXPECT_EQ(find_position(buf), 0);
  }
}

TEST(Buffer, increment) {
  {
    parboil::buffer buf{"hello"};
    EXPECT_EQ((buf++).slice(), "hello");
    EXPECT_EQ(buf.slice(), "ello");

    EXPECT_EQ(find_position(buf), 1);
  }

  {
    parboil::buffer buf{"hello"};
    EXPECT_EQ((++buf).slice(), "ello");
    EXPECT_EQ(buf.slice(), "ello");
  }
}

TEST(Buffer, forward_seek) {
  {
    parboil::buffer buf{"hello"};
    buf += 3;

    EXPECT_EQ(buf.slice(), "lo") << "should only have lo left";
    EXPECT_EQ(find_position(buf), 3);
  }

  {
    parboil::buffer buf{"hello"};
    buf += 600;

    EXPECT_FALSE((bool)buf) << "should empty the buffer";
  }
}
