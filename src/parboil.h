#ifndef PARBOIL_H
#define PARBOIL_H

#include <cstdint>
#include <expected>

namespace parboil {
enum class code_t : std::uint64_t {
  expected,
  oom,
};

struct error_t {
  code_t code;
  std::size_t position;

  bool operator==(const error_t &) const;
};

template <typename T> using result = std::expected<T, error_t>;

} // namespace parboil

#endif
