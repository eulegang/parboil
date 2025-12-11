#include "parboil.h"

using namespace parboil;

bool parboil::error_t::operator==(const parboil::error_t &other) const {
  return code == other.code && position == other.position;
}
