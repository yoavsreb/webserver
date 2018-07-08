#include "graphdb/commons.hpp"

namespace server {
namespace graphdb {
Id createRandomUUID() {
  static auto generator = boost::uuids::random_generator();
  return Id{generator()};
}

Id createFromString(const std::string &str) {
  static boost::uuids::string_generator gen;
  return gen(str);
}
}
} // namespace server::graphdb
