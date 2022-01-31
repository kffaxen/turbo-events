#ifndef XMLINPUT_HPP
#define XMLINPUT_HPP

#include "turboevents-internal.hpp"
#include "turboevents.hpp"

namespace TurboEvents {

/// An input class encapsulating an XML input file
class XMLFileInput : public Input {
public:
  /// Constructor
  XMLFileInput(const char *fileName) : fname(fileName) {}
  virtual ~XMLFileInput() {}

  void addStreams(Output &output,
                  std::function<void(EventStream *)> push) override;

  void finish() override;

private:
  /// The name of the file
  const char *fname;
};

/// An event stream that is read from an XML file
class XMLEventStream : public EventStream {
public:
  /// Constructor
  XMLEventStream(std::vector<Event *> events);

  /// Destructor
  virtual ~XMLEventStream() {}

  /// Generator
  bool generate(Output &output) override;

private:
  /// The events of this stream
  std::vector<Event *> eventVec;

  /// Index of next event to generate
  size_t eventIdx;
};

} // namespace TurboEvents
#endif
