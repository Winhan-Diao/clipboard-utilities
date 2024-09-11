#pragma once
#include <thread>
#include <functional>
#include <mutex>
#include <vector>
#include <queue>
#include <atomic>
#include <iostream>
#include <condition_variable>
#include "usage.h"

class ThreadPool {
private:
    size_t threadCounts;
    std::vector<std::thread> threads{};
    std::queue<std::function<void()>> tasks{};

    std::mutex tasksMutex;
    std::condition_variable cv;
    std::atomic_bool stop{false};
public:
    ThreadPool(size_t threadCounts);

    template <class F, class... Args>
    void addTasks(F&& f, Args&&... args);

    inline size_t getThreadCounts() noexcept;
    inline size_t getWaitingTaskCounts() noexcept;

    ~ThreadPool() noexcept;
};

// ==============================
//         Definition
// ==============================

template <class F, class... Args>
void ThreadPool::addTasks(F&& f, Args&&... args) {
    {
        std::unique_lock<std::mutex> lock(tasksMutex);
        tasks.emplace(std::forward<F>(f), std::forward<Args>(args)...);
    }
    cv.notify_one();
}

inline size_t ThreadPool::getThreadCounts() noexcept {
    return threadCounts;
}

inline size_t ThreadPool::getWaitingTaskCounts() noexcept {
    return tasks.size();
}

namespace general_usage {
    extern ThreadPool threadPool;
}