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

/// Output object that creates Kafka events
class KafkaOutput : public Output {
public:
  /// Destructor
  virtual ~KafkaOutput() override {}

  /// Make an event that published to a topic
  Event *makeEvent(std::chrono::system_clock::time_point t,
                   std::string data) override {
    return new KafkaEvent(t, data);
  }

  /// Make an event that prints an int
  Event *makeEvent(std::chrono::system_clock::time_point, int) override {
    unimp("KafkaOutput", "int");
    return nullptr;
  }
};
} // namespace TurboEvents
#endif
