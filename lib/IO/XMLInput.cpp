#include "XMLInput.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

using namespace xercesc;

namespace TurboEvents {

/// Class managing the XML handling
class XMLInput {
public:
  /// Constructor
  XMLInput();

  /// Destructor
  virtual ~XMLInput();

  /// Open an XML-file and add one or more event streams based on its contents
  void addStreamsFromXMLFile(Output &output,
                             std::function<void(EventStream *)> push,
                             const char *fname, bool timeshift);

private:
  /// Event stream objects for open streams.
  std::vector<XMLEventStream *> streams;
};

static XMLInput *xmlInput = nullptr;

void XMLFileInput::addStreams(Output &output,
                              std::function<void(EventStream *)> push) {
  // First, ensure that the XML system is up and running.
  if (xmlInput == nullptr) xmlInput = new XMLInput();
  xmlInput->addStreamsFromXMLFile(output, push, fname.c_str(), tshift);
}

void XMLFileInput::finish() {
  delete xmlInput;
  xmlInput = nullptr;
}

XMLInput::XMLInput() {
  try {
    XMLPlatformUtils::Initialize();
  } catch (const XMLException &e) {
    exit(1);
  }
}

XMLInput::~XMLInput() {
  for (auto *stream : streams) delete stream;
  XMLPlatformUtils::Terminate();
}

void XMLInput::addStreamsFromXMLFile(Output &output,
                                     std::function<void(EventStream *)> push,
                                     const char *fname, bool timeshift) {
  XMLCh tempStr[100];
  XMLString::transcode("LS", tempStr, 99);
  DOMImplementation *impl =
      DOMImplementationRegistry::getDOMImplementation(tempStr);
  DOMLSParser *parser =
      impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, NULL);

  DOMDocument *doc = nullptr;

  try {
    doc = parser->parseURI(fname);
  } catch (const XMLException &toCatch) {
    char *message = XMLString::transcode(toCatch.getMessage());
    std::cerr << "Exception message is:\n" << message << "\n";
    XMLString::release(&message);
    exit(-1);
  } catch (const DOMException &toCatch) {
    char *message = XMLString::transcode(toCatch.msg);
    std::cerr << "Exception message is:\n" << message << "\n";
    XMLString::release(&message);
    exit(-1);
  } catch (...) {
    std::cerr << "Unexpected Exception\n";
    exit(-1);
  }

  XMLCh *tsAttr = XMLString::transcode("ts");
  XMLCh *eventTag = XMLString::transcode("event");
  XMLCh *valueAttr = XMLString::transcode("value");

  XMLCh *ns = XMLString::transcode("*");
  XMLCh *tag = XMLString::transcode("glucose_level");
  auto *myList = doc->getElementsByTagNameNS(ns, tag);
  XMLString::release(&ns);
  XMLString::release(&tag);

  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  std::chrono::nanoseconds shift(0);
  std::vector<Event *> events;
  for (XMLSize_t i = 0; i < myList->getLength(); ++i) {
    auto *elem = static_cast<DOMElement *>(myList->item(i));

    DOMNodeList *xmlEvents = elem->getElementsByTagName(eventTag);

    bool firstEvent = true;
    for (XMLSize_t idx = 0; idx < xmlEvents->getLength(); ++idx) {
      auto *attrs = xmlEvents->item(idx)->getAttributes();

      char *timeStamp =
          XMLString::transcode(attrs->getNamedItem(tsAttr)->getNodeValue());

      char *value =
          XMLString::transcode(attrs->getNamedItem(valueAttr)->getNodeValue());

      struct tm timeBuf = {};
      strptime(timeStamp, "%d-%m-%Y %H:%M:%S", &timeBuf);
      XMLString::release(&timeStamp);
      auto tp = std::chrono::system_clock::from_time_t(std::mktime(&timeBuf));

      if (firstEvent) {
        if (timeshift) shift = now - tp;
        firstEvent = false;
      }
      tp += shift;
      events.push_back(output.makeEvent(tp, value));
      // The value string has been converted to a std::string by the call above
      // and is no longer used.
      XMLString::release(&value);
    }
    XMLEventStream *stream = new XMLEventStream(std::move(events));
    push(stream);
    events.clear();
    streams.push_back(stream);
  }
  XMLString::release(&tsAttr);
  XMLString::release(&eventTag);
  XMLString::release(&valueAttr);
  parser->release();
}

XMLEventStream::XMLEventStream(std::vector<Event *> events)
    : EventStream(nullptr), eventVec(events), eventIdx(0) {}

bool XMLEventStream::generate(Output &) {
  if (next != nullptr) delete next;
  if (eventIdx >= eventVec.size()) return false;
  next = eventVec[eventIdx];
  eventIdx++;
  time = next->time;
  return true;
}

} // namespace TurboEvents
