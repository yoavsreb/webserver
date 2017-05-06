// Author: yoavsreb

#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <atomic>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <map>
/**
 * Usage:
 * struct Worker {
 *     void run() {
 *        ...
 *     }
 * };
 * ThreadPool t;
 * Worker worker1{};
 * // create a copy
 * if (t.add(foo, 1U)) {
 *      t.join();
 *   std::cout << "In use: " << t.inUse() << std::endl;
 * } else {
 *      std::cout << "No available threads" << std::endl;
 * }
 * 
 * // demonstrates how to move the functor.
 * Worker worker2{};
 * t.add(std::move(worker2), 2U);
 *
 * Synchronization mechanism:
 *  1. Using atomic variables to prevent redundant locks
 *  2. A map of taskId -> std::thread is synchronized using a mutex
 *  3. An internal thread does the vaccuming after threads that are done.
 *
 */

namespace _{ 
/** Namespace for internal implementation constructs.
 * ITask and Task are used for type-erasure implementation.
 */
class ITask {
    public:
        virtual void run() = 0;
        
        virtual ~ITask() {}
        ITask() {}
};

template<typename T> 
class Task : public ITask {
public:
    Task(const T& t) : pTask(new T(t)) {}
    Task(T&& t) : pTask(new T(std::move(t))) {}
    void run() { 
        pTask->run();
    }

private:
    std::unique_ptr<T> pTask;
};
} //of namespace _


/**
 * A simple thread pool.
 * Default max threads is 5;
 * In case all threads are currently in use, pool will not accept more
 * work and the method <b>add</b> will return false.
 *
 * Calling disable will not allow any more work to be allocated. It will 
 * also disable the vaccum thread. That means that if a work-thread finish
 * after disable was called, the memory associated with it (i.e. thread and
 * Task object) will be freed only on destruction of the ThreadPool object.
 * Because of that, the suggested usage is:
 *  - Assign workers (i.e. call ThreadPool::add)
 *  - Wait for workers to be done (i.e. call ThreadPool::join)
 *  - Disable ThreadPool (call ThreadPool::disable)
 *  - ThreadPool destructor can be safetly called.
 */
class ThreadPool {
    public:
        enum {
            MAX_THREADS = 5u
        };

        ThreadPool() : ThreadPool(MAX_THREADS) {}
        ThreadPool(uint16_t max) : maxThreads(max), enabledFlag(true), runningThreadsCount(0U),
            vaccumThread([this] () -> void { 
                    while(this->enabledFlag.load()) { 
                        this->vaccum(); 
                        std::this_thread::yield(); 
                    }
                }) {}
        ThreadPool(const ThreadPool& other) = delete;
        ThreadPool operator=(const ThreadPool& other) = delete;
        
        /**
         * Adds a worker thread.
         * Creates a copy of Functor, therefore it's safe for it to get
         * out of scope. Assumes Functor has a copy-constructor.
         * In case the thread-pool resources are exhausted, the work will
         * be rejected and the function will return false.
         */
        template<typename Functor>
        bool add(const Functor& functor, uint16_t taskId) {
            
            while (1) {
                if (!enabledFlag.load()) {
                    return false;
                }
                auto count = runningThreadsCount.load();
                if (count >= maxThreads) {
                    std::cerr << "count is too big: " << count << std::endl;
                    return false;
                } else {
                    if (runningThreadsCount.compare_exchange_strong(count, count+1)) {
                        std::lock_guard<std::mutex> lock_guard(mutex);
                        std::unique_ptr<::_::ITask> pTask{new ::_::Task<Functor>(functor)};
                        auto pRawTask = pTask.get();
                        std::shared_ptr<std::thread> pThr(
                                new std::thread([this, taskId, pRawTask] () -> void { 
                                try {
                                    pRawTask->run();
                                } catch (const std::exception& ex) {
                                    std::cerr << ex.what() << std::endl;
                                }
                                this->deregister(taskId); 
                            }));
                        threadMap.emplace(taskId, pThr);
                        taskMap.emplace(taskId, std::move(pTask));
                        break;
                    }
                }          
            }  
            return true;
        }
        
        /**
         * Adds a worker thread.
         * Moves the Functor. Source functor may be unusable and should
         * not be used.
         * In case the thread-pool resources are exhausted, the work will
         * be rejected and the function will return false.
         */
        template<typename Functor>
        bool add(Functor&& functor, uint16_t taskId) {
            while (1) {
                if (!enabledFlag.load()) {
                    return false;
                }
                auto count = runningThreadsCount.load();
                if (count >= maxThreads) {
                    std::cerr << "count is too big: " << count << std::endl;
                    return false;
                } else {
                    if (runningThreadsCount.compare_exchange_strong(count, count+1)) {
                        std::lock_guard<std::mutex> lock_guard(mutex);
                        std::unique_ptr<::_::ITask> pTask{new ::_::Task<Functor>(std::move(functor))};
                        auto pRawTask = pTask.get();
                        std::shared_ptr<std::thread> pThr(
                                new std::thread([this, taskId, pRawTask] () -> void { 
                                try {
                                    pRawTask->run();
                                } catch (const std::exception& ex) {
                                    std::cerr << ex.what() << std::endl;
                                }
                                this->deregister(taskId); 
                            }));
                        threadMap.emplace(taskId, pThr);
                        taskMap.emplace(taskId, std::move(pTask));
                        break;
                    }
                }          
            }  
            return true;
        }
        /**
         * Disable the thread pool.
         * Memory of un-finished threads will not be reclaimed by the
         * ThreadPool's vaccum-thread.
         */
        void disable() {
            enabledFlag.store(false);
            vaccumThread.join();
        }

        /**
         * Returns whether a worker thread is still executing
         */
        bool inUse() const {
            return (runningThreadsCount.load() > 0U);
        }

        /**
         * Returns whether the Thread-Pool is accepting more work.
         */
        bool enabled() const {
            return enabledFlag.load();
        }

        /**
         * Wait for all worker threads to finish
         */
        void join() {
            while(inUse()) {
                std::this_thread::yield();
            }
        }


    private:
        /**
         * This method, called from a worker thread, marks the
         * thread related data for deletion
         */
        void deregister(uint16_t taskId) {
            std::lock_guard<std::mutex> guard(mutex);
            tasksToCleanUp.push_back(taskId);
        }

        /**
         * This method, called by the vaccum-thread, cleans
         * the threads data of threads that have finished execution
         */
        void vaccum() {
            std::lock_guard<std::mutex> guard(mutex);
            for(const auto taskId : tasksToCleanUp) {
                threadMap[taskId]->join();
                threadMap.erase(taskId);
                taskMap.erase(taskId);
                runningThreadsCount--;
            }
            tasksToCleanUp.clear();
        }
       
        const uint16_t maxThreads; 
        std::map<uint16_t, std::shared_ptr<std::thread>> threadMap;
        std::map<uint16_t, std::unique_ptr<::_::ITask>> taskMap;
        std::vector<uint16_t> tasksToCleanUp;
        std::atomic_uint runningThreadsCount;
        std::atomic_bool enabledFlag;
        std::mutex mutex;
        std::thread vaccumThread;
};

#endif
