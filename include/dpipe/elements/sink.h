#ifndef DPIPE_ELEMENTS_SINK_H_
#define DPIPE_ELEMENTS_SINK_H_

#include <dpipe/elements/interfaces.h>
#include <dpipe/frame.h>

namespace dpipe {

/**
 * @brief An element that consumes frames.
 *
 * @tparam Impl_ User-defined sink implementation.
 *               It must define `InputPayload` type, as well as a `consume`
 *               function taking `Frame<InputPayload>` and returning `void`.
 */
template <typename Impl_>
class Sink : public Next<typename Impl_::InputPayload> {
public:
    using Impl = Impl_;
    using InputPayload = typename Impl::InputPayload;

    /**
     * @brief Constructor. Typically not used directly, but through `make_arm` or `make_pipe`
     *        builder functions.
     */
    template <typename... Args>
    explicit Sink(Args&&... args)
            : impl_{std::forward<Args>(args)...} {}

    ~Sink() = default;

    Sink(const Sink& other) = delete;
    Sink& operator=(const Sink& other) = delete;

    Sink(Sink&& other) = default;
    Sink& operator=(Sink&& other) = default;

    void push(Frame<InputPayload>&& input) override {
        impl_.consume(std::move(input));
    }

private:
    Impl impl_;
};

} // namespace dpipe

#endif // DPIPE_ELEMENTS_SINK_H_
