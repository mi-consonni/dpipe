#ifndef DPIPE_ELEMENTS_SOURCE_H_
#define DPIPE_ELEMENTS_SOURCE_H_

#include <cassert>
#include <memory>

#include <dpipe/elements/interfaces.h>

namespace dpipe {

/**
 * @brief An element that produces frames.
 *
 * @tparam Impl_ User-defined source implementation.
 *               It must define `OutputPayload` type, as well as a `produce`
 *               function taking no input and returning `std::optional<Frame<OutputPayload>>`.
 */
template <typename Impl_>
class Source : public Entry {
public:
    using Impl = Impl_;
    using OutputPayload = typename Impl::OutputPayload;

    /**
     * @brief Constructor. Typically not used directly, but through `make_arm` or `make_pipe`
     *        builder functions.
     */
    template <typename... Args>
    explicit Source(std::unique_ptr<Next<OutputPayload>>&& next, Args&&... args)
            : next_{std::move(next)}
            , impl_{std::forward<Args>(args)...} {
        assert(next_);
    }

    ~Source() = default;

    Source(const Source& other) = delete;
    Source& operator=(const Source& other) = delete;

    Source(Source&& other) = default;
    Source& operator=(Source&& other) = default;

    void push() override {
        auto output = impl_.produce();
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

#endif // DPIPE_ELEMENTS_SOURCE_H_
