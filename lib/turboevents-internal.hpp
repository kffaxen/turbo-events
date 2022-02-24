#ifndef TURBOEVENTS_INTERNAL_HPP
#define TURBOEVENTS_INTERNAL_HPP

#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <variant>

#include "IO/Serializers.hpp"

namespace TurboEvents {

/// Unique for ordering events with identical time.
extern uint64_t streamNum;

/// Helper for ensuring exhaustive matching in Config::makeEvent().
template <class> inline constexpr bool alwaysFalseV = false;

/// A type for events with time stamps and string payload.
struct Event {
  /// Constructor
  Event(std::chrono::system_clock::time_point t, std::string d)
      : time(t), data(d) {}
  /// Virtual destructor
  virtual ~Event() {}
  const std::chrono::system_clock::time_point time; ///< Time stamp of event.
  const std::string data;                           ///< Data of event.
};

/// Various configuration of the system.
class Config {
public:
  /// Constructor
  Config(char separator, std::chrono::system_clock::time_point t,
         bool timeshift)
      : start(t), tshift(timeshift), serializer(JoinFormat(separator)) {}

  /// Serialize arguments and make an event.
  template <typename... Args>
  std::unique_ptr<Event> makeEvent(std::chrono::system_clock::time_point t,
                                   Args &&...args) {
    return std::visit(
        [&t, &args...](auto &&arg) {
          // Update this function when adding a new type of serializer.
          //
          // This is just an exhaustive switch over all types in the
          // serializer variant declaration and a catch-all for when
          // forgetting to update the code. Everything is known
          // statically so the compiler will optimize this.
          using T = std::remove_cvref_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, JoinFormat>)
            return std::make_unique<Event>(
                t, arg.serialize(std::forward<Args>(args)...));
          else
            static_assert(alwaysFalseV<T>, "non-exhaustive visitor!");
        },
        serializer);
  }

  /// Start time of the system.
  const std::chrono::system_clock::time_point start;
  /// Whether to time shift.
  const bool tshift;

private:
  std::variant<JoinFormat> serializer; ///< The serializer to use.
};

/// A class for event streams where the events of the stream are delivered in
/// order
class EventStream {
public:
  /// Constructor
  EventStream() : id(streamNum++), time(std::chrono::system_clock::now()) {}
  /// Virtual destructor
  virtual ~EventStream() {}
  /// Get the current event.
  virtual Event *getEvent() const = 0;

  /// Try to generate an event, return true if successful.
  virtual bool generate(Config &cfg) = 0;
  /// Identity for deterministic ordering of events with same times.
  const uint64_t id;
  /// The time stamp of the current event.
  std::chrono::system_clock::time_point time;
};

/// A class encapsulating an input, such as a file
class Input {
public:
  /// Virtual destructor
  virtual ~Input() = default;

  /// Add the event streams in the input to the event generator.
  virtual void addStreams(Config &cfg,
                          std::function<void(EventStream *)> push) = 0;
  /// Deallocate resources used by the class.
  virtual void finish() = 0;
};

/// A class encapsulating an output destination
class Output {
public:
  /// Constructor.
  Output() {}
  /// Virtual destructor
  virtual ~Output() = default;

  /// Function to call when the time is right
  virtual void trigger(Event &e) = 0;
};

} // namespace TurboEvents

#endif
