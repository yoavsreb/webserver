#ifndef _SERVER_GRAPHDB_COMMONS_HPP
#define _SERVER_GRAPHDB_COMMONS_HPP

#include <cstddef>
#include <boost/uuid/uuid.hpp>

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



}}
#endif
