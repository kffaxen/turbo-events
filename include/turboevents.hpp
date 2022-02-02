#ifndef TURBOEVENTS_HPP
#define TURBOEVENTS_HPP

#include <chrono>
#include <memory>

namespace TurboEvents {

struct Event;

/// A class encapsulating an output destination
class Output {
public:
  /// Virtual destructor
  virtual ~Output() = 0;

  /// Virtual function to make an event with a string payload
  virtual Event *makeEvent(std::chrono::system_clock::time_point,
                           std::string data) = 0;

  /// Virtual function to make an event with an int payload
  virtual Event *makeEvent(std::chrono::system_clock::time_point, int data) = 0;

protected:
  /// Common error handling function
  void unimp(std::string className, std::string typeName);
};

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

  /// Create a new PrintOutput object
  static std::unique_ptr<Output> createPrintOutput();

  /// Create an Output object based on command line flag
  static std::unique_ptr<Output> createOutput(std::string &s);

  /// Virtual destructor
  virtual ~TurboEvents();

  /// Run the event generator and process events.
  virtual void run(Output &output) = 0;
};

} // namespace TurboEvents

#endif
