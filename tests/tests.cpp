#include <chrono>
#include <cstdint>
#include <thread>

#include <dpipe/dpipe.h>
#include <gtest/gtest.h>

#include "toys.h"

using dpipe::make_arm;
using dpipe::make_pipe;
using dpipe::make_splitter;

static constexpr uint8_t TOTAL_FRAMES = 10;

static void run_pipeline(dpipe::Pipeline& pipeline, uint8_t milliseconds) {
    pipeline.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    pipeline.stop();
}

TEST(DPipe, SimpleSourceToSink) {
    uint64_t counter = 0;
    auto pipeline = make_pipe(CounterSink<RawPayload>{counter}, RampUpSource{TOTAL_FRAMES});
    run_pipeline(pipeline, TOTAL_FRAMES);
    EXPECT_EQ(counter, TOTAL_FRAMES);
}

TEST(DPipe, SimpleSourceToSinkWithDecoupler) {
    uint64_t counter = 0;
    auto pipeline = make_pipe(CounterSink<RawPayload>{counter}, dpipe::DecouplerPlaceholder{},
                              RampUpSource{TOTAL_FRAMES});
    run_pipeline(pipeline, TOTAL_FRAMES);
    EXPECT_EQ(counter, TOTAL_FRAMES);
}

TEST(DPipe, StraightPipeline) {
    uint64_t counter = 0;
    uint8_t threshold = 2;
    auto pipeline = make_pipe(CounterSink<RawPayload>{counter}, ThresholdFilter{threshold},
                              RampUpSource{TOTAL_FRAMES});
    run_pipeline(pipeline, TOTAL_FRAMES);
    EXPECT_EQ(counter, TOTAL_FRAMES - threshold);
}

TEST(DPipe, StraightPipelineWithTypeChange) {
    uint64_t counter = 0;
    uint8_t threshold = 2;
    auto pipeline = make_pipe(CounterSink<CalibratedPayload>{counter}, CalibrationFilter{},
                              ThresholdFilter{threshold}, RampUpSource{TOTAL_FRAMES});
    run_pipeline(pipeline, TOTAL_FRAMES);
    EXPECT_EQ(counter, TOTAL_FRAMES - threshold);
}

TEST(DPipe, StraightPipelineWithDecoupler) {
    uint64_t counter = 0;
    uint8_t threshold = 2;
    auto pipeline = make_pipe(CounterSink<CalibratedPayload>{counter}, CalibrationFilter{},
                              ThresholdFilter{threshold}, dpipe::DecouplerPlaceholder{},
                              RampUpSource{TOTAL_FRAMES});
    run_pipeline(pipeline, TOTAL_FRAMES);
    EXPECT_EQ(counter, TOTAL_FRAMES - threshold);
}

TEST(DPipe, PipelineSplitIntoTwoArms) {
    uint64_t counter1 = 0;
    uint64_t counter2 = 0;
    uint8_t threshold = 2;
    auto arm1 = make_arm<RawPayload>(CounterSink<RawPayload>{counter1});
    auto arm2 = make_arm<RawPayload>(CounterSink<RawPayload>{counter2}, ThresholdFilter{threshold});
    auto splitter = make_splitter(std::move(arm1), std::move(arm2));
    auto pipeline = make_pipe(std::move(splitter), RampUpSource{TOTAL_FRAMES});
    run_pipeline(pipeline, TOTAL_FRAMES);
    EXPECT_EQ(counter1, TOTAL_FRAMES);
    EXPECT_EQ(counter2, TOTAL_FRAMES - threshold);
}

TEST(DPipe, PipelineSplitIntoFourArms) {
    uint64_t counter1 = 0;
    uint64_t counter2 = 0;
    uint64_t counter3 = 0;
    uint64_t counter4 = 0;
    uint64_t counter5 = 0;
    uint8_t shift = 1;
    uint8_t threshold = 2;
    auto arm1 = make_arm<RawPayload>(CounterSink<RawPayload>{counter1});
    auto arm2 = make_arm<RawPayload>(CounterSink<RawPayload>{counter2}, ThresholdFilter{threshold});
    auto arm3 = make_arm<RawPayload>(CounterSink<CalibratedPayload>{counter3}, CalibrationFilter{});
    auto arm4 = make_arm<RawPayload>(CounterSink<CalibratedPayload>{counter4}, CalibrationFilter{},
                                     ThresholdFilter{threshold});
    auto arm5 = make_arm<RawPayload>(CounterSink<CalibratedPayload>{counter5}, CalibrationFilter{},
                                     ThresholdFilter{threshold}, ShiftUpFilter{shift});
    auto pipeline = make_pipe(make_splitter(std::move(arm1), std::move(arm2), std::move(arm3),
                                            std::move(arm4), std::move(arm5)),
                              dpipe::DecouplerPlaceholder{}, RampUpSource{TOTAL_FRAMES});
    run_pipeline(pipeline, TOTAL_FRAMES);
    EXPECT_EQ(counter1, TOTAL_FRAMES);
    EXPECT_EQ(counter2, TOTAL_FRAMES - threshold);
    EXPECT_EQ(counter3, TOTAL_FRAMES);
    EXPECT_EQ(counter4, TOTAL_FRAMES - threshold);
    EXPECT_EQ(counter5, TOTAL_FRAMES + shift - threshold);
}
