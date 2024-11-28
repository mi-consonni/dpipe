#ifndef DPIPE_DPIPE_H_
#define DPIPE_DPIPE_H_

#include <dpipe/builders.h>
#include <dpipe/elements.h>
#include <dpipe/frame.h>
#include <dpipe/mut-frame.h>

namespace dpipe {

/**
 * @brief Returns a string containing the library version,
 *        following semantic versioning conventions.
 */
inline constexpr const char* version() {
    return "0.1.0";
}

} // namespace dpipe

#endif // DPIPE_DPIPE_H_
