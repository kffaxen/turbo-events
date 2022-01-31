#include "IO/IO.hpp"
#include "IO/XMLInput.hpp"
#include "turboevents-internal.hpp"

#include <queue>
#include <thread>

namespace TurboEvents {

Input::~Input() = default;

/// An input class encapsulating a single event stream
class StreamInput : public Input {
public:
  /// Constructor
  StreamInput(EventStream *s) : stream(s) {}

  virtual ~StreamInput() {}

  void addStreams(std::function<void(EventStream *)> push) override {
    push(stream);
  }

  void finish() override { delete stream; }

private:
  /// The event stream
  EventStream *stream;
};

/// Dummy event stream
class SimpleEventStream : public EventStream {
public:
  /// Constructor
  SimpleEventStream(int m, int i = 1000)
      : EventStream(nullptr), n(m), interval(i) {}

  /// Generator
  bool generate() override {
    if (next != nullptr) delete next;
    if (n <= 0) return false;
    next = makeIntEvent(std::chrono::system_clock::now() +
                            std::chrono::milliseconds(interval),
                        n);
    time = next->time;
    n--;
    return true;
  }

private:
  int n;              ///< How many events to generate
  const int interval; ///< Interval in ms between events
};

TurboEvents::TurboEvents() {}

TurboEvents::~TurboEvents() {}

std::unique_ptr<TurboEvents> TurboEvents::create() {
  return std::make_unique<TurboEvents>();
}

std::unique_ptr<Input> TurboEvents::createXMLFileInput(const char *name) {
  return std::make_unique<XMLFileInput>(name);
}

std::unique_ptr<Input> TurboEvents::createStreamInput(int m, int i) {
  return std::make_unique<StreamInput>(new SimpleEventStream(m, i));
}

void TurboEvents::run(std::vector<std::unique_ptr<Input>> &inputs) {
  auto greaterES = [](const EventStream *a, const EventStream *b) {
    return a->time > b->time;
  };
  std::priority_queue<
      EventStream *, std::vector<EventStream *>,
      std::function<bool(const EventStream *, const EventStream *)>>
      q(greaterES);
  auto push = [&q](EventStream *s) {
    if (s->generate()) q.push(s);
  };
  for (auto &input : inputs) input->addStreams(push);
  while (!q.empty()) {
    EventStream *es = q.top();
    q.pop();
    std::this_thread::sleep_until(es->time);
    es->getNext()->trigger();
    if (es->generate())
      q.push(es); // Push the stream back on the queue if there are more events
  }
  for (auto &input : inputs) input->finish();
}

Output::~Output() = default;

void Output::unimp(std::string className, std::string typeName) {
  std::cerr << className << "::makeEvent(" << typeName << ") not implemented\n";
  exit(1);
}

} // namespace TurboEvents
