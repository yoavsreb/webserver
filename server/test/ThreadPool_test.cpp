#include "gtest/gtest.h"
#include "ThreadPool.hpp"

struct CopyableFunctor {
    void run() {
        std::cout << "Hello World CopuableFunctor!" << std::endl;
    }

    CopyableFunctor() {}
    CopyableFunctor(const CopyableFunctor& other) = default;
    CopyableFunctor(CopyableFunctor&& other) = delete;
};

class MoveableFunctor {
public:
    MoveableFunctor(std::size_t count, const std::string& str) :
        msgCount(count), message(str) {}

    void run() {
        for (auto i = 0U; i < msgCount; i++) {
            std::cout << message << std::endl << std::flush;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    MoveableFunctor(const MoveableFunctor& other) = delete;
    MoveableFunctor(MoveableFunctor&& other) = default;

    
private:
    std::string message;
    std::size_t msgCount;
};

TEST(EmptyTest, test1) {
    ThreadPool threadPool{4};
    
    // Demonstrates how to create a moveable functor,
    MoveableFunctor functor{10, "Thread1"};
    threadPool.add(std::move(functor), 1U);
    
    {   //demonstrates that a copy of the functor is created.
        //other will go out of scope but the thread will not fail.
        const CopyableFunctor other;
        threadPool.add(other, 2U);
    }

    {
        MoveableFunctor other2{10, "Thread2"};
        threadPool.add(std::move(other2), 3U);
    }
    
    std::cout << "Is in use: " << threadPool.inUse() << std::endl;
    threadPool.join();
    EXPECT_EQ(threadPool.inUse(), false);
    threadPool.disable();
}
