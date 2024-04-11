#ifndef DPIPE_ELEMENTS_SPLITTER_H_
#define DPIPE_ELEMENTS_SPLITTER_H_

#include <cassert>
#include <memory>
#include <vector>

#include <dpipe/elements/interfaces.h>
#include <dpipe/frame.h>

namespace dpipe {

/**
 * @brief A special kind of filter that distributes frames to more than one destination elements.
 *
 * @tparam InputPayload_ The input data type.
 */
template <typename InputPayload_>
class Splitter : public Next<InputPayload_> {
public:
    using InputPayload = InputPayload_;
    using OutputPayload = InputPayload;

    /**
     * @brief Constructor. Typically not used directly, but through `make_splitter`
     *        builder function.
     */
    template <typename Arg1, typename Arg2, typename... Args>
    explicit Splitter(Arg1&& arg1, Arg2&& arg2, Args&&... args) {
        nexts_.push_back(std::move(arg1));
        nexts_.push_back(std::move(arg2));
        (nexts_.push_back(std::forward<Args>(args)), ...);
        for (const auto& next : nexts_) {
            assert(next);
            (void)next; // Suppress unused variable warning in release build.
        }
    }

    ~Splitter() = default;

    Splitter(const Splitter& other) = delete;
    Splitter& operator=(const Splitter& other) = delete;

    Splitter(Splitter&& other) = default;
    Splitter& operator=(Splitter&& other) = default;

    void push(Frame<InputPayload>&& input) override {
        for (const auto& next : nexts_) {
            assert(next);
            next->push(Frame<InputPayload>{input});
        }
    }

private:
    std::vector<std::unique_ptr<Next<InputPayload>>> nexts_;
};

} // namespace dpipe

#endif // DPIPE_ELEMENTS_SPLITTER_H_
