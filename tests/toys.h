#ifndef DPIPE_TESTS_TOYS_H_
#define DPIPE_TESTS_TOYS_H_

#include <cstdint>
#include <functional>
#include <limits>
#include <optional>

#include <dpipe/frame.h>
#include <dpipe/mut-frame.h>

struct RawPayload {
    static constexpr uint8_t MAX_LEVEL = std::numeric_limits<uint8_t>::max();
    uint8_t level{};
};

struct CalibratedPayload {
    float percentage{};
};

class RampUpSource {
public:
    using OutputPayload = RawPayload;
    using OutputFrame = dpipe::Frame<OutputPayload>;

    explicit RampUpSource(uint8_t target_level)
            : target_level_{target_level} {}

    std::optional<OutputFrame> produce() {
        if (counter_ >= target_level_) {
            return {};
        }

        // Create a new immutable frame from scratch.
        auto frame = dpipe::Frame<RawPayload>::make(counter_);
        counter_ += 1;
        return frame;
    }

private:
    uint8_t target_level_{};
    uint8_t counter_{};
};

class ShiftUpFilter {
public:
    using InputPayload = RawPayload;
    using OutputPayload = RawPayload;
    using InputFrame = dpipe::Frame<InputPayload>;
    using OutputFrame = dpipe::Frame<OutputPayload>;

    explicit ShiftUpFilter(uint8_t amount)
            : amount_{amount} {}

    std::optional<OutputFrame> process(InputFrame&& frame) {
        // Clone a frame into a new mutable one.
        auto shifted_frame = dpipe::MutFrame<RawPayload>::make(*frame);
        if (RawPayload::MAX_LEVEL - shifted_frame->level >= amount_) {
            shifted_frame->level += amount_;
        } else {
            shifted_frame->level = RawPayload::MAX_LEVEL;
        }
        return shifted_frame;
    }

private:
    uint8_t amount_{};
};

class ThresholdFilter {
public:
    using InputPayload = RawPayload;
    using OutputPayload = RawPayload;
    using InputFrame = dpipe::Frame<InputPayload>;
    using OutputFrame = dpipe::Frame<OutputPayload>;

    explicit ThresholdFilter(uint8_t threshold)
            : threshold_{threshold} {}

    std::optional<OutputFrame> process(InputFrame&& frame) {
        // Simply discard some frames.
        if (frame->level < threshold_) {
            return {};
        }
        return frame;
    }

private:
    uint8_t threshold_{};
};

class CalibrationFilter {
public:
    using InputPayload = RawPayload;
    using OutputPayload = CalibratedPayload;
    using InputFrame = dpipe::Frame<InputPayload>;
    using OutputFrame = dpipe::Frame<OutputPayload>;

    std::optional<OutputFrame> process(InputFrame&& frame) {
        // Create a new mutable frame from scratch.
        auto calibrated_frame = dpipe::MutFrame<CalibratedPayload>::make();
        calibrated_frame->percentage = static_cast<float>(frame->level) * 100.0;
        calibrated_frame->percentage /= RawPayload::MAX_LEVEL;
        return calibrated_frame;
    }
};

template <typename Payload>
class CounterSink {
public:
    using InputPayload = Payload;
    using InputFrame = dpipe::Frame<InputPayload>;

    explicit CounterSink(uint64_t& counter)
            : counter_{counter} {}

    void consume(InputFrame&& /* frame */) {
        // Count incoming frames.
        counter_.get() += 1;
    }

private:
    std::reference_wrapper<uint64_t> counter_;
};

#endif // DPIPE_TESTS_TOYS_H_
