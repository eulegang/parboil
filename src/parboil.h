#ifndef PARBOIL_H
#define PARBOIL_H

#include <array>
#include <cassert>
#include <cstdint>
#include <expected>
#include <string_view>
#include <vector>

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

struct buffer {
  buffer(std::string_view view)
      : rel{view.data()}, len{static_cast<uint32_t>(view.size())}, pos{0} {
    assert(view.size() < UINT32_MAX);
  }
  buffer &operator+=(std::size_t) noexcept;
  buffer &operator++() noexcept;
  buffer operator++(int) noexcept;
  char operator*() noexcept;
  operator bool() noexcept;

  result<std::string_view> slice() const noexcept;
  result<std::string_view> slice(std::size_t) const noexcept;

private:
  const char *rel;
  uint32_t len;
  uint32_t pos;
};

template <typename T>
concept SubParser = requires(buffer &buf) {
  typename T::Value;
  { T::parse(buf) } -> std::same_as<result<typename T::Value>>;
};

template <size_t N> struct kstr {
  std::array<char, N> value;
  constexpr kstr(const char *str) : value() {
    for (std::size_t i = 0; i < N - 1; ++i) {
      value[i] = str[i];
    }
    value[N - 1] = '\0';
  }

  static constexpr size_t size() { return N - 1; }
};
template <size_t N> kstr(const char (&)[N]) -> kstr<N>;

template <kstr T> struct keyword {
  using Value = std::string_view;

  static result<Value> parse(buffer &buf) {
    auto sub = buf.slice(T.size());
    if (!sub)
      return sub;

    buf += T.size();
    return sub;
  }
};

template <SubParser T, SubParser Sep, bool nonempty = false> struct sep {
  using Value = std::vector<typename T::Value>;

  static result<Value> parse(buffer &buf) {
    Value value;
    buffer snapshot = buf;

    if (nonempty) {
      auto first = T::parse(buf);

      if (!first) {
        return std::unexpected(first.error());
      }

      value.push_back(*first);
      snapshot = buf;

      auto sep = Sep::parse(buf);

      if (!sep) {
        buf = snapshot;
        return value;
      }
    }

    bool first = true;
    while (true) {
      const auto part = T::parse(buf);
      if (!part) {
        if (first) {
          first = false;
          return value;
        } else {
          return std::unexpected(part.error());
        }
      }

      value.push_back(*part);
      snapshot = buf;

      const auto sep = Sep::parse(buf);
      if (!sep) {
        buf = snapshot;
        return value;
      }
    }
  }
};

enum class byte_size {
  byte,
  word,
  dword,
  qword,
};

enum class base {
  bin,
  oct,
  dec,
  hex,
};

template <byte_size size, bool sign> struct num_type {};
template <> struct num_type<byte_size::byte, false> {
  using value = uint8_t;
};
template <> struct num_type<byte_size::word, false> {
  using value = uint16_t;
};
template <> struct num_type<byte_size::dword, false> {
  using value = uint32_t;
};
template <> struct num_type<byte_size::qword, false> {
  using value = uint64_t;
};

template <> struct num_type<byte_size::byte, true> {
  using value = int8_t;
};
template <> struct num_type<byte_size::word, true> {
  using value = int16_t;
};
template <> struct num_type<byte_size::dword, true> {
  using value = int32_t;
};
template <> struct num_type<byte_size::qword, true> {
  using value = int64_t;
};

template <base base> struct injester {
  uint64_t value;

  injester() : value{0} {}

  bool injest(char input) { return false; };
};

template <> struct injester<base::hex> {
  uint64_t value;
  injester() : value{0} {}

  bool injest(char input) {
    uint64_t i;

    if ('0' <= input && input <= '9') {
      i = input - '0';
    } else if ('a' <= input && input <= 'f') {
      i = input - 'a' + 10;
    } else if ('A' <= input && input <= 'F') {
      i = input - 'A' + 10;
    } else {
      return false;
    }

    value *= 16;
    value += i;

    return true;
  }
};

template <> struct injester<base::dec> {
  uint64_t value;
  injester() : value{0} {}

  bool injest(char input) {

    if (input < '0' || input > '9') {
      return false;
    }

    value *= 10;
    value += input - '0';

    return true;
  }
};

template <> struct injester<base::oct> {
  uint64_t value;
  injester() : value{0} {}

  bool injest(char input) {
    if (input < '0' || input > '7') {
      return false;
    }

    value *= 8;
    value += input - '0';

    return true;
  }
};

template <> struct injester<base::bin> {
  uint64_t value;
  injester() : value{0} {}

  bool injest(char input) {
    if (input < '0' || input > '1') {
      return false;
    }

    value *= 2;
    value += input - '0';

    return true;
  }
};

template <byte_size size = byte_size::qword, base base = base::dec,
          bool sign = true>
struct number {
  using Value = num_type<size, sign>::value;

  static result<Value> parse(buffer &buf) {
    bool neg = false;
    injester<base> injester;

    if (sign) {
      if (*buf == '-') {
        neg = true;
        buf++;
      }
    }

    while (injester.injest(*buf++))
      ;

    Value v = static_cast<Value>(injester.value);

    return neg ? -v : v;
  }
};

} // namespace parboil

#endif
