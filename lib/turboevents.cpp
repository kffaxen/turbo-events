#include "turboevents.hpp"
#include "IO/XMLInput.hpp"

#include <iostream>
#include <thread>

namespace TurboEvents {

static XMLInput *xmlInput = nullptr;

TurboEvents::TurboEvents() : q(greaterES) {
  std::cout << "TurboEvents initialized\n";
}

TurboEvents::~TurboEvents() {
  if (xmlInput != nullptr) delete xmlInput;
}

std::unique_ptr<TurboEvents> TurboEvents::create() {
  return std::make_unique<TurboEvents>();
}

void TurboEvents::addEventStream(EventStream *s) {
  if (s->getNext() != nullptr)
    q.push(s);
  else
    delete s;
}

void TurboEvents::addStreamsFromFile(const char *fileName) {
  // For now we assume it is an XML file
  // First, ensure that the XML system is up and running
  if (xmlInput == nullptr) {
    xmlInput = new XMLInput();
  }
  xmlInput->addStreamsFromXMLFile(this, fileName);
}

void TurboEvents::run() {
  while (!q.empty()) {
    EventStream *es = q.top();
    q.pop();
    std::this_thread::sleep_until(es->time);
    es->getNext()->trigger();
    if (es->generate())
      q.push(es); // Push the stream back on the queue if there are more events
    else
      delete es; // Delete the stream when there are no more events
  }
}

} // namespace TurboEvents
