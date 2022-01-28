#ifndef TURBOEVENTS_HPP
#define TURBOEVENTS_HPP

#include <functional>
#include <memory>
#include <vector>

namespace TurboEvents {

class EventStream;

/// A class encapsulating an input, such as a file
class Input {
public:
  /// Virtual destructor
  virtual ~Input() = 0;

  /// Add the event streams in the input to the event generator.
  virtual void addStreams(std::function<void(EventStream *)> push) = 0;
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

  /// Virtual destructor
  virtual ~TurboEvents();

  /// Run the event generator and process events.
  void run(std::vector<std::unique_ptr<Input>> &input);
};

} // namespace TurboEvents

#endif
