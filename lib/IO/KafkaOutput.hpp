#ifndef KAFKAOUTPUT_HPP
#define KAFKAOUTPUT_HPP

#include "turboevents-internal.hpp"
#include <string>

namespace TurboEvents {

/// Class for sending Kafka events.
class KafkaEvent : public Event {
public:
  /// Public constructor
  KafkaEvent(std::string broker, std::string caLocation,
             std::string certLocation, std::string keyLocation,
             std::string keyPwd, std::string top,
             std::chrono::system_clock::time_point t, std::string d)
      : Event(t), brokers(broker), caLoc(caLocation), certLoc(certLocation),
        keyLoc(keyLocation), keyPw(keyPwd), topic(top), data(d) {}

  /// Implementation of trigger() for Kafka events
  void trigger() const override;

private:
  /// The brokers for Kafka.
  std::string brokers;
  /// The ca location.
  std::string caLoc;
  /// The certificate location.
  std::string certLoc;
  /// The key location.
  std::string keyLoc;
  /// The key password.
  std::string keyPw;
  /// The topic to send the data to.
  std::string topic;
  /// The data to send.
  std::string data;
};

/// Output object that creates Kafka events
class KafkaOutput : public Output {
public:
  /// Constructor.
  KafkaOutput(std::string broker, std::string caLocation,
              std::string certLocation, std::string keyLocation,
              std::string keyPwd, std::string top)
      : brokers(broker), caLoc(caLocation), certLoc(certLocation),
        keyLoc(keyLocation), keyPw(keyPwd), topic(top) {}
  /// Destructor
  virtual ~KafkaOutput() override {}

  /// Make an event that is published to a topic
  std::unique_ptr<Event> makeEvent(std::chrono::system_clock::time_point t,
                                   std::string data) override {
    return std::make_unique<KafkaEvent>(brokers, caLoc, certLoc, keyLoc, keyPw,
                                        topic, t, data);
  }

  /// Make an event that prints an int
  std::unique_ptr<Event> makeEvent(std::chrono::system_clock::time_point,
                                   int) override {
    unimp("KafkaOutput", "int");
    return nullptr;
  }

private:
  /// The brokers for Kafka.
  std::string brokers;
  /// The ca location.
  std::string caLoc;
  /// The certificate location.
  std::string certLoc;
  /// The key location.
  std::string keyLoc;
  /// The key password.
  std::string keyPw;
  /// The topic to send the data to.
  std::string topic;
};
} // namespace TurboEvents
#endif
