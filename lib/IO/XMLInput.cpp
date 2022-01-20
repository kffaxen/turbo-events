#include "XMLInput.hpp"
#include "IO.hpp"

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
  void addStreamsFromXMLFile(
      std::priority_queue<
          EventStream *, std::vector<EventStream *>,
          std::function<bool(const EventStream *, const EventStream *)>> &q,
      const char *fileName);

private:
  /// Parser objects for open files
  std::vector<DOMLSParser *> openDocs;
  /// Event stream objects for open streams.
  std::vector<XMLEventStream *> streams;
};

static XMLInput *xmlInput = nullptr;

void XMLFileInput::addStreams(
    std::priority_queue<
        EventStream *, std::vector<EventStream *>,
        std::function<bool(const EventStream *, const EventStream *)>> &q) {
  // First, ensure that the XML system is up and running.
  if (xmlInput == nullptr) xmlInput = new XMLInput();
  xmlInput->addStreamsFromXMLFile(q, fname);
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
  for (auto *parser : openDocs) parser->release();
  for (auto *stream : streams) delete stream;
  XMLPlatformUtils::Terminate();
}

void XMLInput::addStreamsFromXMLFile(
    std::priority_queue<
        EventStream *, std::vector<EventStream *>,
        std::function<bool(const EventStream *, const EventStream *)>> &q,
    const char *fileName) {
  XMLCh tempStr[100];
  XMLString::transcode("LS", tempStr, 99);
  DOMImplementationLS *impl = static_cast<DOMImplementationLS *>(
      DOMImplementationRegistry::getDOMImplementation(tempStr));
  DOMLSParser *parser =
      impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

  DOMDocument *doc = nullptr;

  try {
    doc = parser->parseURI(fileName);
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

  XMLCh *ns = XMLString::transcode("*");
  XMLCh *tag = XMLString::transcode("glucose_level");

  auto *myList = doc->getElementsByTagNameNS(ns, tag);
  XMLString::release(&ns);
  XMLString::release(&tag);

  for (XMLSize_t i = 0; i < myList->getLength(); ++i) {
    auto *elem = static_cast<DOMElement *>(myList->item(i));

    XMLCh *eventTag = XMLString::transcode("event");
    auto *stream = new XMLEventStream(elem->getElementsByTagName(eventTag));
    XMLString::release(&eventTag);

    q.push(stream);
    streams.push_back(stream);
  }
  openDocs.push_back(parser);
}

XMLEventStream::XMLEventStream(DOMNodeList *events)
    : EventStream(nullptr), xmlEvents(events), nextIdx(0) {
  generate();
}

bool XMLEventStream::generate() {
  if (next != nullptr) delete next;
  if (nextIdx >= xmlEvents->getLength()) return false;
  DOMNode *node = xmlEvents->item(nextIdx);
  nextIdx++;

  auto *attrs = node->getAttributes();

  XMLCh *tsAttr = XMLString::transcode("ts");
  char *timeStamp =
      XMLString::transcode(attrs->getNamedItem(tsAttr)->getNodeValue());
  XMLString::release(&tsAttr);

  XMLCh *valueAttr = XMLString::transcode("value");
  char *value =
      XMLString::transcode(attrs->getNamedItem(valueAttr)->getNodeValue());
  XMLString::release(&valueAttr);

  struct tm timeBuf;
  strptime(timeStamp, "%d-%m-%Y %H:%M:%S", &timeBuf);
  XMLString::release(&timeStamp);
  auto tp = std::chrono::system_clock::from_time_t(std::mktime(&timeBuf));

  next = makeStringEvent(tp, value);
  // The value string has been converted to a std::string by the call above and
  // is no longer used.
  XMLString::release(&value);

  time = next->time;
  return true;
}

} // namespace TurboEvents
