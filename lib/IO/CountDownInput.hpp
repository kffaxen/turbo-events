#include "turboevents-internal.hpp"

namespace TurboEvents {

/// Event stream that counts down to 0.
class CountDownEventStream : public EventStream {
public:
  /// Constructor
  CountDownEventStream(int m, int i)
      : EventStream(nullptr), n(m), interval(i) {}

  bool generate(Output &output) override {
    if (next != nullptr) delete next;
    if (n <= 0) return false;
    next = output.makeEvent(std::chrono::system_clock::now() +
                                std::chrono::milliseconds(interval),
                            n);
    time = next->time;
    --n;
    return true;
  }

private:
  int n;              ///< How many events to generate
  const int interval; ///< Interval in ms between events
};

/// An input class for streams that count down.
class CountDownInput : public Input {
public:
  /// Constructor
  CountDownInput(int m, int i)
      : stream(std::make_unique<CountDownEventStream>(m, i)) {}

  virtual ~CountDownInput() {}

  void addStreams(Output &, std::function<void(EventStream *)> push) override {
    push(stream.get());
  }

  void finish() override {}

private:
  /// The event stream
  std::unique_ptr<EventStream> stream;
};

} // namespace TurboEvents
