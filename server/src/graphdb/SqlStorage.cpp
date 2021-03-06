#include "graphdb/SqlStorage.hpp"
#include <boost/iterator/iterator_facade.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace server {
namespace graphdb {

/**
 * RAII wrapper for Sql statement objects
 */
class SqlStmtHander {
public:
  SqlStmtHander(sqlite3_stmt *s) : stmt(s) {}
  SqlStmtHander() : stmt(nullptr) {}

  sqlite3_stmt **statementPtr() { return &stmt; }

  sqlite3_stmt *statement() { return stmt; }

  ~SqlStmtHander() {
    if (stmt) {
      sqlite3_finalize(stmt);
    }
  }

private:
  sqlite3_stmt *stmt;
};

/**
 * An iterator used to iterate over Sqlite Prepared and Binded SELECT statements
 * (i.e. query results)
 */

class SqliteResultsIterator
    : public boost::iterator_facade<SqliteResultsIterator, int,
                                    boost::forward_traversal_tag> {
public:
  SqliteResultsIterator(sqlite3_stmt *stmt) : pStmt(stmt) { increment(); }

  SqliteResultsIterator() : pStmt(nullptr) {}

  std::string getStringColumn(int column) const {
    return std::string{
        reinterpret_cast<const char *>(sqlite3_column_text(pStmt, column))};
  }

  int getIntColumn(int column) const {
    return sqlite3_column_int(pStmt, column);
  }

private:
  friend class boost::iterator_core_access;

  void increment() {
    auto s = sqlite3_step(pStmt);
    if (s != SQLITE_ROW) {
      pStmt = nullptr;
    }
  }

  bool equal(SqliteResultsIterator const &other) const {
    return other.pStmt == pStmt;
  }

  int &dereference() {
    throw std::runtime_error("Don't dereference a Sqlite iterator.");
  }

  sqlite3_stmt *pStmt;
};

void executeStatment(const std::string &stmt, sqlite3 *pDB) {
  sqlite3_stmt *pStmt;
  if (SQLITE_OK !=
      sqlite3_prepare_v2(pDB, stmt.c_str(), stmt.size(), &pStmt, NULL)) {
    throw std::runtime_error{"Failed to create stmt: " + stmt};
  }
  SqlStmtHander handler(pStmt);
  sqlite3_step(pStmt);
}

/**
 * Returns NodeId from a nodes table
 **/
std::vector<Node> getNodesQuery(sqlite3_stmt *stmt) {
  std::vector<Node> retval;
  SqliteResultsIterator iterBegin(stmt);
  SqliteResultsIterator iterEnd;
  while (iterBegin != iterEnd) {
    // moving from const unsigned char to const char *
    auto id = createFromString(iterBegin.getStringColumn(0));
    auto nodeType = static_cast<NodeType>(iterBegin.getIntColumn(1));
    retval.push_back({id, nodeType});
    iterBegin++;
  }
  return retval;
}

/**
 * Both Edges and Nodes tables have the ID as the
 * first column
 **/
std::vector<Edge> getEdgesQuery(sqlite3_stmt *stmt) {
  std::vector<Edge> retval;
  SqliteResultsIterator iterBegin(stmt);
  SqliteResultsIterator iterEnd;
  while (iterBegin != iterEnd) {
    // moving from const unsigned char to const char *
    auto id = createFromString(iterBegin.getStringColumn(0));

    auto nodeid1 = createFromString(iterBegin.getStringColumn(2));
    auto nodeid2 = createFromString(iterBegin.getStringColumn(3));
    int nodeType = iterBegin.getIntColumn(1);
    retval.push_back({id, static_cast<Edgetype>(nodeType), nodeid1, nodeid2});
    iterBegin++;
  }
  return retval;
}

SqlStorage::SqlStorage(const char *path) : pDB(nullptr) {
  if (sqlite3_open(path, &pDB) != SQLITE_OK) {
    throw std::runtime_error{std::string{"Could not open "} + path};
  }

  std::string createNodes{
      "CREATE TABLE IF NOT EXISTS nodes(id TEXT PRIMARY KEY, type INTEGER);"};

  executeStatment(createNodes, pDB);

  std::string createEdges{"CREATE TABLE IF NOT EXISTS edges(id TEXT PRIMARY "
                          "KEY, type INTEGER, v1 TEXT, v2 TEXT);"};
  executeStatment(createEdges, pDB);
}

SqlStorage::~SqlStorage() { sqlite3_close(pDB); }

Id SqlStorage::createNewNode(NodeType type) {
  auto newId = createRandomUUID();
  std::stringstream ss;
  ss << "INSERT INTO nodes VALUES ('" << newId << "', "
     << static_cast<uint32_t>(type) << ");";
  executeStatment(ss.str(), pDB);
  return newId;
}

Id SqlStorage::createNewEdge(Id node1, Id node2, Edgetype eType) {
  auto newId = createRandomUUID();
  std::stringstream ss;
  ss << "INSERT INTO edges VALUES ('" << newId << "', "
     << static_cast<uint32_t>(eType) << ", '" << node1 << "','" << node2
     << "');";
  executeStatment(ss.str(), pDB);
  return newId;
}

bool SqlStorage::nodeExists(Id node1) const {
  SqlStmtHander handler;

  std::string nodeSelectQuery = "SELECT * FROM nodes where id=?;";
  if (SQLITE_OK != sqlite3_prepare_v2(pDB, nodeSelectQuery.c_str(), -1,
                                      handler.statementPtr(), nullptr)) {
    throw std::runtime_error{std::string{"Failed to prepare nodes query"}};
  }
  std::string id = to_string(node1);
  if (SQLITE_OK != sqlite3_bind_text(handler.statement(), 1, id.c_str(),
                                     id.size(), SQLITE_STATIC)) {
    std::cout << "Failed to bind" << std::endl;
  }
  auto ids = getNodesQuery(handler.statement());
  return ids.size() > 0;
}

bool SqlStorage::edgeExists(Id edgeId) const {
  SqlStmtHander handler;
  std::string edgeSelectQuery = "SELECT * FROM edges where id=?;";
  if (SQLITE_OK != sqlite3_prepare_v2(pDB, edgeSelectQuery.c_str(), -1,
                                      handler.statementPtr(), nullptr)) {
    throw std::runtime_error{std::string{"Failed to prepare edges query"}};
  }
  std::string id = to_string(edgeId);
  if (SQLITE_OK != sqlite3_bind_text(handler.statement(), 1, id.c_str(),
                                     id.size(), SQLITE_STATIC)) {
    std::cout << "Failed to bind" << std::endl;
  }
  auto ids = getEdgesQuery(handler.statement());
  return ids.size() > 0;
}

boost::optional<Node> SqlStorage::getNode(Id nodeId) const {
  SqlStmtHander handler;
  std::string nodeSelectQuery = "SELECT * FROM nodes where id=?;";
  if (SQLITE_OK != sqlite3_prepare_v2(pDB, nodeSelectQuery.c_str(), -1,
                                      handler.statementPtr(), nullptr)) {
    throw std::runtime_error{std::string{"Failed to prepare nodes query"}};
  }
  std::string id = to_string(nodeId);
  if (SQLITE_OK != sqlite3_bind_text(handler.statement(), 1, id.c_str(),
                                     id.size(), SQLITE_STATIC)) {
    std::cout << "Failed to bind" << std::endl;
  }
  auto ids = getNodesQuery(handler.statement());
  if (ids.empty()) {
    return boost::none;
  } else {
    return boost::optional<Node>{ids.front()};
  }
}

boost::optional<Edge> SqlStorage::getEdge(Id edgeId) const {
  SqlStmtHander handler;
  std::string edgeSelectQuery = "SELECT * FROM edges where id=?;";
  if (SQLITE_OK != sqlite3_prepare_v2(pDB, edgeSelectQuery.c_str(), -1,
                                      handler.statementPtr(), nullptr)) {
    throw std::runtime_error{std::string{"Failed to prepare edges query"}};
  }
  std::string id = to_string(edgeId);
  if (SQLITE_OK != sqlite3_bind_text(handler.statement(), 1, id.c_str(),
                                     id.size(), SQLITE_STATIC)) {
    std::cout << "Failed to bind" << std::endl;
  }
  auto ids = getEdgesQuery(handler.statement());
  if (ids.empty()) {
    return boost::none;
  } else {
    return boost::optional<Edge>{ids.front()};
  }
}

std::vector<Edge> SqlStorage::getIncomingEdges(Id nodeId) const {
  SqlStmtHander handler;
  std::string query = "SELECT * FROM edges WHERE v2=?;";

  if (SQLITE_OK != sqlite3_prepare_v2(pDB, query.c_str(), -1,
                                      handler.statementPtr(), nullptr)) {
    throw std::runtime_error{std::string{"Failed to prepare edges query"}};
  }
  std::string id = to_string(nodeId);
  if (SQLITE_OK != sqlite3_bind_text(handler.statement(), 1, id.c_str(),
                                     id.size(), SQLITE_STATIC)) {
    std::cout << "Failed to bind" << std::endl;
  }
  return getEdgesQuery(handler.statement());
}

std::vector<Edge> SqlStorage::getOutgoingEdges(Id nodeId) const {
  SqlStmtHander handler;
  std::string query = "SELECT * FROM edges WHERE v1=?;";

  if (SQLITE_OK != sqlite3_prepare_v2(pDB, query.c_str(), -1,
                                      handler.statementPtr(), nullptr)) {
    throw std::runtime_error{std::string{"Failed to prepare edges query"}};
  }
  std::string id = to_string(nodeId);
  if (SQLITE_OK != sqlite3_bind_text(handler.statement(), 1, id.c_str(),
                                     id.size(), SQLITE_STATIC)) {
    std::cout << "Failed to bind" << std::endl;
  }
  return getEdgesQuery(handler.statement());
}

std::vector<Node> SqlStorage::getNodesByType(NodeType type) const {
  SqlStmtHander handler;
  std::string query = "SELECt * FROM nodes WHERE type=?";

  if (SQLITE_OK != sqlite3_prepare_v2(pDB, query.c_str(), -1,
                                      handler.statementPtr(), nullptr)) {
    throw std::runtime_error{std::string{"Failed to prepare edges query"}};
  }
  if (SQLITE_OK != sqlite3_bind_int64(handler.statement(), 1,
                                      static_cast<sqlite_int64>(type))) {
    std::cout << "Failed to bind" << std::endl;
  }
  return getNodesQuery(handler.statement());
}
}
}
