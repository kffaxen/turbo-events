#ifndef TURBOEVENTS_INTERNAL_HPP
#define TURBOEVENTS_INTERNAL_HPP

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace TurboEvents {

class EventStream;
struct Event;

/// Unique for ordering events with identical time.
extern uint64_t streamNum;

/// A class encapsulating an output destination
class Output {
public:
  /// Virtual destructor
  virtual ~Output() = 0;

  /// Virtual function to make an event with a string payload
  virtual std::unique_ptr<Event>
  makeEvent(std::chrono::system_clock::time_point, std::string data) = 0;

  /// Virtual function to make an event with an int payload
  virtual std::unique_ptr<Event>
  makeEvent(std::chrono::system_clock::time_point, int data) = 0;

protected:
  /// Common error handling function
  void unimp(std::string className, std::string typeName);
};

/// A class encapsulating an input, such as a file
class Input {
public:
  /// Virtual destructor
  virtual ~Input() = 0;

  /// Add the event streams in the input to the event generator.
  virtual void addStreams(Output &output,
                          std::function<void(EventStream *)> push) = 0;
  /// Deallocate resources used by the class.
  virtual void finish() = 0;
};

/// A type for events with time stamps
struct Event {
  /// Constructor
  Event(std::chrono::system_clock::time_point t) : time(t) {}
  /// Virtual destructor
  virtual ~Event() {}
  /// Function to call when the time is right
  virtual void trigger() const = 0;
  /// The time stamp of the event
  const std::chrono::system_clock::time_point time;
};

/// A class for event streams where the events of the stream are delivered in
/// order
class EventStream {
public:
  /// Constructor
  EventStream() : id(streamNum++), time(std::chrono::system_clock::now()) {}
  /// Virtual destructor
  virtual ~EventStream() {}
  /// Get the current event.
  virtual Event *getEvent() const = 0;

  /// Try to generate an event, return true if successful.
  virtual bool generate(Output &output) = 0;
  /// Identity for deterministic ordering of events with same times.
  const uint64_t id;
  /// The time stamp of the current event.
  std::chrono::system_clock::time_point time;
};

} // namespace TurboEvents

#endif
