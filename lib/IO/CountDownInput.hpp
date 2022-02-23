#ifndef COUNTDOWNINPUT_HPP
#define COUNTDOWNINPUT_HPP

#include "turboevents-internal.hpp"

namespace TurboEvents {

/// Event stream that counts down to 0.
class CountDownEventStream : public EventStream {
public:
  /// Constructor
  CountDownEventStream(int m, int i)
      : n(m), interval(std::chrono::milliseconds(i)) {}

  Event *getEvent() const override { return event.get(); }

  bool generate(Config &cfg) override {
    time += interval;
    event = cfg.makeEvent(time, n);
    return n-- > 0;
  }

private:
  int n;                                    ///< How many events to generate
  const std::chrono::milliseconds interval; ///< Interval between events
  std::unique_ptr<Event> event;             ///< The current event.
};

/// An input class for streams that count down.
class CountDownInput : public Input {
public:
  /// Constructor
  CountDownInput(int m, int i)
      : stream(std::make_unique<CountDownEventStream>(m, i)) {}

  virtual ~CountDownInput() {}

  void addStreams(Config &, std::function<void(EventStream *)> push) override {
    push(stream.get());
  }

  void finish() override {}

private:
  /// The event stream
  std::unique_ptr<EventStream> stream;
};

} // namespace TurboEvents

#endif
