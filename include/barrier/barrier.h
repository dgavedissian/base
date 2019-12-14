/* C++ utils
 * Written by David Avedissian (c) 2019-2020 (git@dga.dev)  */
#pragma once

#include <mutex>
#include <condition_variable>

// Based on: https://stackoverflow.com/a/27118537
class Barrier {
public:
    explicit Barrier(std::size_t count) : threshold_(count), count_(count), generation_(0) {
    }

    void wait() {
        std::unique_lock<std::mutex> lock{mutex_};
        auto current_generation = generation_;
        if (--count_ == 0) {
            generation_++;
            count_ = threshold_;
            cv_.notify_all();
        } else {
            cv_.wait(lock,
                     [this, current_generation] { return current_generation != generation_; });
        }
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::size_t threshold_;
    std::size_t count_;
    std::size_t generation_;
};