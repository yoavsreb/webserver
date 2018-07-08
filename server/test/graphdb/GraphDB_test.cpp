#include "Logger.hpp"
#include "graphdb/SqlStorage.hpp"
#include "graphdb/graphdb.hpp"
#include "gtest/gtest.h"

#include <boost/filesystem.hpp>

namespace sg = server::graphdb;
namespace bf = boost::filesystem;

TEST(GraphDBTest, test1) {

  bf::remove("/tmp/graphdb.log");
  Logger logger("/tmp/graphdb.log");

  bf::remove("/tmp/file_sql.db");
  sg::SqlStorage storage("/tmp/file_sql.db");
  sg::GraphDB<sg::SqlStorage> graphdb(storage, logger);
  auto newNode1 = graphdb.createNode(1);
  auto newNode2 = graphdb.createNode(2);
  auto edge1 = graphdb.createEdge(newNode1->nodeId, newNode2->nodeId, 3);
  // Edge already exists
  EXPECT_FALSE(graphdb.createEdge(newNode1->nodeId, newNode2->nodeId, 3));

  // 2nd node doesn't exist
  EXPECT_FALSE(graphdb.createEdge(newNode1->nodeId, sg::createRandomUUID(), 3));
}
