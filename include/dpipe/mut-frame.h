#ifndef DPIPE_MUT_FRAME_H_
#define DPIPE_MUT_FRAME_H_

#include <dpipe/frame.h>

namespace dpipe {

/**
 * @brief Identical to a Frame but providing write-access to the inner data object.
 *
 * @tparam T Data object type held by the MutFrame.
 */
template <typename T>
class MutFrame {
public:
    /// @brief Type of the data object held by the MutFrame.
    using Inner = T;

    /**
     * @brief Creates a new data object and wraps it into a MutFrame.
     */
    template <typename... Args>
    static MutFrame<T> make(Args&&... args) {
        return MutFrame<T>{Frame<T>::make(std::forward<Args>(args)...)};
    }

    ~MutFrame() = default;

    // We do not want multiple mutable references to the same frame.
    MutFrame(const MutFrame<T>& other) = delete;
    MutFrame& operator=(const MutFrame<T>& other) = delete;

    MutFrame(MutFrame<T>&& other) noexcept = default;
    MutFrame& operator=(MutFrame<T>&& other) noexcept = default;

    /**
     * @brief Returns a const reference to the inner data object.
     */
    const T& operator*() const {
        return *frame_;
    }

    /**
     * @brief Returns a mutable reference to the inner data object.
     */
    T& operator*() {
        return frame_.inner(FrameAccessKey{});
    }

    /**
     * @brief Returns a const pointer to the inner data object.
     */
    const T* operator->() const {
        return frame_.operator->();
    }

    /**
     * @brief Returns a mutable pointer to the inner data object.
     */
    T* operator->() {
        return &frame_.inner(FrameAccessKey{});
    }

    /**
     * @brief Consumes the MutFrame creating a Frame, making the
     *        inner data object immutable.
     */
    Frame<T> into_immutable() {
        // Move this, so that an immutable frame never has any
        // mutable references around.
        return std::move(*this).frame_;
    }

    /**
     * @brief Implicit conversion operator to immutable frame.
     *        Consumes the current MutFrame.
     */
    operator Frame<T>() {
        return into_immutable();
    }

private:
    using FrameAccessKey = typename Frame<T>::AccessKey;

    explicit MutFrame(Frame<T>&& frame)
            : frame_{std::move(frame)} {}

    Frame<T> frame_;
};

} // namespace dpipe

#endif // DPIPE_MUT_FRAME_H_
