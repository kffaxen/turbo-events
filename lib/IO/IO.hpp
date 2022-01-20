#ifndef IO_HPP
#define IO_HPP

#include "PrintOutput.hpp"

namespace TurboEvents {

static inline Event *makeStringEvent(std::chrono::system_clock::time_point t,
                                     std::string data) {
  return new PrintEvent<std::string>(t, data);
}

static inline Event *makeIntEvent(std::chrono::system_clock::time_point t,
                                  int data) {
  return new PrintEvent<int>(t, data);
}

} // namespace TurboEvents
#endif
