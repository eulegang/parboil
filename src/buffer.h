#ifndef PARBOIL_BUFFER_H
#define PARBOIL_BUFFER_H

#include "parboil.h"
#include <string_view>

namespace parboil {
struct buffer {
  buffer(std::string_view view) : view{view}, pos{0} {}
  buffer &operator+=(std::size_t) noexcept;
  buffer &operator++() noexcept;
  buffer operator++(int) noexcept;
  operator bool() noexcept;

  result<std::string_view> slice() const noexcept;
  result<std::string_view> slice(std::size_t) const noexcept;

private:
  std::string_view view;
  size_t pos;
};

} // namespace parboil

#endif
