#ifndef KAFKAOUTPUT_HPP
#define KAFKAOUTPUT_HPP

#include "turboevents-internal.hpp"
#include <string>

namespace TurboEvents {

/// Class for sending Kafka events.
class KafkaEvent : public Event {
public:
  /// Public constructor
  KafkaEvent(std::string broker, std::string top,
             std::chrono::system_clock::time_point t, std::string d)
      : Event(t), brokers(broker), topic(top), data(d) {}

  /// Implementation of trigger() for Kafka events
  void trigger() const override;

private:
  /// The brokers for Kafka.
  std::string brokers;
  /// The topic to send the data to.
  std::string topic;
  /// The data to send.
  std::string data;
};

/// Output object that creates Kafka events
class KafkaOutput : public Output {
public:
  /// Constructor.
  KafkaOutput(std::string broker, std::string top)
      : brokers(broker), topic(top) {}
  /// Destructor
  virtual ~KafkaOutput() override {}

  /// Make an event that is published to a topic
  Event *makeEvent(std::chrono::system_clock::time_point t,
                   std::string data) override {
    return new KafkaEvent(brokers, topic, t, data);
  }

  /// Make an event that prints an int
  Event *makeEvent(std::chrono::system_clock::time_point, int) override {
    unimp("KafkaOutput", "int");
    return nullptr;
  }

private:
  /// The brokers for Kafka.
  std::string brokers;
  /// The topic to send the data to.
  std::string topic;
};
} // namespace TurboEvents
#endif
