#ifndef PARBOIL_BUFFER_H
#define PARBOIL_BUFFER_H

#include "parboil.h"
#include <string_view>

namespace parboil {
struct buffer {
  buffer(std::string_view view) : view{view} {}
  buffer &operator++();
  buffer operator++(int);
  operator bool();

  result<std::string_view> slice() const;
  result<std::string_view> slice(std::size_t) const;

private:
  std::string_view view;
};

} // namespace parboil

#endif
