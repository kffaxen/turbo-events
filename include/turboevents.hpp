#ifndef TURBOEVENTS_HPP
#define TURBOEVENTS_HPP

#include <memory>

namespace TurboEvents {

/// A class encapsulating an event generator
class TurboEvents {
public:
  /// Simple constructor
  TurboEvents();
  /// Create a new TurboEvents object.
  static std::unique_ptr<TurboEvents> create();
};

} // namespace TurboEvents

#endif
