#include <chrono>
#include <optional>
#include <thread>

#include <dpipe/dpipe.h>

using namespace dpipe;

struct RawPayload {};
struct CalibPayload {};

struct RawSource {
    using OutputPayload = RawPayload;
    std::optional<Frame<OutputPayload>> produce() {
        return {};
    }
};

struct Transform {
    using InputPayload = RawPayload;
    using OutputPayload = CalibPayload;
    std::optional<Frame<OutputPayload>> process(Frame<InputPayload>&&) {
        return {};
    }
};

struct Display {
    using InputPayload = CalibPayload;
    void consume(Frame<InputPayload>&&) {}
};

int main() {
    auto pipeline = make_pipe(Display{}, Transform{}, RawSource{});
    pipeline.start();
    // ...
}
