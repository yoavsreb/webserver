#include "graphdb/InMemStorage.hpp"


namespace server { namespace graphdb {
    
Id InMemStorage::createNewNode(NodeType type) {
    auto id = createRandomUUID();
    nodes.emplace(id, Node{id, type});
    return id;
}

Id InMemStorage::createNewEdge(Id node1, Id node2, Edgetype eType) {
    auto id = createRandomUUID();
    edges.emplace(id, Edge{id, eType, node1, node2});
    return id;
}

// Getters

bool InMemStorage::nodeExists(Id node1) const {
    return false;
}

bool InMemStorage::edgeExists(Id edgeId) const {
    return false;
}

boost::optional<Node> InMemStorage::getNode(Id nodeId) const {
    return boost::none;
}
boost::optional<Edge> InMemStorage::getEdge(Id edgeId) const {
    return boost::none;
}

std::vector<Edge> InMemStorage::getIncomingEdges(Id nodeId) const {
    return std::vector<Edge>{};
}

std::vector<Edge> InMemStorage::getOutgoingEdges(Id nodeId) const {
    return std::vector<Edge>{};
}



}}
