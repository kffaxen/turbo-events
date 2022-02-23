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
  void addStreamsFromXMLFile(Config &cfg,
                             std::function<void(EventStream *)> push,
                             const char *fname,
                             std::vector<std::vector<std::string>> &control);

private:
  /// Event stream objects for open streams.
  std::vector<std::unique_ptr<ContainerStream>> streams;
  /// Traversing a parsed XML file
  void addStreamsFromNode(Config &cfg, std::function<void(EventStream *)> push,
                          std::string ctx, bool &firstEvent,
                          std::chrono::nanoseconds &shift,
                          std::vector<std::string> &str, DOMNode *node);
  /// Read attribute value from an XML element
  std::string getAttrVal(DOMNamedNodeMap *attrs, XMLCh *attrTag);
};

static std::unique_ptr<XMLInput> xmlInput;

void XMLFileInput::addStreams(Config &cfg,
                              std::function<void(EventStream *)> push) {
  // First, ensure that the XML system is up and running.
  if (!xmlInput) xmlInput = std::make_unique<XMLInput>();
  xmlInput->addStreamsFromXMLFile(cfg, push, fname.c_str(), control);
}

XMLInput::XMLInput() {
  try {
    XMLPlatformUtils::Initialize();
  } catch (const XMLException &e) {
    exit(1);
  }
}

XMLInput::~XMLInput() { XMLPlatformUtils::Terminate(); }

/***********************************************************************
 * The streams created by the addStreamsFromXMLFile() function consist
 * of events with payloads in the form of strings in CSV (comma
 * separated values) sytńtax.
 * The 'control' argument defines which streams to generate from an
 * XML file and it also determines what data should be included in
 * the payload strings.
 *
 * The syntax of the 'control' string is as follows:
 * 'control' should be a comma separated list of stream descriptors.
 * A stream descriptor is a sequence of element descriptors, separated
 *   by '/' (slash).
 * An element descriptor starts with an optional ':' (colon) followed by
 *   an element tag and a sequence of attribute descriptors.
 * An attribute descriptor consists of a ':' (colon) followed by an
 *   attribute name.
 *
 * Example: patient:id/glucose_level/event:ts:value
 * This control string consists of a single stream descriptor made
 * up of three element descriptors, 'patient:id', 'glucose_level', and
 * 'events:ts:value'.
 *
 * This stream descriptor is intended to work with for instance the
 * following XML file:
 *
 * <?xml version="1.0" encoding="UTF-8"?>
 * <patient id="0">
 *  <glucose_level>
 *   <event ts="12-01-2022  9:38:00" value="100"/>
 *   <event ts="12-01-2022  9:38:01" value="102"/>
 *  </glucose_level>
 * </patient>
 *
 * The stream descriptor defines both which streams should be generated
 * and what data their events should carry. In the example above, each
 * XML element with the tag 'glucose_level' that is nested inside an
 * element with the 'patient' tag generate one stream of events. Each of
 * these evets is built from an XML element tagged 'event' that is nested
 * inside the element tagged 'glucose_level'.
 *
 * Since there is only one 'patient' element and that element contains
 * only one 'glucose_level' element, only one stream will be generated.
 * That stream will include two events with the following payloads:
 *
 *  12-01-2022  9:38:00,0,100
 *  12-01-2022  9:38:01,0,101
 *
 * There will be one value field in the event payload string for each ':'
 * in the stream descriptor. Invariably, the time stamp of the event is
 * the first value. By convention, the first attribute descriptor in the
 * last (most deeply nested) element descriptor (':ts' in the example)
 * is used as the time stamp of the event, and will be the first part of
 * each event payload. The rest of the parts correspond to the other
 * occurrences of ':' in the stream descriptor (':id' and ':value' in
 * the example). If the colon is part of an attribute descriptor, the
 * value of the attribute is used. If it is an attribute of an enclosing
 * element, the same value will be used in each element of the stream.
 * If the colon is the optional colon starting an element descriptor,
 * that tag is the corresponding value. This mechanism is not used in
 * the example above.
 *
 * *********************************************************************/

void XMLInput::addStreamsFromXMLFile(
    Config &cfg, std::function<void(EventStream *)> push, const char *fname,
    std::vector<std::vector<std::string>> &control) {
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

  bool firstEvent = true;
  std::chrono::nanoseconds shift(0);
  for (auto &ctrl : control)
    addStreamsFromNode(cfg, push, "", firstEvent, shift, ctrl, doc);
  parser->release();
}

