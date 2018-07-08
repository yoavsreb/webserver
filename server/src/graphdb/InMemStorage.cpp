#include "graphdb/InMemStorage.hpp"

namespace server {
namespace graphdb {

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
  return nodes.find(node1) != nodes.end();
}

bool InMemStorage::edgeExists(Id edgeId) const {
  return edges.find(edgeId) != edges.end();
}

boost::optional<Node> InMemStorage::getNode(Id nodeId) const {
  auto iter = nodes.find(nodeId);
  if (iter == nodes.end()) {
    return boost::none;
  } else {
    return boost::optional<Node>(Node(iter->second));
  }
}
boost::optional<Edge> InMemStorage::getEdge(Id edgeId) const {
  auto iter = edges.find(edgeId);
  if (iter == edges.end()) {
    return boost::none;
  } else {
    return boost::optional<Edge>(Edge(iter->second));
  }
}

std::vector<Edge> InMemStorage::getIncomingEdges(Id nodeId) const {
  std::vector<Edge> retval;
  std::for_each(edges.begin(), edges.end(),
                [&retval, nodeId](const std::pair<Id, Edge> &e) {
                  if (e.second.v2 == nodeId) {
                    retval.push_back(e.second);
                  }
                });
  return retval;
}

std::vector<Edge> InMemStorage::getOutgoingEdges(Id nodeId) const {
  std::vector<Edge> retval;
  std::for_each(edges.begin(), edges.end(),
                [&retval, nodeId](const std::pair<Id, Edge> &e) {
                  if (e.second.v1 == nodeId) {
                    retval.push_back(e.second);
                  }
                });
  return retval;
}

std::vector<Node> InMemStorage::getNodesByType(NodeType type) const {
  std::vector<Node> retval;
  std::for_each(nodes.begin(), nodes.end(),
                [&retval, type](const std::pair<Id, Node> &pair) {
                  if (pair.second.nodeType == type) {
                    retval.emplace_back(pair.second);
                  }
                });
  return retval;
}
}
}
