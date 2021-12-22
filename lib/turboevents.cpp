#include "turboevents.hpp"
#include "IO/XMLInput.hpp"

#include <iostream>
#include <thread>

namespace TurboEvents {

static XMLInput xmlInput;

TurboEvents::TurboEvents() : q(greaterES) {
  std::cout << "TurboEvents initialized\n";
}

TurboEvents::~TurboEvents() {}

std::unique_ptr<TurboEvents> TurboEvents::create() {
  return std::make_unique<TurboEvents>();
}

void TurboEvents::addEventStream(EventStream &s) { q.push(&s); }

void TurboEvents::run() {
  while (!q.empty()) {
    EventStream *es = q.top();
    q.pop();
    std::this_thread::sleep_until(es->time);
    es->getNext()->trigger();
    if (es->generate())
      q.push(es); // Push the stream back on the queue if there are more events
  }
}

} // namespace TurboEvents
