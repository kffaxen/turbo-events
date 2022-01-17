#ifndef TURBOEVENTS_INTERNAL_HPP
#define TURBOEVENTS_INTERNAL_HPP

#include "turboevents.hpp"
#include <chrono>
#include <functional>
#include <queue>
#include <vector>

namespace TurboEvents {

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
  virtual bool generate() = 0;

  /// The time stamp of the first event
  std::chrono::system_clock::time_point time;

protected:
  /// The next event
  Event *next;
};

/// Compare EventStream objects based on time
static inline bool greaterES(const EventStream *a, const EventStream *b) {
  return a->time > b->time;
}

/// A class encapsulating an input, such as a file
class Input {
public:
  /// Virtual destructor
  virtual ~Input() {}

  /// A virtual function to add the event streams in the input to the event
  /// generator
  virtual void addStreams(
      std::priority_queue<EventStream *, std::vector<EventStream *>,
                          std::function<bool(const EventStream *,
                                             const EventStream *)>> &q) = 0;
  /// Deallocate resources used by the class.
  virtual void finish() = 0;
};

} // namespace TurboEvents

#endif
