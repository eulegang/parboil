
#include "parboil.h"
#include <type_traits>

using namespace parboil;

template <typename T>
class is_sub_parser : public std::bool_constant<parboil::SubParser<T>> {};

template <typename T> constexpr bool is_sub_parser_v = is_sub_parser<T>::value;

static_assert(is_sub_parser_v<keyword<"type">>);
static_assert(is_sub_parser_v<sep<keyword<"type">, keyword<",">>>);

static_assert(sizeof(buffer) == 16);
