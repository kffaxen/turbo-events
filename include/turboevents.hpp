#ifndef TURBOEVENTS_HPP
#define TURBOEVENTS_HPP

#include <chrono>
#include <memory>
#include <string>

namespace TurboEvents {

/// A class encapsulating an event generator
class TurboEvents {
public:
  /// Simple constructor
  TurboEvents();
  /// Virtual destructor
  virtual ~TurboEvents();

  /// Create a new TurboEvents object.
  static std::unique_ptr<TurboEvents> create();

  /// Create an input from the previous calls to addEvent.
  virtual void createContainerInput() = 0;
  /// Create a new StreamInput object.
  virtual void createCountDownInput(int m, int i = 200) = 0;
  /// Create a new XML file input.
  virtual void createXMLFileInput(const char *name, const char *ctrl) = 0;

  /// Set the output to Kafka.
  virtual void setKafkaOutput(bool timeshift, std::string brokers,
                              std::string caLocation, std::string certLocation,
                              std::string keyLocation, std::string keyPwd,
                              std::string topic) = 0;
  /// Set the output to print.
  virtual void setPrintOutput(bool timeshift) = 0;

  /// Run the file in Python.
  static void runScript(std::string &file);
  /// Run the string in Python.
  static void runString(std::string &s);

  /// Run the event generator and process events.
  virtual void run(double scale) = 0;

  /// Add an event to an internal container.
  virtual void addEvent(std::chrono::system_clock::time_point time,
                        std::string data) = 0;
};

} // namespace TurboEvents

#endif
