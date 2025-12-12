#ifndef PARBOIL_H
#define PARBOIL_H

#include <array>
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

} // namespace parboil

#endif
