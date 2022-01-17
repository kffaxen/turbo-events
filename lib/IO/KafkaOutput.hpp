#include "turboevents-internal.hpp"

namespace TurboEvents {

/// Class for sending Kafka events.
class KafkaOutput : public Event {
  /// The data to send.
  std::string data;
  KafkaOutput(std::chrono::system_clock::time_point t, std::string d)
      : Event(t), data(d) {}
  void trigger() const override;
};

} // namespace TurboEvents
