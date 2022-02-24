#ifndef KAFKAOUTPUT_HPP
#define KAFKAOUTPUT_HPP

#include "turboevents-internal.hpp"
#include <librdkafka/rdkafkacpp.h>
#include <string>

namespace TurboEvents {

class DeliveryReportCb;

/// Output object that creates Kafka events
class KafkaOutput : public Output {
public:
  /// Constructor.
  KafkaOutput(std::string brokers, std::string caLoc, std::string certLoc,
              std::string keyLoc, std::string keyPw, std::string top);
  /// Destructor
  virtual ~KafkaOutput() override;

  virtual void trigger(Event &e) override;

private:
  /// The Kafka producer.
  RdKafka::Producer *p;
  /// Internal callback handle.
  DeliveryReportCb *drCb;
  /// The topic to send the data to.
  std::string topic;
};
} // namespace TurboEvents
#endif
