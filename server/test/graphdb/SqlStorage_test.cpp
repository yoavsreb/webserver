#include "graphdb/SqlStorage.hpp"
#include "gtest/gtest.h"
#include <boost/filesystem.hpp>

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
  EXPECT_TRUE(storage.edgeExists(eid));

  auto node1Opt = storage.getNode(id1);
  EXPECT_TRUE(node1Opt);
  auto node1 = *node1Opt;
  EXPECT_EQ(node1.nodeId, id1);
  EXPECT_EQ(node1.nodeType, 1);

  auto edgeOpt = storage.getEdge(eid);
  EXPECT_TRUE(edgeOpt);
  auto edge = *edgeOpt;
  EXPECT_EQ(edge.edgeId, eid);
  EXPECT_EQ(edge.edgeType, 2);
  EXPECT_EQ(edge.v1, id1);
  EXPECT_EQ(edge.v2, id2);

  auto incomingEdges = storage.getIncomingEdges(id2);
  auto outgoingEdges = storage.getOutgoingEdges(id1);
  EXPECT_EQ(incomingEdges.size(), outgoingEdges.size());
  EXPECT_EQ(incomingEdges.front().edgeId, outgoingEdges.front().edgeId);

  {
    auto nodesByType = storage.getNodesByType(1);
    EXPECT_EQ(id1, nodesByType.front().nodeId);
  }
  {
    auto nodesByType = storage.getNodesByType(2);

    EXPECT_EQ(id2, nodesByType.front().nodeId);
  }
}
