#ifndef TURBOEVENTS_HPP
#define TURBOEVENTS_HPP

#include <functional>
#include <memory>
#include <queue>
#include <vector>

namespace TurboEvents {

class EventStream;
class Input;

/// A class encapsulating an event generator
class TurboEvents {
public:
  /// Simple constructor
  TurboEvents();
  /// Create a new TurboEvents object.
  static std::unique_ptr<TurboEvents> create();
  /// Create a new XML file input.
  static Input *createXMLFileInput(const char *name);
  /// Create a new StreamInput object.
  static Input *createStreamInput(int m, int i = 1000);

  /// Virtual destructor
  virtual ~TurboEvents();

  /// Run the event generator and process events.
  void run(std::vector<Input *> &input);

private:
  /// Priority queue containing EventStreams
  std::priority_queue<
      EventStream *, std::vector<EventStream *>,
      std::function<bool(const EventStream *, const EventStream *)>>
      q;
};

} // namespace TurboEvents

#endif
