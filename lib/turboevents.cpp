#include "turboevents.hpp"

#include <iostream>
#include <thread>

namespace TurboEvents {

bool TurboEvents::lessES(const EventStream *a, const EventStream *b) {
  return a->time > b->time;
}

TurboEvents::TurboEvents() : q(lessES) {
  std::cout << "TurboEvents initialized\n";
}

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
