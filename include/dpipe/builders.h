#ifndef DPIPE_BUILDERS_H_
#define DPIPE_BUILDERS_H_

#include <dpipe/elements.h>

namespace dpipe {

/**
 * @brief An helper object to be used in `make_arm` or in `make_pipe` builder
 *        functions to put a decoupler between user-defined elements.
 */
struct DecouplerPlaceholder {};

namespace impl {

template <typename T, typename NextType>
std::unique_ptr<dpipe::Next<T>> make_arm_inner(NextType&& next) {
    return std::move(next);
}

template <typename T, typename NextType, typename... Args>
std::unique_ptr<dpipe::Next<T>> make_arm_inner(NextType&& next, DecouplerPlaceholder&&,
                                               Args&&... args) {
    auto filter = std::make_unique<dpipe::Decoupler<T>>(std::move(next));
    return impl::make_arm_inner<T>(std::move(filter), std::forward<Args>(args)...);
}

template <typename T, typename NextType, typename FilterImpl, typename... Args>
std::unique_ptr<dpipe::Next<T>> make_arm_inner(NextType&& next, FilterImpl&& filterImpl,
                                               Args&&... args) {
    auto filter = std::make_unique<dpipe::Filter<FilterImpl>>(std::move(next),
                                                              std::move(filterImpl));
    return impl::make_arm_inner<T>(std::move(filter), std::forward<Args>(args)...);
}

template <typename NextType, typename SourceImpl>
dpipe::Pipeline make_pipe_inner(NextType&& next, SourceImpl&& sourceImpl) {
    auto source = std::make_unique<dpipe::Source<SourceImpl>>(std::move(next),
                                                              std::move(sourceImpl));
    return dpipe::Pipeline{std::move(source)};
}

template <typename NextType, typename... Args>
dpipe::Pipeline make_pipe_inner(NextType&& next, DecouplerPlaceholder&&, Args&&... args) {
    using T = typename NextType::element_type::Payload;
    auto filter = std::make_unique<dpipe::Decoupler<T>>(std::move(next));
    return impl::make_pipe_inner(std::move(filter), std::forward<Args>(args)...);
}

template <typename NextType, typename FilterImpl, typename... Args>
dpipe::Pipeline make_pipe_inner(NextType&& next, FilterImpl&& filterImpl, Args&&... args) {
    auto filter = std::make_unique<dpipe::Filter<FilterImpl>>(std::move(next),
                                                              std::move(filterImpl));
    return impl::make_pipe_inner(std::move(filter), std::forward<Args>(args)...);
}

} // namespace impl

/**
 * @brief Creates a pipeline arm, _i.e._, a straight segment ending with a sink and
 *        starting with a non-source element.
 *        The elements must be passed in reverse orders (the sink goes first).
 *
 * @tparam T        The input data type of the resulting arm.
 * @tparam SinkImpl User-defined sink implementation.
 * @tparam Args     Zero or more user-defined filter implementations or
 *                  `dpipe::DecouplerPlaceholder` objects.
 */
template <typename T, typename SinkImpl, typename... Args>
std::unique_ptr<dpipe::Next<T>> make_arm(SinkImpl&& sinkImpl, Args&&... args) {
    auto sink = std::make_unique<dpipe::Sink<SinkImpl>>(std::move(sinkImpl));
    return impl::make_arm_inner<T>(std::move(sink), std::forward<Args>(args)...);
}

/**
 * @brief Creates a splitter, _i.e._, a node that receives in input a frame and forwards it into
 *        multiple elements.
 *        By definition, a splitter is linked to two or more arms.
 *
 * @tparam T    The data type handled by the splitter.
 * @tparam Args Additional linked arms.
 */
template <typename T, typename... Args>
std::unique_ptr<dpipe::Splitter<T>> make_splitter(std::unique_ptr<dpipe::Next<T>>&& next1,
                                                  std::unique_ptr<dpipe::Next<T>>&& next2,
                                                  Args&&... args) {
    return std::make_unique<dpipe::Splitter<T>>(std::move(next1), std::move(next2),
                                                std::forward<Args>(args)...);
}

/**
 * @brief Creates a pipeline that ends with a splitter and starts with a source.
 *        The elements must be passed in reverse orders (the splitter goes first).
 *
 * @tparam T        The data type handled by the splitter.
 * @tparam Args     Zero or more user-defined element implementations or
 *                  `dpipe::DecouplerPlaceholder` objects.
 *                  The last element (and only it) must be a source.
 */
template <typename T, typename... Args>
dpipe::Pipeline make_pipe(std::unique_ptr<dpipe::Splitter<T>>&& splitter, Args&&... args) {
    return impl::make_pipe_inner(std::move(splitter), std::forward<Args>(args)...);
}

/**
 * @brief Creates a straight pipeline that ends with a sink and starts with a source.
 *        The elements must be passed in reverse orders (the sink goes first).
 *
 * @tparam SinkImpl User-defined sink implementation.
 * @tparam Args     Zero or more user-defined element implementations or
 *                  `dpipe::DecouplerPlaceholder` objects.
 *                  The last element (and only it) must be a source.
 */
template <typename SinkImpl, typename... Args>
dpipe::Pipeline make_pipe(SinkImpl&& sinkImpl, Args&&... args) {
    auto sink = std::make_unique<dpipe::Sink<SinkImpl>>(std::move(sinkImpl));
    return impl::make_pipe_inner(std::move(sink), std::forward<Args>(args)...);
}

} // namespace dpipe

#endif // DPIPE_BUILDERS_H_
