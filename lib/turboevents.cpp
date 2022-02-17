#include "turboevents.hpp"
#include "IO/ContainerInput.hpp"
#include "IO/CountDownInput.hpp"
#include "IO/KafkaOutput.hpp"
#include "IO/PrintOutput.hpp"
#include "IO/XMLInput.hpp"
#include <pybind11/chrono.h>
#include <pybind11/embed.h>
#include <queue>
#include <thread>

namespace py = pybind11;

namespace TurboEvents {

uint64_t streamNum = 0;

/// The real TurboEvents implementation.
class TurboEventsImpl : public TurboEvents {
public:
  /// Constructor.
  TurboEventsImpl() : inputs() {}
  ~TurboEventsImpl() {}

  void createContainerInput() override;
  void createCountDownInput(int m, int i) override;
  void createXMLFileInput(const char *name) override;

  void setKafkaOutput(bool timeshift, std::string brokers,
                      std::string caLocation, std::string certLocation,
                      std::string keyLocation, std::string keyPwd,
                      std::string topic) override;
  void setPrintOutput(bool timeshift) override;

  void run(double scale) override;

  void addEvent(std::chrono::system_clock::time_point time,
                std::string data) override;

private:
  /// The output for the run.
  std::unique_ptr<Output> output;
  /// The input sources for the run.
  std::vector<std::unique_ptr<Input>> inputs;
  /// Intermediate events for createContainerInput.
  std::vector<std::unique_ptr<Event>> events;
};

PYBIND11_EMBEDDED_MODULE(TurboEvents, m) {
  py::class_<TurboEventsImpl>(m, "TurboEvents")
      .def(py::init())
      .def("createContainerInput", &TurboEventsImpl::createContainerInput)
      .def("createCountDownInput", &TurboEventsImpl::createCountDownInput)
      .def("createXMLFileInput", &TurboEventsImpl::createXMLFileInput)
      .def("setKafkaOutput", &TurboEventsImpl::setKafkaOutput)
      .def("setPrintOutput", &TurboEventsImpl::setPrintOutput)
      .def("run", &TurboEventsImpl::run)
      .def("addEvent", &TurboEventsImpl::addEvent);
}

TurboEvents::TurboEvents() {}

TurboEvents::~TurboEvents() = default;

std::unique_ptr<TurboEvents> TurboEvents::create() {
  return std::make_unique<TurboEventsImpl>();
}

void TurboEventsImpl::createContainerInput() {
  inputs.push_back(std::make_unique<ContainerInput>(std::move(events)));
}

void TurboEventsImpl::createCountDownInput(int m, int i) {
  inputs.push_back(std::make_unique<CountDownInput>(m, i));
}

void TurboEventsImpl::createXMLFileInput(const char *name) {
  inputs.push_back(std::make_unique<XMLFileInput>(name));
}

void TurboEventsImpl::setKafkaOutput(bool timeshift, std::string brokers,
                                     std::string caLocation,
                                     std::string certLocation,
                                     std::string keyLocation,
                                     std::string keyPwd, std::string topic) {
  output = std::make_unique<KafkaOutput>(
      timeshift, brokers, caLocation, certLocation, keyLocation, keyPwd, topic);
}

void TurboEventsImpl::setPrintOutput(bool timeshift) {
  output = std::make_unique<PrintOutput>(timeshift);
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

void TurboEventsImpl::run(double scale) {
  auto greaterES = [](const EventStream *a, const EventStream *b) {
    // std::priority_queue is not stable, use stream id as differentiator
    // to ensure determinism across runs.
    if (a->time == b->time) return a->id > b->id;
    return a->time > b->time;
  };
  std::priority_queue<EventStream *, std::vector<EventStream *>,
                      decltype(greaterES)>
      q(greaterES);
  auto push = [&q, &output = output](EventStream *s) {
    if (s->generate(*output)) q.push(s);
  };
  for (auto &input : inputs) input->addStreams(*output, push);
  const auto start = output->start;
  while (!q.empty()) {
    EventStream *es = q.top();
    q.pop();
    std::this_thread::sleep_until(start + scale * (es->time - start));
    es->getEvent()->trigger();
    if (es->generate(*output))
      q.push(es); // Push the stream back on the queue if there are more events
  }
  for (auto &input : inputs) input->finish();
}

void TurboEventsImpl::addEvent(std::chrono::system_clock::time_point time,
                               std::string data) {
  events.push_back(output->makeEvent(time, data));
}

} // namespace TurboEvents
