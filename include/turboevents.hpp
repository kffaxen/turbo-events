#ifndef TURBOEVENTS_HPP
#define TURBOEVENTS_HPP

#include <memory>
#include <string>

namespace TurboEvents {

/// A class encapsulating an event generator
class TurboEvents {
public:
  /// Simple constructor
  TurboEvents();
  /// Create a new TurboEvents object.
  static std::unique_ptr<TurboEvents> create();
  /// Create a new XML file input.
  virtual void createXMLFileInput(const char *name, bool timeshift) = 0;
  /// Create a new StreamInput object.
  virtual void createCountDownInput(int m, int i = 200) = 0;

  /// Set the output to print.
  virtual void setPrintOutput() = 0;
  /// Set the output to Kafka.
  virtual void setKafkaOutput(std::string brokers, std::string caLocation,
                              std::string certLocation, std::string keyLocation,
                              std::string keyPwd, std::string topic) = 0;

  /// Virtual destructor
  virtual ~TurboEvents();

  /// Run the file in Python.
  static void runScript(std::string &file);
  /// Run the string in Python.
  static void runString(std::string &s);

  /// Run the event generator and process events.
  virtual void run() = 0;
};

} // namespace TurboEvents

#endif
