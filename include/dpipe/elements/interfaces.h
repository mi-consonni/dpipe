#ifndef DPIPE_ELEMENTS_INTERFACES_H_
#define DPIPE_ELEMENTS_INTERFACES_H_

#include <dpipe/frame.h>

namespace dpipe {

/**
 * @brief An interface for the first element in a pipeline (source).
 *        Client code does not need to use it directly.
 */
class Entry {
public:
    virtual ~Entry() = default;

    /**
     * @brief Makes the source produce a frame.
     */
    virtual void push() = 0;
};

/**
 * @brief An interface for middle and last elements in a pipeline (filters and sinks).
 *        Client code does not need to use it directly.
 */
template <typename Payload_>
class Next {
public:
    using Payload = Payload_;

    virtual ~Next() = default;

    /**
     * @brief Pushes a new frame into the element.
     */
    virtual void push(Frame<Payload>&& frame) = 0;
};

} // namespace dpipe

#endif // DPIPE_ELEMENTS_INTERFACES_H_
