#ifndef PARBOIL_H
#define PARBOIL_H

#include <cstdint>
#include <expected>

namespace parboil {
enum class code : std::uint64_t {
  expected,
  oom,
};

template <typename T> using result = std::expected<T, code>;

} // namespace parboil

#endif
