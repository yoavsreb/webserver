#ifndef _SERVER_GRAPHDB_INMEMSTORAGE_HPP
#define _SERVER_GRAPHDB_INMEMSTORAGE_HPP
#include "commons.hpp"
#include <boost/optional.hpp>
#include <map>

namespace server { namespace graphdb {
class InMemStorage {
public:
    Id createNewNode(NodeType type);
    Id createNewEdge(Id node1, Id node2, Edgetype eType);
    
    // Getters
    bool nodeExists(Id node1) const;
    bool edgeExists(Id edgeId) const;

    boost::optional<Node> getNode(Id nodeId) const;
    boost::optional<Edge> getEdge(Id edgeId) const;

    std::vector<Edge> getIncomingEdges(Id nodeId) const;
    std::vector<Edge> getOutgoingEdges(Id nodeId) const;

    std::vector<Node> getNodesByType(NodeType type) const;
private:
    std::map<Id, Node> nodes;
    std::map<Id, Edge> edges;
};
}}
#endif

