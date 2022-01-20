#ifndef PRINTOUTPUT_HPP
#define PRINTOUTPUT_HPP

#include "turboevents-internal.hpp"

#include <iostream>

namespace TurboEvents {

/// Event that prints its data to standard output.
template <typename T> class PrintEvent : public Event {
public:
  /// Constructor
  PrintEvent(std::chrono::system_clock::time_point t, T data)
      : Event(t), d(data) {}

  /// Destructor
  virtual ~PrintEvent() override {}

  /// Prints the data to standard output.
  void trigger() const override { std::cout << d << "\n"; }

private:
  const T d; ///< Value to print
};

} // namespace TurboEvents
#endif
