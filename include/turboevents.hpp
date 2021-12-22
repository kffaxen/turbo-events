#ifndef TURBOEVENTS_HPP
#define TURBOEVENTS_HPP

#include <chrono>
#include <memory>
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

/// A class encapsulating an event generator
class TurboEvents {
public:
  /// Simple constructor
  TurboEvents();
  /// Create a new TurboEvents object.
  static std::unique_ptr<TurboEvents> create();

  /// Virtual destructor
  virtual ~TurboEvents();

  /// Add an event stream
  void addEventStream(EventStream &s);

  /// Run the event streams added so far
  void run();

private:
  /// Compare EventStream objects based on time
  static bool greaterES(const EventStream *a, const EventStream *b) {
    return a->time > b->time;
  }

  /// Priority queue containing EventStreams
  std::priority_queue<EventStream *, std::vector<EventStream *>,
                      decltype(&greaterES)>
      q;
};

} // namespace TurboEvents

#endif
