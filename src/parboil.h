#ifndef PARBOIL_H
#define PARBOIL_H

#include <array>
#include <cassert>
#include <cstdint>
#include <expected>
#include <iostream>
#include <optional>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

namespace parboil {

enum class code_t : std::uint64_t {
  expected,
  oom,
  overflow,
  incomplete,
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
  bool operator==(std::string_view view) const noexcept;
  char operator*() noexcept;
  operator bool() noexcept;
  friend std::ostream &operator<<(std::ostream &, const buffer &);

  error_t make_error(code_t) const noexcept;
  result<std::string_view> slice() const noexcept;
  result<std::string_view> slice(std::size_t) const noexcept;

private:
  const char *rel;
  uint32_t len;
  uint32_t pos;
};

std::ostream &operator<<(std::ostream &, const buffer &);

template <typename T>
concept SubParser = requires(buffer &buf) {
  typename T::Value;
  { T::parse(buf) } -> std::same_as<result<typename T::Value>>;
};

template <SubParser T> T::Value parse(std::string_view view) {
  buffer buf(view);

  auto res = T::parse(buf);

  if (!res) {
    throw res.error();
  }

  if (buf) {
    throw buf.make_error(code_t::incomplete);
  }

  return *res;
}

template <size_t N> struct kstr {
  std::array<char, N> value;
  constexpr kstr(const char *str) : value() {
    for (std::size_t i = 0; i < N - 1; ++i) {
      value[i] = str[i];
    }
    value[N - 1] = '\0';
  }

  bool operator==(std::string_view view) const {
    std::string_view me(value.data(), N - 1);

    return me == view;
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

    if (!(T == sub.value())) {
      return std::unexpected(buf.make_error(code_t::expected));
    }

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
  static const uint64_t MAX_VALUE = UINT8_MAX;
};
template <> struct num_type<byte_size::word, false> {
  using value = uint16_t;
  static const uint64_t MAX_VALUE = UINT16_MAX;
};
template <> struct num_type<byte_size::dword, false> {
  using value = uint32_t;
  static const uint64_t MAX_VALUE = UINT32_MAX;
};
template <> struct num_type<byte_size::qword, false> {
  using value = uint64_t;
  static const uint64_t MAX_VALUE = UINT64_MAX;
};

template <> struct num_type<byte_size::byte, true> {
  using value = int8_t;
  static const uint64_t MAX_VALUE = INT8_MAX;
};
template <> struct num_type<byte_size::word, true> {
  using value = int16_t;
  static const uint64_t MAX_VALUE = INT16_MAX;
};
template <> struct num_type<byte_size::dword, true> {
  using value = int32_t;
  static const uint64_t MAX_VALUE = INT32_MAX;
};
template <> struct num_type<byte_size::qword, true> {
  using value = int64_t;
  static const uint64_t MAX_VALUE = INT64_MAX;
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
  using ty = num_type<size, sign>;
  using Value = ty::value;

  static result<Value> parse(buffer &buf) {
    buffer root(buf);
    bool neg = false;
    injester<base> injester;

    if (sign) {
      if (*buf == '-') {
        neg = true;
        buf++;
      }
    }

    size_t chars{};
    buffer snapshot(buf);
    while (injester.injest(*buf++)) {
      snapshot = buf;
      chars++;
    }
    buf = snapshot;

    if (chars == 0) {
      buf = root;
      return std::unexpected(buf.make_error(code_t::expected));
    }

    if (injester.value > ty::MAX_VALUE) {
      buf = root;
      return std::unexpected(buf.make_error(code_t::overflow));
    }

    Value v = static_cast<Value>(injester.value);

    return neg ? -v : v;
  }
};

template <SubParser T> struct opt {
  using Value = std::optional<typename T::Value>;

  static result<Value> parse(buffer &buf) {
    buffer snapshot = buf;
    auto res = T::parse(buf);

    if (!res) {
      buf = snapshot;
      return std::nullopt;
    } else {
      return *res;
    }
  }
};

template <SubParser T> using _GetSubParserValue = T::Value;

template <SubParser... T> struct seq;

template <SubParser First, SubParser... Rest> struct seq<First, Rest...> {
  using Value = std::tuple<typename First::Value, _GetSubParserValue<Rest>...>;

  static result<Value> parse(buffer &buf) {
    buffer snapshot(buf);
    auto first = First::parse(buf);

    if (!first) {
      buf = snapshot;
      return std::unexpected(buf.make_error(code_t::expected));
    }

    auto rest = seq<Rest...>::parse(buf);

    if (!rest) {
      buf = snapshot;
      return std::unexpected(buf.make_error(code_t::expected));
    }

    return std::tuple_cat(std::tie(*first), *rest);
  }
};

template <SubParser T> struct seq<T> {
  using Value = std::tuple<typename T::Value>;

  static result<Value> parse(buffer &buf) {
    buffer snapshot(buf);
    auto only = T::parse(buf);

    if (!only) {
      buf = snapshot;
      return std::unexpected(buf.make_error(code_t::expected));
    }

    return std::tie(*only);
  }
};

template <bool (*T)(char, size_t)> struct pred {
  using Value = std::string_view;

  static result<Value> parse(buffer &buf) {
    buffer it(buf);
    size_t index{};

    while (it) {
      if (!T(*it, index)) {
        break;
      }

      it++;
      index++;
    }

    if (index == 0) {
      return std::unexpected(buf.make_error(code_t::expected));
    } else {
      auto res = buf.slice(index);
      buf = it;
      return res;
    }
  }
};

template <SubParser... T> struct alt {
  using Value = std::variant<_GetSubParserValue<T>...>;

  template <SubParser... P> struct _inner;
  template <SubParser First, SubParser... Rest> struct _inner<First, Rest...> {
    static result<Value> parse(buffer &buf) {
      buffer snapshot(buf);

      auto cur = First::parse(buf);

      if (cur) {
        return *cur;
      }

      buf = snapshot;
      auto next = _inner<Rest...>::parse(buf);

      if (next) {
        return *next;
      }

      return std::unexpected(next.error());
    }
  };
  template <SubParser P> struct _inner<P> {
    static result<Value> parse(buffer &buf) {
      buffer snapshot(buf);
      auto only = P::parse(buf);

      if (!only) {
        buf = snapshot;
        return std::unexpected(buf.make_error(code_t::expected));
      }

      return *only;
    };
  };

  static result<Value> parse(buffer &buf) { return _inner<T...>::parse(buf); }
};

template <SubParser T, bool nonempty = false> struct repeat {
  using Value = std::vector<typename T::Value>;

  static result<Value> parse(buffer &buf) {
    Value value;
    buffer snapshot = buf;

    if (nonempty) {
      const auto first = T::parse(buf);
      if (!first) {
        buf = snapshot;
        return std::unexpected(first.error());
      }

      value.push_back(*first);
    }

    while (true) {
      snapshot = buf;
      const auto part = T::parse(buf);

      if (!part) {
        buf = snapshot;

        if (nonempty && value.size() == 0) {
          return std::unexpected(part.error());
        } else {
          return value;
        }
      }

      value.push_back(*part);
    }
  }
};

} // namespace parboil

#endif
