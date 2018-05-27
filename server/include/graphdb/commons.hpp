#ifndef _SERVER_GRAPHDB_COMMONS_HPP
#define _SERVER_GRAPHDB_COMMONS_HPP

#include <cstddef>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace server { namespace graphdb {

using NodeType = uint8_t;
using Edgetype = uint8_t;
using Id = boost::uuids::uuid;

struct Node {
    Id nodeId;
    NodeType nodeType;

};

struct Edge {
    Id edgeId;
    Edgetype edgeType;
    Id v1;
    Id v2;
};


Id createRandomUUID();

Id createFromString(const std::string& str);
}}
#endif
