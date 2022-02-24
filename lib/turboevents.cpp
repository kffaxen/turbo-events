#include "turboevents.hpp"
#include "IO/ContainerInput.hpp"
#include "IO/CountDownInput.hpp"
#include "IO/KafkaOutput.hpp"
#include "IO/PrintOutput.hpp"
#include "IO/XMLInput.hpp"
#include <pybind11/chrono.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <queue>
#include <thread>

namespace py = pybind11;

namespace TurboEvents {

uint64_t streamNum = 0;

/// The real TurboEvents implementation.
class TurboEventsImpl : public Config, public TurboEvents {
public:
  /// Constructor.
  TurboEventsImpl(char sep, bool timeshift)
      : Config(sep, std::chrono::system_clock::now(), timeshift), outputs(),
        inputs() {}
  ~TurboEventsImpl() {}

  void createContainerInput() override;
  void createCountDownInput(int m, int i) override;
  void createXMLFileInput(const char *name,
                          std::vector<std::vector<std::string>> &ctrl) override;

  void addKafkaOutput(std::string brokers, std::string caLocation,
                      std::string certLocation, std::string keyLocation,
                      std::string keyPwd, std::string topic) override;
  void addPrintOutput() override;

  void run(double scale) override;

  void addEvent(std::chrono::system_clock::time_point time,
                std::string data) override;

private:
  /// The outputs for the run.
  std::vector<std::unique_ptr<Output>> outputs;
  /// The input sources for the run.
  std::vector<std::unique_ptr<Input>> inputs;
  /// Intermediate events for createContainerInput.
  std::vector<std::unique_ptr<Event>> events;
};

PYBIND11_EMBEDDED_MODULE(TurboEvents, m) {
  py::class_<TurboEventsImpl>(m, "TurboEvents")
      .def(py::init<char, bool>())
      .def("createContainerInput", &TurboEventsImpl::createContainerInput)
      .def("createCountDownInput", &TurboEventsImpl::createCountDownInput)
      .def("createXMLFileInput", &TurboEventsImpl::createXMLFileInput)
      .def("addKafkaOutput", &TurboEventsImpl::addKafkaOutput)
      .def("addPrintOutput", &TurboEventsImpl::addPrintOutput)
      .def("run", &TurboEventsImpl::run)
      .def("addEvent", &TurboEventsImpl::addEvent);
}

TurboEvents::TurboEvents() {}

TurboEvents::~TurboEvents() = default;

std::unique_ptr<TurboEvents> TurboEvents::create(char sep, bool timeshift) {
  return std::make_unique<TurboEventsImpl>(sep, timeshift);
}

void TurboEventsImpl::createContainerInput() {
  inputs.push_back(std::make_unique<ContainerInput>(std::move(events)));
}

void TurboEventsImpl::createCountDownInput(int m, int i) {
  inputs.push_back(std::make_unique<CountDownInput>(m, i));
}

void TurboEventsImpl::createXMLFileInput(
    const char *name, std::vector<std::vector<std::string>> &ctrl) {
  inputs.push_back(std::make_unique<XMLFileInput>(name, ctrl));
}

void TurboEventsImpl::addKafkaOutput(std::string brokers,
                                     std::string caLocation,
                                     std::string certLocation,
                                     std::string keyLocation,
                                     std::string keyPwd, std::string topic) {
  outputs.push_back(std::make_unique<KafkaOutput>(
      brokers, caLocation, certLocation, keyLocation, keyPwd, topic));
}

void TurboEventsImpl::addPrintOutput() {
  outputs.push_back(std::make_unique<PrintOutput>());
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
  auto push = [&q, this](EventStream *s) {
    if (s->generate(*this)) q.push(s);
  };
  for (auto &input : inputs) input->addStreams(*this, push);
  const auto strt = start;
  while (!q.empty()) {
    EventStream *es = q.top();
    Event *e = es->getEvent();
    std::this_thread::sleep_until(strt + scale * (e->time - strt));
    for (auto &o : outputs) o->trigger(*e);
    q.pop();
    if (es->generate(*this))
      q.push(es); // Push the stream back on the queue if there are more events
  }
  for (auto &input : inputs) input->finish();
}

void TurboEventsImpl::addEvent(std::chrono::system_clock::time_point time,
                               std::string data) {
  events.push_back(makeEvent(time, data));
}

} // namespace TurboEvents
