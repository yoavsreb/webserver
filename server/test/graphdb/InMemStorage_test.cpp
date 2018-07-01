#include "gtest/gtest.h"
#include "graphdb/InMemStorage.hpp"

TEST(InMemStorage, test1) {
    server::graphdb::InMemStorage storage;
    
    auto id1 = storage.createNewNode(1);
    auto id2 = storage.createNewNode(2);

    EXPECT_TRUE(storage.nodeExists(id1));
    EXPECT_TRUE(storage.nodeExists(id2));

    auto node1Opt = storage.getNode(id1);
    EXPECT_TRUE(node1Opt);
    auto node1 = *node1Opt;
    EXPECT_EQ(node1.nodeId, id1);
    EXPECT_EQ(node1.nodeType, 1);
    
    {
        auto nodesByType = storage.getNodesByType(1);
        EXPECT_EQ(id1, nodesByType.front().nodeId); 
    }
    {
        auto nodesByType = storage.getNodesByType(2);

        EXPECT_EQ(id2, nodesByType.front().nodeId); 

    }
}
