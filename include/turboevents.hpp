#ifndef TURBOEVENTS_HPP
#define TURBOEVENTS_HPP

#include <memory>

namespace TurboEvents {

/// A class encapsulating an event generator
class TurboEvents {
public:
  /// Simple constructor
  TurboEvents();
  /// Create a new TurboEvents object.
  static std::unique_ptr<TurboEvents> create();
  /// Create a new XML file input.
  virtual void createXMLFileInput(const char *name) = 0;
  /// Create a new StreamInput object.
  virtual void createStreamInput(int m, int i = 1000) = 0;

  /// Set the output to print.
  virtual void setPrintOutput() = 0;
  /// Set the output to Kafka.
  virtual void setKafkaOutput() = 0;

  /// Virtual destructor
  virtual ~TurboEvents();

  /// Run the event generator and process events.
  virtual void run() = 0;
};

} // namespace TurboEvents

#endif
