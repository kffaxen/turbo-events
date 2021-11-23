#ifndef TURBOEVENTS_HPP
#define TURBOEVENTS_HPP

#include <chrono>
#include <memory>

namespace TurboEvents {

/// A type for events with time stamps
struct Event {
  /// The time stamp of the event
  std::chrono::system_clock::time_point time;
  /// Function to call when the time is right
  virtual void trigger() = 0;
};

/// A class for event streams
class EventStream {
public:
  /// Get next event
  std::unique_ptr<Event> getNext() { return std::move(next); }

  /// Generate the next event and write it to next returning true if an event
  /// was found
  virtual bool generate() = 0;

  /// The time stamp of the first event
  std::chrono::system_clock::time_point time;

private:
  std::unique_ptr<Event> next;
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
