#include "graphdb/SqlStorage.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace server { namespace graphdb {

/**
 * An iterator used to iterate over Sqlite Prepared and Binded SELECT statements (i.e. query results)
 */

class SqliteResultsIterator : public boost::iterator_facade<SqliteResultsIterator, 
    int, 
    boost::forward_traversal_tag> {
public:
    SqliteResultsIterator(sqlite3_stmt* stmt) : pStmt(stmt) {
        increment();
    }

    SqliteResultsIterator() : pStmt(nullptr) {}

    //TODO: add access methods here.
    
private:
    friend class boost::iterator_core_access;

    void increment() {
        auto s = sqlite3_step(pStmt);
        if (s != SQLITE_ROW) {
            pStmt = nullptr;
        }
    }

    bool equal(SqliteResultsIterator const& other) const {
        return other.pStmt == pStmt;
    }

    int& dereference() {
        throw std::runtime_error("Don't dereference a Sqlite iterator.");
    }

    sqlite3_stmt *pStmt;
};

Id createRandomUUID() {
    static auto generator = boost::uuids::random_generator();
    return Id{generator()};
}

Id createFromString(const std::string& str) {
    static boost::uuids::string_generator gen;
    return gen(str);
}

void executeStatment(const std::string& stmt, sqlite3* pDB) {
    sqlite3_stmt *pStmt;
    if(SQLITE_OK != sqlite3_prepare_v2(pDB, stmt.c_str(), stmt.size(), &pStmt, NULL)) {
        throw std::runtime_error{"Failed to create stmt: " + stmt};
    }
    sqlite3_step(pStmt);
    sqlite3_finalize(pStmt);
}



/**
 * Returns NodeId from a nodes table
 **/
std::vector<Id> getNodesQuery(sqlite3_stmt* stmt) {
    std::vector<Id> retval;
    SqliteResultsIterator iterBegin(stmt);
    SqliteResultsIterator iterEnd;
    while(iterBegin != iterEnd) {
        auto id = createFromString(std::string{reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))});
        retval.push_back(id);
        iterBegin++;
    }
    return retval;
}

SqlStorage::SqlStorage(const char* path) {
    if (sqlite3_open(path, &pDB)!= SQLITE_OK) {
        throw std::runtime_error{std::string{"Could not open "} + path};
    }

    std::string createNodes{"CREATE TABLE IF NOT EXISTS nodes(id TEXT PRIMARY KEY, type INTEGER);"};

    executeStatment(createNodes, pDB);

    std::string createEdges{"CREATE TABLE IF NOT EXISTS edges(id TEXT PRIMARY KEY, type INTEGER, v1 TEXT, v2 TEXT);"};
    executeStatment(createEdges, pDB);

}



SqlStorage::~SqlStorage() {
    sqlite3_close(pDB);
}


Id SqlStorage::createNewNode(NodeType type) {
    auto newId = createRandomUUID();
    std::stringstream ss;
    ss << "INSERT INTO nodes VALUES ('"<< newId << "', " << static_cast<uint32_t>(type) << ");";
    executeStatment(ss.str(), pDB);
    return newId;
}




Id SqlStorage::createNewEdge(Id node1, Id node2, Edgetype eType) {
    auto newId = createRandomUUID();
    std::stringstream ss;
    ss << "INSERT INTO edges VALUES ('"<< newId << "', " << static_cast<uint32_t>(eType) << ", '" << node1 << "','" << node2 << "');";
    executeStatment(ss.str(), pDB);
    return newId;
}


bool SqlStorage::nodeExists(Id node1) const {
    std::string selectQuery = "SELECT * FROM nodes where id=?;";
    sqlite3_stmt *pStmt;
    if (SQLITE_OK != sqlite3_prepare_v2(pDB, selectQuery.c_str(), -1, &pStmt, nullptr)) {
        std::cout << "Failed to prepare query" << std::endl;
    }
    
    std::string id = to_string(node1);
    if(SQLITE_OK != sqlite3_bind_text(pStmt, 1, id.c_str(), id.size(), SQLITE_STATIC)) {
        std::cout << "Failed to bind" << std::endl;
    }
    auto ids = getNodesQuery(pStmt);
    sqlite3_finalize(pStmt);
    return ids.size() > 0;
}



}}
