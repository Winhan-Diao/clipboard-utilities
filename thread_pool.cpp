#include "thread_pool.h"

ThreadPool::ThreadPool(size_t threadCounts): threadCounts{threadCounts} {
    for (int i = 0; i < threadCounts; ++i) {
        threads.emplace_back([this](){
            std::function<void()> task;
            for (;;) {
                {
                    std::unique_lock<std::mutex> lock(this->tasksMutex);
                    this->cv.wait(lock, [this](){
                        return this->stop || !this->tasks.empty();
                    });
                    if (this->stop && this->tasks.empty()) return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                try {
                    task();
                } catch (const std::exception& e) {
                    std::cerr << "[threadpool] exception caught; what(): " << e.what();     //error
                    wxLogError(wxString{} << "[threadpool] exception caught; what(): " << e.what());        //error
                } catch (...) {
                    std::cerr << "[threadpool] an unknown exception caught";
                    wxLogError("[threadpool] an unknown exception caught");
                }
            }
        });
    }
}


ThreadPool::~ThreadPool() noexcept {
    stop = true;
    cv.notify_all();
    for (auto& _thread: threads)
        _thread.join();
}

namespace general_usage {
    ThreadPool threadPool{std::thread::hardware_concurrency()};
}