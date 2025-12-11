#include "buffer.h"
#include "parboil.h"
#include <gtest/gtest.h>

using namespace parboil;

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
    std::cout << buf.slice(64).has_value() << std::endl;
    EXPECT_EQ(buf.slice(64), std::unexpected(parboil::error_t{
                                 .code = code_t::oom, .position = 0}));
  }
}

TEST(Buffer, increment) {
  {
    parboil::buffer buf{"hello"};
    EXPECT_EQ((buf++).slice(), "hello");
    EXPECT_EQ(buf.slice(), "ello");

    EXPECT_EQ(buf.slice(64), std::unexpected(parboil::error_t{
                                 .code = code_t::oom, .position = 1}));
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
    EXPECT_EQ(buf.slice(50), std::unexpected(parboil::error_t{
                                 .code = code_t::oom, .position = 3}))
        << "should be on position 3";
  }

  {
    parboil::buffer buf{"hello"};
    buf += 600;

    EXPECT_FALSE((bool)buf) << "should empty the buffer";
  }
}
