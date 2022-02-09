#include "turboevents.hpp"
#include "IO/CountDownInput.hpp"
#include "IO/KafkaOutput.hpp"
#include "IO/PrintOutput.hpp"
#include "IO/XMLInput.hpp"
#include <pybind11/embed.h>
#include <queue>
#include <thread>

namespace py = pybind11;

namespace TurboEvents {

Input::~Input() = default;

/// The real TurboEvents implementation.
class TurboEventsImpl : public TurboEvents {
public:
  /// Constructor.
  TurboEventsImpl() : inputs() {}
  ~TurboEventsImpl() {}

  void createXMLFileInput(const char *name, bool timeshift) override;
  void createCountDownInput(int m, int i) override;

  void setPrintOutput() override;
  void setKafkaOutput(std::string brokers, std::string topic) override;

  void run() override;

private:
  /// The output for the run.
  std::unique_ptr<Output> output;
  /// The input sources for the run.
  std::vector<std::unique_ptr<Input>> inputs;
};

PYBIND11_EMBEDDED_MODULE(TurboEvents, m) {
  py::class_<TurboEventsImpl>(m, "TurboEvents")
      .def(py::init())
      .def("createXMLFileInput", &TurboEventsImpl::createXMLFileInput)
      .def("createCountDownInput", &TurboEventsImpl::createCountDownInput)
      .def("setPrintOutput", &TurboEventsImpl::setPrintOutput)
      .def("setKafkaOutput", &TurboEventsImpl::setKafkaOutput)
      .def("run", &TurboEventsImpl::run);
}

TurboEvents::TurboEvents() {}

TurboEvents::~TurboEvents() = default;

std::unique_ptr<TurboEvents> TurboEvents::create() {
  return std::make_unique<TurboEventsImpl>();
}

void TurboEventsImpl::createXMLFileInput(const char *name, bool timeshift) {
  inputs.push_back(std::make_unique<XMLFileInput>(name, timeshift));
}

void TurboEventsImpl::createCountDownInput(int m, int i) {
  inputs.push_back(std::make_unique<CountDownInput>(m, i));
}

void TurboEventsImpl::setPrintOutput() {
  output = std::make_unique<PrintOutput>();
}

void TurboEventsImpl::setKafkaOutput(std::string brokers, std::string topic) {
  output = std::make_unique<KafkaOutput>(brokers, topic);
}

void TurboEvents::runScript(std::string &file) {
  py::scoped_interpreter guard{};

  py::object scope = py::module_::import("__main__").attr("__dict__");
  py::eval_file(file, scope);
}

void TurboEvents::runString(std::string &s) {
  py::scoped_interpreter guard{};

  py::object scope = py::module_::import("__main__").attr("__dict__");
  py::exec(s, scope);
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
