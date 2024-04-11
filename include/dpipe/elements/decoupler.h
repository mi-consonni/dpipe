#ifndef DPIPE_ELEMENTS_DECOUPLER_H_
#define DPIPE_ELEMENTS_DECOUPLER_H_

#include <cassert>
#include <chrono>
#include <memory>
#include <thread>

#include <dpipe/elements/interfaces.h>
#include <dpipe/frame.h>
#include <dpipe/utils/async-queue.h>

namespace dpipe {

/**
 * @brief A special kind of filter that decouples contiguous elements, so that the producer can push
 *        multiple frames without waiting for the consumer to finish its processing.
 *
 * @tparam InputPayload_ The input data type.
 */
template <typename InputPayload_>
class Decoupler : public Next<InputPayload_> {
public:
    using InputPayload = InputPayload_;
    using OutputPayload = InputPayload;

    /**
     * @brief Constructor. Typically not used directly, but through `make_arm` or `make_pipe`
     *        builder functions.
     */
    explicit Decoupler(std::unique_ptr<Next<OutputPayload>>&& next)
            : next_{std::move(next)}
            , queue_{std::make_shared<AsyncQueue<Frame<OutputPayload>>>()} {
        assert(next_);
        start();
    }

    ~Decoupler() = default;

    Decoupler(const Decoupler& other) = delete;
    Decoupler& operator=(const Decoupler& other) = delete;

    Decoupler(Decoupler&& other) = default;
    Decoupler& operator=(Decoupler&& other) = default;

    void push(Frame<InputPayload>&& input) override {
        assert(queue_);
        queue_->push_back(std::move(input));
    }

private:
    void start() {
        thread_ = std::jthread{[next = next_, queue = queue_](std::stop_token token) {
            while (!token.stop_requested()) {
                static constexpr auto DURATION = std::chrono::milliseconds(1);
                auto output = queue->try_pop_front_for(DURATION);
                if (output.has_value()) {
                    next->push(std::move(*output));
                }
            }
        }};
    }

    // The reference to `Next` is stored as a shared pointer to allow its use in
    // the internally-spawned thread. Though, it is not meant to be shared outside of this class.
    std::shared_ptr<Next<OutputPayload>> next_;
    std::shared_ptr<AsyncQueue<Frame<OutputPayload>>> queue_;
    std::jthread thread_;
};

} // namespace dpipe

#endif // DPIPE_ELEMENTS_DECOUPLER_H_
