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
  /// The start time for the XML subsystem.
  std::chrono::system_clock::time_point now;
  /// Event stream objects for open streams.
  std::vector<std::unique_ptr<ContainerStream>> streams;
};

static std::unique_ptr<XMLInput> xmlInput;

void XMLFileInput::addStreams(Output &output,
                              std::function<void(EventStream *)> push) {
  // First, ensure that the XML system is up and running.
  if (!xmlInput) xmlInput = std::make_unique<XMLInput>();
  xmlInput->addStreamsFromXMLFile(output, push, fname.c_str(), tshift);
}

XMLInput::XMLInput() : now(std::chrono::system_clock::now()) {
  try {
    XMLPlatformUtils::Initialize();
  } catch (const XMLException &e) {
    exit(1);
  }
}

XMLInput::~XMLInput() { XMLPlatformUtils::Terminate(); }

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
  XMLCh *patient = XMLString::transcode("patient");
  XMLCh *idAttr = XMLString::transcode("id");

  auto *patientList = doc->getElementsByTagName(patient);
  for (XMLSize_t pi = 0; pi < patientList->getLength(); pi++) {
    if (patientList->item(pi)->getNodeType() != DOMNode::ELEMENT_NODE) continue;

    auto *pat = static_cast<DOMElement *>(patientList->item(pi));
    char *patId = XMLString::transcode(
        pat->getAttributes()->getNamedItem(idAttr)->getNodeValue());

    XMLCh *ns = XMLString::transcode("*");
    XMLCh *tag = XMLString::transcode("glucose_level");
    auto *myList = pat->getElementsByTagNameNS(ns, tag);
    XMLString::release(&ns);
    XMLString::release(&tag);

    std::chrono::nanoseconds shift(0);
    std::vector<std::unique_ptr<Event>> events;
    for (XMLSize_t i = 0; i < myList->getLength(); ++i) {
      auto *elem = static_cast<DOMElement *>(myList->item(i));

      DOMNodeList *xmlEvents = elem->getElementsByTagName(eventTag);

      bool firstEvent = true;
      for (XMLSize_t idx = 0; idx < xmlEvents->getLength(); ++idx) {
        auto *attrs = xmlEvents->item(idx)->getAttributes();

        char *timeStamp =
            XMLString::transcode(attrs->getNamedItem(tsAttr)->getNodeValue());

        // Use strptime, libstdc++ has numerous issues with std::get_time
        // before 2022 (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78714).
        struct tm timeBuf = {};
        char *rv = strptime(timeStamp, "%d-%m-%Y %H:%M:%S", &timeBuf);
        if (!rv)
          std::throw_with_nested(std::runtime_error(
              "Could not parse time: '" + std::string(timeStamp) + "'"));
        XMLString::release(&timeStamp);
        auto tp = std::chrono::system_clock::from_time_t(std::mktime(&timeBuf));

        if (firstEvent) {
          if (timeshift) shift = now - tp;
          firstEvent = false;
        }
        tp += shift;

        std::time_t tptime = std::chrono::system_clock::to_time_t(tp);
        std::string csv(100, '\0');

        // The use of std::localtime is unfortunate since it is not guaranteed
        // to be reentrant. For the time being it works since turboevents is
        // single threaded.
        std::strftime(&csv[0], csv.size(), "%d-%m-%Y %H:%M:%S",
                      std::localtime(&tptime));

        char *value = XMLString::transcode(
            attrs->getNamedItem(valueAttr)->getNodeValue());

        csv += ",";
        csv += patId;
        csv += ",";
        csv += value;
        XMLString::release(&value);

        events.push_back(output.makeEvent(tp, csv));
      }
      streams.push_back(std::make_unique<ContainerStream>(std::move(events)));
      push(streams.back().get());
      events.clear();
    }
    XMLString::release(&patId);
  }
  XMLString::release(&idAttr);
  XMLString::release(&patient);
  XMLString::release(&tsAttr);
  XMLString::release(&eventTag);
  XMLString::release(&valueAttr);
  parser->release();
}

} // namespace TurboEvents
