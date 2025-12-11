
#include "buffer.h"

using namespace parboil;

buffer::operator bool() { return !view.empty(); }

buffer &buffer::operator++() {
  view = view.substr(1);
  return *this;
}

buffer buffer::operator++(int) {
  buffer res = *this;
  view = view.substr(1);
  return res;
}

result<std::string_view> buffer::slice() const { return view; }
result<std::string_view> buffer::slice(std::size_t len) const {
  if (len >= view.size()) {
    return std::unexpected(code::oom);
  }

  return this->view.substr(0, len);
}
