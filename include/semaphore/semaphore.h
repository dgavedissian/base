/* C++ utils
 * Written by David Avedissian (c) 2019-2020 (git@dga.dev)  */
#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>

// Based on https://stackoverflow.com/a/27852868
class Semaphore {
public:
    using native_handle_type = typename std::condition_variable::native_handle_type;

    explicit Semaphore(std::size_t count = 0);
    Semaphore(const Semaphore&) = delete;
    Semaphore(Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;

    void notify();
    void wait();
    bool try_wait();
    template <class Rep, class Period> bool wait_for(const std::chrono::duration<Rep, Period>& d);
    template <class Clock, class Duration>
    bool wait_until(const std::chrono::time_point<Clock, Duration>& t);

    native_handle_type native_handle();

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::size_t count_;
};

inline Semaphore::Semaphore(std::size_t count) : count_(count) {
}

inline void Semaphore::notify() {
    std::lock_guard<std::mutex> lock{mutex_};
    ++count_;
    cv_.notify_one();
}

inline void Semaphore::wait() {
    std::unique_lock<std::mutex> lock{mutex_};
    cv_.wait(lock, [&] { return count_ > 0; });
    --count_;
}

inline bool Semaphore::try_wait() {
    std::lock_guard<std::mutex> lock{mutex_};
    if (count_ > 0) {
        --count_;
        return true;
    }
    return false;
}

template <class Rep, class Period>
bool Semaphore::wait_for(const std::chrono::duration<Rep, Period>& d) {
    std::unique_lock<std::mutex> lock{mutex_};
    auto finished = cv_.wait_for(lock, d, [&] { return count_ > 0; });
    if (finished) {
        --count_;
    }
    return finished;
}

template <class Clock, class Duration>
bool Semaphore::wait_until(const std::chrono::time_point<Clock, Duration>& t) {
    std::unique_lock<std::mutex> lock{mutex_};
    auto finished = cv_.wait_until(lock, t, [&] { return count_ > 0; });
    if (finished) {
        --count_;
    }
    return finished;
}

inline Semaphore::native_handle_type Semaphore::native_handle() {
    return cv_.native_handle();
}
