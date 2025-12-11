
#include "buffer.h"

#include <cmath>

using namespace parboil;

buffer::operator bool() noexcept { return !view.empty(); }

buffer &buffer::operator+=(std::size_t n) noexcept {
  int steps = std::min(n, view.size());

  view = view.substr(steps);
  pos += steps;

  return *this;
}

buffer &buffer::operator++() noexcept {
  if (!view.empty()) {
    view = view.substr(1);
    pos++;
  }

  return *this;
}

buffer buffer::operator++(int) noexcept {
  buffer res = *this;
  if (!view.empty()) {
    view = view.substr(1);
    pos++;
  }
  return res;
}

result<std::string_view> buffer::slice() const noexcept { return view; }
result<std::string_view> buffer::slice(std::size_t len) const noexcept {
  if (len >= view.size()) {
    return std::unexpected(error_t{.code = code_t::oom, .position = pos});
  }

  return this->view.substr(0, len);
}
