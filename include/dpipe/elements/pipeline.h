#ifndef DPIPE_ELEMENTS_PIPELINE_H_
#define DPIPE_ELEMENTS_PIPELINE_H_

#include <cassert>
#include <memory>
#include <thread>

#include <dpipe/elements/interfaces.h>

namespace dpipe {

/**
 * @brief The pipeline, _i.e._, the object that makes the data flow happen.
 */
class Pipeline {
public:
    /**
     * @brief Constructor. Typically not used directly, but through `make_arm` or `make_pipe`
     *        builder functions.
     */
    explicit Pipeline(std::unique_ptr<Entry>&& entry)
            : entry_{std::move(entry)} {
        assert(entry_);
    }

    ~Pipeline() = default;

    Pipeline(const Pipeline& other) = delete;
    Pipeline& operator=(const Pipeline& other) = delete;

    Pipeline(Pipeline&& other) = default;
    Pipeline& operator=(Pipeline&& other) = default;

    /**
     * @brief Starts the data flow.
     *        It is safe to start multiple times, though data processing is stopped and restarted at
     *        every call.
     */
    void start() {
        assert(entry_);
        thread_ = std::jthread{[entry = entry_](std::stop_token token) {
            while (!token.stop_requested()) {
                entry->push();
            }
        }};
    }

    /**
     * @brief Stops the data flow. The call blocks.
     */
    void stop() {
        thread_ = {};
    }

private:
    // The reference to `Entry` is stored as a shared pointer to allow its use in
    // the internally-spawned thread. Though, it is not meant to be shared outside of this class.
    std::shared_ptr<Entry> entry_;
    std::jthread thread_;
};

} // namespace dpipe

#endif // DPIPE_ELEMENTS_PIPELINE_H_
