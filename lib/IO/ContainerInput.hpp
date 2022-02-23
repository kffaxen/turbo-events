#ifndef CONTAINERINPUT_HPP
#define CONTAINERINPUT_HPP

#include "turboevents-internal.hpp"

namespace TurboEvents {

/// Event stream that generates events from a container.
class ContainerStream : public EventStream {
public:
  /// Constructor
  ContainerStream(std::vector<std::unique_ptr<Event>> v)
      : events(std::move(v)), ix(-1) {}
  virtual ~ContainerStream() {}

  Event *getEvent() const override { return events[ix].get(); }

  bool generate(Config &) override {
    // FIXME: (Clang-13) Use std::ssize(events) in RHS instead casting LHS.
    if (static_cast<size_t>(++ix) >= events.size()) return false;
    time = events[ix]->time;
    return true;
  }

private:
  std::vector<std::unique_ptr<Event>> events; ///< The events of the stream.
  ssize_t ix;                                 ///< Index of current event.
};

/// An input class for streams triggering events from an internal container.
class ContainerInput : public Input {
public:
  /// Constructor
  ContainerInput(std::vector<std::unique_ptr<Event>> v)
      : stream(std::make_unique<ContainerStream>(std::move(v))) {}

  virtual ~ContainerInput() {}

  void addStreams(Config &, std::function<void(EventStream *)> push) override {
    push(stream.get());
  }

  void finish() override {}

private:
  std::unique_ptr<EventStream> stream; ///< The event stream.
};

} // namespace TurboEvents

#endif
