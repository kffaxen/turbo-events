#ifndef KAFKAOUTPUT_HPP
#define KAFKAOUTPUT_HPP

#include "turboevents-internal.hpp"
#include <string>

namespace TurboEvents {

/// Class for sending Kafka events.
class KafkaEvent : public Event {
public:
  /// Public constructor
  KafkaEvent(std::chrono::system_clock::time_point t, std::string d)
      : Event(t), data(d) {}

  /// Implementation of trigger() for Kafka events
  void trigger() const override;

private:
  /// The data to send.
  std::string data;
};

} // namespace TurboEvents
#endif
