#ifndef SERVER_GRAPHDB_HPP
#define SERVER_GRAPHDB_HPP
#include "Logger.hpp"
#include "commons.hpp"
#include <boost/optional.hpp>

/** Assume following API for storage:
 *
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
*/

namespace server {
namespace graphdb {
using NodeSet = std::vector<Node>;
using NodeIdSet = std::vector<Id>;
using EdgeSet = std::vector<Edge>;
using EdgeIdSet = std::vector<Id>;

template <typename Storage> class GraphDB {
public:
  GraphDB(Storage &s, Logger &l) : storage(s), logger(l) {
    info(logger, "Inside GraphDB C'tor");
  }

  /**
   * Creates a node.
   */
  boost::optional<Node> createNode(NodeType type) {
    debug(logger, "Creating node");
    auto id = storage.createNewNode(type);
    debug(logger, concat("create new node:", id));
    return storage.getNode(id);
  }

  /**
   * Creates an edge.
   * If arguments are bad, will return boost::none
   */
  boost::optional<Edge> createEdge(Id n1, Id n2, Edgetype eType) {
    debug(logger, "Creating edge");
    if (!storage.nodeExists(n1) || !storage.nodeExists(n2)) {
      warn(logger, "At least one of the nodes doesn't exist");
      return boost::none;
    }
    auto edges = storage.getOutgoingEdges(n1);
    bool alreadyExists = std::any_of(
        edges.begin(), edges.end(), [n2](const Edge &e) { return e.v2 == n2; });
    if (alreadyExists) {
      warn(logger, "Edge already exists");
      return boost::none;
    } else {
      auto eId = storage.createNewEdge(n1, n2, eType);
      debug(logger, concat("created edge:", eId));
      return boost::optional<Edge>(storage.getEdge(eId));
    }
  }

  /**
   * Entry point for queries.
   */
  std::vector<Id> performQuery(void *ptr) { return std::vector<Id>{}; }

private:
  Storage &storage;
  Logger &logger;
};
}
}
#endif
