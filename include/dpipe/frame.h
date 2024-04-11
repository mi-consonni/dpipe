#ifndef DPIPE_FRAME_H_
#define DPIPE_FRAME_H_

#include <memory>

namespace dpipe {

/**
 * @brief A box that holds a reference-counted pointer to a data object
 *        and prevents write-access to it, except for MutFrame.
 *
 * @tparam T Data object type held by the Frame.
 */
template <typename T>
class Frame {
public:
    /// @brief Type of the data object held by the Frame.
    using Inner = T;

    /**
     * @brief An helper class that allows to restrict access to the
     *        mutable reference to the inner data object held by a Frame.
     */
    class AccessKey {
        AccessKey() = default;
        ~AccessKey() = default;

        AccessKey(const AccessKey&) = delete;
        AccessKey& operator=(const AccessKey&) = delete;

        AccessKey(AccessKey&&) = delete;
        AccessKey& operator=(AccessKey&&) = delete;

        template <typename U>
        friend class MutFrame;
    };

    /**
     * @brief Creates a new data object and wraps it into a Frame.
     */
    template <typename... Args>
    static Frame<T> make(Args&&... args) {
        return Frame<T>(std::make_shared<T>(std::forward<Args>(args)...));
    }

    /**
     * @brief Returns a const reference to the inner data object.
     */
    const T& operator*() const {
        return *ptr_;
    }

    /**
     * @brief Returns a const pointer to the inner data object.
     */
    const T* operator->() const {
        return ptr_.operator->();
    }

    /**
     * @brief Returns a mutable reference to the inner data object.
     *        The access is restricted to classes that can create an AccessKey,
     *        _i.e._, its friends.
     */
    T& inner(AccessKey) {
        return *ptr_;
    }

private:
    explicit Frame(std::shared_ptr<T>&& ptr)
            : ptr_{ptr} {}

    std::shared_ptr<T> ptr_;
};

} // namespace dpipe

#endif // DPIPE_FRAME_H_
