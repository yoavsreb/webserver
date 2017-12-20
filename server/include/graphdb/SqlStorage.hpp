#ifndef _SERVER_GRAPHDB_SQLSTORAGE_HPP
#define _SERVER_GRAPHDB_SQLSTORAGE_HPP
#include "commons.hpp"
#include <sqlite3.h>

namespace server { namespace graphdb {
class SqlStorage {
public:
    SqlStorage(const char* path);
    ~SqlStorage();

    SqlStorage(const SqlStorage& other) = delete;
    SqlStorage operator=(const SqlStorage& other) = delete;

    Id createNewNode(NodeType type);
    Id createNewEdge(Id node1, Id node2, Edgetype eType);
    bool nodeExists(Id node1) const;

private:
    sqlite3 *pDB;
};
}}
#endif
