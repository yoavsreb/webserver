#include "gtest/gtest.h"
#include <boost/filesystem.hpp>
#include "graphdb/SqlStorage.hpp"

TEST(SqlStorage, testOpen) {
    boost::filesystem::remove("/tmp/file_sql.db");
    server::graphdb::SqlStorage storage("/tmp/file_sql.db");
}

TEST(SqlStorage, testOpenCreate) {
    boost::filesystem::remove("/tmp/file_sql2.db");
    server::graphdb::SqlStorage storage("/tmp/file_sql2.db");
    auto id1 = storage.createNewNode(1);
    auto id2 = storage.createNewNode(2);
    auto eid = storage.createNewEdge(id1, id2, 2);

    EXPECT_TRUE(storage.nodeExists(id1));
    EXPECT_TRUE(storage.nodeExists(id2));
}
