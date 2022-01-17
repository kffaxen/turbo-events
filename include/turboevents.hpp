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

class Input;

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
  void addEventStream(EventStream *s);

  /// Add one or more event streams from a file
  void addStreamsFromFile(const char *fileName);

  /// Run the event streams added so far
  void run(std::vector<Input *> &input);

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

/// A class encapsulating an input, such as a file
class Input {
public:
  /// Virtual destructor
  virtual ~Input() {}

  /// A virtual function to add the event streams in the input to the event
  /// generator
  virtual void addStreams(TurboEvents &turbo) = 0;
};

/// An input class encapsulating an input file
class FileInput : public Input {
public:
  /// Constructor
  FileInput(const char *fileName) : fname(fileName) {}

  /// Virtual Destructor
  virtual ~FileInput() {}

  /// Add the streams contained in the file
  void addStreams(TurboEvents &turbo) override {
    turbo.addStreamsFromFile(fname);
  }

private:
  /// The name of the file
  const char *fname;
};

/// An input class encapsulating a single event stream
class StreamInput : public Input {
public:
  /// Constructor
  StreamInput(EventStream *s) : stream(s) {}

  /// Virtual Destructor
  virtual ~StreamInput() {}

  /// Add the stream
  void addStreams(TurboEvents &turbo) override {
    turbo.addEventStream(stream);
    stream = nullptr;
  }

private:
  /// The event stream
  EventStream *stream;
};

} // namespace TurboEvents

#endif
