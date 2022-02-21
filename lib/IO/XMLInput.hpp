#ifndef XMLINPUT_HPP
#define XMLINPUT_HPP

#include "ContainerInput.hpp"

namespace TurboEvents {

/// An input class encapsulating an XML input file
class XMLFileInput : public Input {
public:
  /// Constructor
  XMLFileInput(const char *fileName,
               std::vector<std::vector<std::string>> &ctrl)
      : fname(fileName), control(ctrl) {}
  virtual ~XMLFileInput() {}

  void addStreams(Output &output,
                  std::function<void(EventStream *)> push) override;

  void finish() override {}

private:
  /// The name of the file
  std::string fname;
  /// What information to extract from the XML file
  std::vector<std::vector<std::string>> control;
};

} // namespace TurboEvents
#endif
