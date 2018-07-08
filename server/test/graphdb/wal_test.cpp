#include "graphdb/wal.hpp"
#include "gtest/gtest.h"
#include <boost/filesystem.hpp>

TEST(WALTest, test1) {
  boost::filesystem::remove("/tmp/file_wal.txt");
  server::WAL wal("/tmp/file_wal.txt");
  EXPECT_EQ(0, wal.init().size());
  EXPECT_TRUE(boost::filesystem::exists("/tmp/file_wal.txt"));
}

TEST(WALTest, test2_existing_lines) {
  boost::filesystem::remove("/tmp/file_wal.txt");
  {
    server::WAL wal("/tmp/file_wal.txt");
    EXPECT_EQ(0, wal.init().size());
    server::WAL_Message message =
        server::buildMessage('1', '1', '1', "Hello World!");
    wal.write(&message);
  }

  {
    server::WAL wal("/tmp/file_wal.txt");
    auto nonCommitted = wal.init();
    EXPECT_EQ(1, nonCommitted.size());
    EXPECT_EQ('1', nonCommitted[0].type);
  }
}

TEST(WALTest, test3_existing_lines) {
  boost::filesystem::remove("/tmp/file_wal.txt");
  {
    server::WAL wal("/tmp/file_wal.txt");
    EXPECT_EQ(0, wal.init().size());
    wal.writeSnapshotMarker();
  }

  {
    server::WAL wal("/tmp/file_wal.txt");
    auto nonCommitted = wal.init();
    EXPECT_EQ(0, nonCommitted.size());
  }
}
