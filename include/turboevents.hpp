#ifndef TURBOEVENTS_HPP
#define TURBOEVENTS_HPP

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

namespace TurboEvents {

class EventStream;
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

/// A class encapsulating an input, such as a file
class Input {
public:
  /// Virtual destructor
  virtual ~Input() = 0;

  /// Add the event streams in the input to the event generator.
  virtual void addStreams(Output &output,
                          std::function<void(EventStream *)> push) = 0;
  /// Deallocate resources used by the class.
  virtual void finish() = 0;
};

/// A class encapsulating an event generator
class TurboEvents {
public:
  /// Simple constructor
  TurboEvents();
  /// Create a new TurboEvents object.
  static std::unique_ptr<TurboEvents> create();
  /// Create a new XML file input.
  static std::unique_ptr<Input> createXMLFileInput(const char *name);
  /// Create a new StreamInput object.
  static std::unique_ptr<Input> createStreamInput(int m, int i = 1000);

  /// Create a new PrintOutput object
  static std::unique_ptr<Output> createPrintOutput();

  /// Virtual destructor
  virtual ~TurboEvents();

  /// Run the event generator and process events.
  void run(Output &output, std::vector<std::unique_ptr<Input>> &input);
};

} // namespace TurboEvents

#endif
