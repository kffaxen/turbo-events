#ifndef TURBOEVENTS_HPP
#define TURBOEVENTS_HPP

#include <chrono>
#include <memory>

namespace TurboEvents {

/// A type for events with time stamps
template <class T> struct Event {
  /// The time stamp of the event
  std::chrono::system_clock::time_point time;
  /// The event data
  T val;
};

/// A class for event streams
template <class T> class EventStream {
public:
  /// Get next event
  inline Event<T> getNext(void) { return next; }
  /// Generate the next event and write it to next
  virtual void generate(void) = 0;

private:
  Event<T> next;
};

/// A class encapsulating an event generator
class TurboEvents {
public:
  /// Simple constructor
  TurboEvents();
  /// Create a new TurboEvents object.
  static std::unique_ptr<TurboEvents> create();
};

} // namespace TurboEvents

#endif
