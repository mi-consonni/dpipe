#ifndef DPIPE_ELEMENTS_FILTER_H_
#define DPIPE_ELEMENTS_FILTER_H_

#include <cassert>
#include <memory>

#include <dpipe/elements/interfaces.h>
#include <dpipe/frame.h>

namespace dpipe {

/**
 * @brief An element that filters, manipulates, or transforms frames.
 *
 * @tparam Impl_ User-defined filter implementation.
 *               It must define `InputPayload` and `OutPayload` types, as well as a `process`
 *               function taking `Frame<InputPayload>` and returning
 *               `std::optional<Frame<OutputPayload>>`.
 */
template <typename Impl_>
class Filter : public Next<typename Impl_::InputPayload> {
public:
    using Impl = Impl_;
    using InputPayload = typename Impl::InputPayload;
    using OutputPayload = typename Impl::OutputPayload;

    /**
     * @brief Constructor. Typically not used directly, but through `make_arm` or `make_pipe`
     *        builder functions.
     */
    template <typename... Args>
    explicit Filter(std::unique_ptr<Next<OutputPayload>>&& next, Args&&... args)
            : next_{std::move(next)}
            , impl_{std::forward<Args>(args)...} {
        assert(next_);
    }

    ~Filter() = default;

    Filter(const Filter& other) = delete;
    Filter& operator=(const Filter& other) = delete;

    Filter(Filter&& other) = default;
    Filter& operator=(Filter&& other) = default;

    void push(Frame<InputPayload>&& input) override {
        auto output = impl_.process(std::move(input));
        if (output.has_value()) {
            assert(next_);
            next_->push(std::move(*output));
        }
    }

private:
    std::unique_ptr<Next<OutputPayload>> next_;
    Impl impl_;
};

} // namespace dpipe

#endif // DPIPE_ELEMENTS_FILTER_H_
