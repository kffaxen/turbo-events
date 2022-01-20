#ifndef XMLINPUT_HPP
#define XMLINPUT_HPP

#include <xercesc/dom/DOM.hpp>

#include "turboevents-internal.hpp"

using namespace xercesc;

namespace TurboEvents {

/// An input class encapsulating an XML input file
class XMLFileInput : public Input {
public:
  /// Constructor
  XMLFileInput(const char *fileName) : fname(fileName) {}
  virtual ~XMLFileInput() {}

  void addStreams(std::priority_queue<
                  EventStream *, std::vector<EventStream *>,
                  std::function<bool(const EventStream *, const EventStream *)>>
                      &q) override;

  void finish() override;

private:
  /// The name of the file
  const char *fname;
};

/// An event stream that is read from an XML file
class XMLEventStream : public EventStream {
public:
  /// Constructor
  XMLEventStream(DOMNodeList *events);

  /// Destructor
  virtual ~XMLEventStream() {}

  /// Generator
  bool generate() override;

private:
  /// List of XML events to read from
  DOMNodeList *xmlEvents;

  /// Index of next event to deliver
  XMLSize_t nextIdx;
};

} // namespace TurboEvents
#endif
