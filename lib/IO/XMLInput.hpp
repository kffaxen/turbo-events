#ifndef XMLINPUT_HPP
#define XMLINPUT_HPP

#include "ContainerInput.hpp"

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

} // namespace TurboEvents
#endif
