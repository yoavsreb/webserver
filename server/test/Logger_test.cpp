#include "gtest/gtest.h"
#include "Logger.hpp"
#include "ThreadPool.hpp"

#include <boost/filesystem.hpp>

struct CopyableFunctor {
    void run() {
        for (auto i = 0; i < 1000; i++) {
            info(logger, msg); 
        }
    }

    CopyableFunctor(Logger& l, const std::string& m) : logger(l), msg(m) {}
    CopyableFunctor(const CopyableFunctor& other) = default;
    CopyableFunctor(CopyableFunctor&& other) = delete;

    Logger& logger;
    std::string msg;

};

TEST(LoggerTest, test1) {
    boost::filesystem::remove("/tmp/file1.txt");
    Logger logger("/tmp/file1.txt");
    EXPECT_EQ(logger.level(), Level::INFO);
    trace(logger, "What's up1"); 
    debug(logger, "What's up2"); 
    info(logger, "What's up3"); 
    warn(logger, "What's up4"); 
    error(logger, "What's up5"); 
}

TEST(LoggerTest, test2_multithreaded) {
    boost::filesystem::remove("/tmp/file2.txt");
    Logger logger("/tmp/file2.txt");
    const CopyableFunctor func1(logger, "Hello A");
    const CopyableFunctor func2(logger, "Hello B");

    ThreadPool threadPool{4};
    threadPool.add(func1, 1U);
    threadPool.add(func2, 2U);
    threadPool.join();
    threadPool.disable();
}


