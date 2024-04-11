#ifndef DPIPE_UTILS_ASYNC_QUEUE_H_
#define DPIPE_UTILS_ASYNC_QUEUE_H_

#include <deque>
#include <mutex>
#include <optional>
#include <thread>

namespace dpipe {

/**
 * @brief A simple thread-safe queue.
 *
 * @tparam T The element type contained by the queue.
 */
template <typename T>
class AsyncQueue {
public:
    using ElementType = T;

    std::optional<T> try_pop_front() {
        std::lock_guard<std::mutex> lock{mutex_};
        if (queue_.empty()) {
            return {};
        }
        T element = queue_.front();
        queue_.pop_front();
        return element;
    }

    template <typename Duration>
    std::optional<T> try_pop_front_for(Duration duration) {
        auto option = try_pop_front();
        if (!option.has_value()) {
            // We are obviously cheating: this is only a quick emulation
            // of what should be done with semaphores.
            std::this_thread::sleep_for(duration);
        }
        return option;
    }

    void push_back(T&& t) {
        std::lock_guard<std::mutex> lock{mutex_};
        queue_.push_back(std::move(t));
    }

private:
    std::deque<T> queue_;
    std::mutex mutex_;
};

} // namespace dpipe

#endif // DPIPE_UTILS_ASYNC_QUEUE_H_
