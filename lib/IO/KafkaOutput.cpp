#include "KafkaOutput.hpp"
#include <iostream>

namespace TurboEvents {

/// The callback for when events have been received.
class DeliveryReportCb : public RdKafka::DeliveryReportCb {
public:
  /// Callback method.
  void dr_cb(RdKafka::Message &message) {
    if (message.err())
      std::cerr << "% Message delivery failed: " << message.errstr() << "\n";
  }
};

KafkaOutput::KafkaOutput(std::string brokers, std::string caLoc,
                         std::string certLoc, std::string keyLoc,
                         std::string keyPw, std::string top)
    : topic(top) {
  std::string errstr;

  RdKafka::Conf *c = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
  if (c->set("bootstrap.servers", brokers, errstr) != RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << "\n";
    exit(1);
  }
  if (c->set("security.protocol", "ssl", errstr) != RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << "\n";
    exit(1);
  }
  if (c->set("ssl.ca.location", caLoc, errstr) != RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << "\n";
    exit(1);
  }
  if (c->set("ssl.certificate.location", certLoc, errstr) !=
      RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << "\n";
    exit(1);
  }
  if (c->set("ssl.key.location", keyLoc, errstr) != RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << "\n";
    exit(1);
  }
  if (c->set("ssl.key.password", keyPw, errstr) != RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << "\n";
    exit(1);
  }

  drCb = new DeliveryReportCb();
  if (c->set("dr_cb", drCb, errstr) != RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << "\n";
    exit(1);
  }

  p = RdKafka::Producer::create(c, errstr);
  if (!p) {
    std::cerr << "Failed to create producer: " << errstr << "\n";
    exit(1);
  }
  delete c;
}

KafkaOutput::~KafkaOutput() {
  p->poll(0);
  p->flush(10 * 1000);
  if (p->outq_len() > 0)
    std::cerr << "% " << p->outq_len() << " message(s) were not delivered\n";
  delete p;
  delete drCb;
}

void KafkaOutput::trigger(Event &e) {
retry:
  RdKafka::ErrorCode err = p->produce(topic, RdKafka::Topic::PARTITION_UA,
                                      RdKafka::Producer::RK_MSG_COPY,
                                      const_cast<char *>(e.data.c_str()),
                                      e.data.size(), NULL, 0, 0, NULL, NULL);

  if (err != RdKafka::ERR_NO_ERROR) {
    std::cerr << "% Failed to produce to topic " << topic << ": "
              << RdKafka::err2str(err) << "\n";
    if (err == RdKafka::ERR__QUEUE_FULL) {
      p->poll(1000);
      goto retry;
    }
  }
  p->poll(0);
}

} // namespace TurboEvents
