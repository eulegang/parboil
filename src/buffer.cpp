#include "parboil.h"
#include <cmath>

using namespace parboil;

buffer::operator bool() noexcept { return len != 0; }

buffer &buffer::operator+=(std::size_t n) noexcept {
  int steps = std::min(n, static_cast<std::size_t>(len));

  rel += steps;
  pos += steps;
  len -= steps;

  return *this;
}

buffer &buffer::operator++() noexcept {
  if (len != 0) {
    rel++;
    len--;
    pos++;
  }

  return *this;
}

buffer buffer::operator++(int) noexcept {
  buffer res = *this;
  if (len != 0) {
    rel++;
    len--;
    pos++;
  }
  return res;
}

result<std::string_view> buffer::slice() const noexcept {
  return std::string_view(rel, len);
}

result<std::string_view> buffer::slice(std::size_t sub_len) const noexcept {
  if (sub_len > len) {
    return std::unexpected(error_t{.code = code_t::oom, .position = pos});
  }

  return std::string_view(rel, sub_len);
}
