#include "turboevents.hpp"

#include <iostream>

namespace TurboEvents {

TurboEvents::TurboEvents() { std::cout << "TurboEvents initialized\n"; }

std::unique_ptr<TurboEvents> TurboEvents::create() {
  return std::make_unique<TurboEvents>();
}

} // namespace TurboEvents
