#ifndef _SERVER_GRAPHDB_SQLSTORAGE_HPP
#define _SERVER_GRAPHDB_SQLSTORAGE_HPP
#include "commons.hpp"
#include <sqlite3.h>
#include <boost/optional.hpp>

namespace server { namespace graphdb {
class SqlStorage {
public:
    SqlStorage(const char* path);
    ~SqlStorage();

    SqlStorage(const SqlStorage& other) = delete;
    SqlStorage operator=(const SqlStorage& other) = delete;

    Id createNewNode(NodeType type);
    Id createNewEdge(Id node1, Id node2, Edgetype eType);
    
    // Getters
    bool nodeExists(Id node1) const;
    bool edgeExists(Id edgeId) const;

    boost::optional<Node> getNode(Id nodeId) const;
    boost::optional<Edge> getEdge(Id edgeId) const;

private:
    sqlite3 *pDB;
};
}}
#endif
