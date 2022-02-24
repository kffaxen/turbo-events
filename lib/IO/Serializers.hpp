#ifndef SERIALIZERS_HPP
#define SERIALIZERS_HPP

#include <sstream>

namespace TurboEvents {

/// Format arguments into a string separated by a character.
class JoinFormat {
public:
  /// Constructor.
  JoinFormat(char separator) : sep(separator) {}
  virtual ~JoinFormat() = default;

  /// Join arguments into a string separated by sep.
  template <typename H, typename... T>
  std::string serialize(H &&u, T &&...args) {
    std::stringstream s;
    ((s << u) << ... << (s << sep, args));
    return s.str();
  }

private:
  char sep; ///< Separator to use in join.
};

} // namespace TurboEvents

#endif
