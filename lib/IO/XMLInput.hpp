#ifndef XMLINPUT_HPP
#define XMLINPUT_HPP

#include "turboevents-internal.hpp"

namespace TurboEvents {

/// An input class encapsulating an XML input file
class XMLFileInput : public Input {
public:
  /// Constructor
  XMLFileInput(const char *fileName, bool timeshift)
      : fname(fileName), tshift(timeshift) {}
  virtual ~XMLFileInput() {}

  void addStreams(Output &output,
                  std::function<void(EventStream *)> push) override;

  void finish() override;

private:
  /// The name of the file
  std::string fname;
  /// Whether to time shift
  bool tshift;
};

/// An event stream that is read from an XML file
class XMLEventStream : public EventStream {
public:
  /// Constructor
  XMLEventStream(std::vector<std::unique_ptr<Event>> events);

  /// Destructor
  virtual ~XMLEventStream() {}

  Event *getEvent() const override { return eventVec[eventIdx].get(); }

  bool generate(Output &output) override;

private:
  /// The events of this stream
  std::vector<std::unique_ptr<Event>> eventVec;

  /// Index of current event
  ssize_t eventIdx;
};

} // namespace TurboEvents
#endif
