#include "IO/XMLInput.hpp"
#include "turboevents-internal.hpp"

#include <iostream>
#include <thread>

namespace TurboEvents {

/// An input class encapsulating a single event stream
class StreamInput : public Input {
public:
  /// Constructor
  StreamInput(EventStream *s) : stream(s) {}

  virtual ~StreamInput() {}

  void addStreams(std::priority_queue<
                  EventStream *, std::vector<EventStream *>,
                  std::function<bool(const EventStream *, const EventStream *)>>
                      &q) override {
    q.push(stream);
  }

  void finish() override { delete stream; }

private:
  /// The event stream
  EventStream *stream;
};

/// Dummy event type
class SimpleEvent : public Event {
public:
  /// Constructor
  SimpleEvent(int m, std::chrono::system_clock::time_point t)
      : Event(t), n(m) {}

  /// Destructor
  virtual ~SimpleEvent() override {}

  /// Trigger
  void trigger() const override { std::cout << "SimpleEvent " << n << "\n"; }

private:
  const int n; ///< Value to print
};

/// Dummy event stream
class SimpleEventStream : public EventStream {
public:
  /// Constructor
  SimpleEventStream(int m, int i = 1000)
      : EventStream(nullptr), n(m), interval(i) {
    (void)generate();
  }

  /// Generator
  bool generate() override {
    if (next != nullptr) delete next;
    if (n <= 0) return false;
    next = new SimpleEvent(n, std::chrono::system_clock::now() +
                                  std::chrono::milliseconds(interval));
    time = next->time;
    n--;
    return true;
  }

private:
  int n;              ///< How many events to generate
  const int interval; ///< Interval in ms between events
};

TurboEvents::TurboEvents() : q(greaterES) {
  std::cout << "TurboEvents initialized\n";
}

TurboEvents::~TurboEvents() {}

std::unique_ptr<TurboEvents> TurboEvents::create() {
  return std::make_unique<TurboEvents>();
}

Input *TurboEvents::createXMLFileInput(const char *name) {
  return new XMLFileInput(name);
}

Input *TurboEvents::createStreamInput(int m, int i) {
  return new StreamInput(new SimpleEventStream(m, i));
}

void TurboEvents::run(std::vector<Input *> &inputs) {
  for (Input *input : inputs) input->addStreams(q);
  while (!q.empty()) {
    EventStream *es = q.top();
    q.pop();
    std::this_thread::sleep_until(es->time);
    es->getNext()->trigger();
    if (es->generate())
      q.push(es); // Push the stream back on the queue if there are more events
  }
  for (Input *input : inputs) {
    input->finish();
    delete input;
  }
}

} // namespace TurboEvents
