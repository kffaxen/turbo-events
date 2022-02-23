#ifndef PRINTOUTPUT_HPP
#define PRINTOUTPUT_HPP

#include "turboevents-internal.hpp"

#include <iostream>

namespace TurboEvents {

/// Output object that creates PrintObject
class PrintOutput : public Output {
public:
  /// Constructor
  PrintOutput() {}
  /// Destructor
  virtual ~PrintOutput() override {}

  /// Prints the data to standard output.
  void trigger(Event &e) override { std::cout << e.data << "\n"; }
};

} // namespace TurboEvents
#endif
