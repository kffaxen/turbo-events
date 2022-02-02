#include "turboevents.hpp"
#include "IO/KafkaOutput.hpp"
#include "IO/PrintOutput.hpp"
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

  void addStreams(Output &, std::function<void(EventStream *)> push) override {
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
  bool generate(Output &output) override {
    if (next != nullptr) delete next;
    if (n <= 0) return false;
    next = output.makeEvent(std::chrono::system_clock::now() +
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

/// The real TurboEvents implementation.
class TurboEventsImpl : public TurboEvents {
public:
  /// Constructor.
  TurboEventsImpl() : inputs() {}
  ~TurboEventsImpl() {}

  void createXMLFileInput(const char *name) override;
  void createStreamInput(int m, int i) override;

  void setPrintOutput() override;
  void setKafkaOutput() override;

  void run() override;

private:
  /// The output for the run.
  std::unique_ptr<Output> output;
  /// The input sources for the run.
  std::vector<std::unique_ptr<Input>> inputs;
};

TurboEvents::TurboEvents() {}

TurboEvents::~TurboEvents() = default;

std::unique_ptr<TurboEvents> TurboEvents::create() {
  return std::make_unique<TurboEventsImpl>();
}

void TurboEventsImpl::createXMLFileInput(const char *name) {
  inputs.push_back(std::make_unique<XMLFileInput>(name));
}

void TurboEventsImpl::createStreamInput(int m, int i) {
  inputs.push_back(std::make_unique<StreamInput>(new SimpleEventStream(m, i)));
}

void TurboEventsImpl::setPrintOutput() {
  output = std::make_unique<PrintOutput>();
}

void TurboEventsImpl::setKafkaOutput() {
  output = std::make_unique<KafkaOutput>();
}

void TurboEventsImpl::run() {
  auto greaterES = [](const EventStream *a, const EventStream *b) {
    return a->time > b->time;
  };
  std::priority_queue<EventStream *, std::vector<EventStream *>,
                      decltype(greaterES)>
      q(greaterES);
  auto push = [&q, &output = output](EventStream *s) {
    if (s->generate(*output)) q.push(s);
  };
  for (auto &input : inputs) input->addStreams(*output, push);
  while (!q.empty()) {
    EventStream *es = q.top();
    q.pop();
    std::this_thread::sleep_until(es->time);
    es->getNext()->trigger();
    if (es->generate(*output))
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
