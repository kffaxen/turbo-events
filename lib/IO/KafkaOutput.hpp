#ifndef KAFKAOUTPUT_HPP
#define KAFKAOUTPUT_HPP

#include "turboevents-internal.hpp"
#include <string>

namespace TurboEvents {

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

  virtual void trigger(Event &e) override;

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