std::string XMLInput::getAttrVal(DOMNamedNodeMap *attrs, XMLCh *attrTag) {
  std::string result;
  auto *attr = attrs->getNamedItem(attrTag);
  if (attr != nullptr) {
    char *attrVal = XMLString::transcode(attr->getNodeValue());
    result = attrVal;
    XMLString::release(&attrVal);
  }
  return result;
}

void XMLInput::addStreamsFromNode(Config &cfg,
                                  std::function<void(EventStream *)> push,
                                  std::string ctx, bool &firstEvent,
                                  std::chrono::nanoseconds &shift,
                                  std::vector<std::string> &str,
                                  DOMNode *node) {
  std::vector<std::string> items;
  std::istringstream iss(str[0]);
  std::string item;
  while (std::getline(iss, item, ':')) items.push_back(item);
  str.erase(str.begin());

  // Recursive case; there are more element tags after this one.
  const bool moreTags = items[0].empty();
  if (moreTags) items.erase(items.begin());

  XMLCh *elemTag = XMLString::transcode(items[0].c_str());
  DOMNodeList *nodes = nullptr;
  if (node->getNodeType() == DOMNode::ELEMENT_NODE)
    nodes = static_cast<DOMElement *>(node)->getElementsByTagName(elemTag);
  else if (node->getNodeType() == DOMNode::DOCUMENT_NODE)
    nodes = static_cast<DOMDocument *>(node)->getElementsByTagName(elemTag);
  XMLString::release(&elemTag);
  if (!nodes) return;

  std::vector<XMLCh *> attrTags;
  for (auto i = 0; auto &t : items)
    if (i++ > 0) attrTags.push_back(XMLString::transcode(t.c_str()));

  const std::string comma = ",";
  if (moreTags) ctx += comma + items[0];

  if (!str.empty()) {
    for (XMLSize_t nodeIdx = 0; nodeIdx < nodes->getLength(); ++nodeIdx) {
      std::string lctx = ctx;
      DOMNode *lnode = nodes->item(nodeIdx);
      for (XMLCh *attrTag : attrTags)
        lctx += comma + getAttrVal(lnode->getAttributes(), attrTag);
      addStreamsFromNode(cfg, push, lctx, firstEvent, shift, str, lnode);
    }
    for (XMLCh *attrTag : attrTags) XMLString::release(&attrTag);
    return;
  }

  // Read events
  // The first attribute tag is the name of the time stamp, peel it off
  XMLCh *tsTag = attrTags[0];
  attrTags.erase(attrTags.begin());
  std::vector<std::unique_ptr<Event>> events;
  for (XMLSize_t nodeIdx = 0; nodeIdx < nodes->getLength(); ++nodeIdx) {
    auto *attrs = nodes->item(nodeIdx)->getAttributes();

    char *timeStamp =
        XMLString::transcode(attrs->getNamedItem(tsTag)->getNodeValue());

    // Use strptime, libstdc++ has numerous issues with std::get_time
    // before 2022 (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78714).
    struct tm timeBuf = {};
    char *rv = strptime(timeStamp, "%d-%m-%Y %H:%M:%S", &timeBuf);
    if (!rv)
      std::throw_with_nested(std::runtime_error("Could not parse time: '" +
                                                std::string(timeStamp) + "'"));
    XMLString::release(&timeStamp);
    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&timeBuf));

    if (firstEvent) {
      if (cfg.tshift) shift = cfg.start - tp;
      firstEvent = false;
    }
    tp += shift;

    std::time_t tptime = std::chrono::system_clock::to_time_t(tp);
    char buf[100];

    // The use of std::localtime is unfortunate since it is not guaranteed
    // to be reentrant. For the time being it works since turboevents is
    // single threaded.
    std::strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S",
                  std::localtime(&tptime));
    std::string csv = buf;
    csv += ctx;
    for (XMLCh *attrTag : attrTags) csv += comma + getAttrVal(attrs, attrTag);
    events.push_back(cfg.makeEvent(tp, csv));
  }
  streams.push_back(std::make_unique<ContainerStream>(std::move(events)));
  push(streams.back().get());
  XMLString::release(&tsTag);
  for (XMLCh *attrTag : attrTags) XMLString::release(&attrTag);
}

} // namespace TurboEvents
