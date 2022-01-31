#ifndef TURBOEVENTS_INTERNAL_HPP
#define TURBOEVENTS_INTERNAL_HPP

#include <chrono>
#include <string>

namespace TurboEvents {

class Output;

/// A type for events with time stamps
struct Event {
  /// Constructor
  Event(std::chrono::system_clock::time_point t) : time(t) {}
  /// Virtual destructor
  virtual ~Event() {}
  /// The time stamp of the event
  const std::chrono::system_clock::time_point time;
  /// Function to call when the time is right
  virtual void trigger() const = 0;
};

/// A class for event streams where the events of the stream are delivered in
/// order
class EventStream {
public:
  /// Constructor
  EventStream(Event *e) : next(e) {}
  /// Virtual destructor
  virtual ~EventStream() {}
  /// Get next event
  Event *getNext() const { return next; }

  /// Generate the next event and write it to next returning true if an event
  /// was found
  virtual bool generate(Output &output) = 0;

  /// The time stamp of the first event
  std::chrono::system_clock::time_point time;

protected:
  /// The next event
  Event *next;
};

} // namespace TurboEvents

#endif
