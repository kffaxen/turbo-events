#ifndef XMLINPUT_HPP
#define XMLINPUT_HPP

#include <xercesc/dom/DOM.hpp>

#include "turboevents.hpp"

using namespace xercesc;

namespace TurboEvents {

/// Placeholder event class
class XMLEvent : public Event {
public:
  /// Constructor
  XMLEvent(std::chrono::system_clock::time_point t, std::string data)
      : Event(t), d(data) {}

  /// Virtual destructor
  ~XMLEvent() {}

  /// XML specific implementation of trigger
  void trigger() const override;

private:
  /// Event data
  std::string d;
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

/// Class managing the XML handling
class XMLInput {
public:
  /// Constructor
  XMLInput();

  /// Destructor
  virtual ~XMLInput();

  /// Open an XML-file and add one or more event streams based on its contents
  void addStreamsFromXMLFile(TurboEvents *turbo, const char *fileName);

private:
  /// Parser objects for open files
  std::vector<DOMLSParser *> openDocs;
};

} // namespace TurboEvents
#endif
