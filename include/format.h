#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace format {
static std::string elapsed_time(long seconds) noexcept {
  const auto hours = seconds / 3'600;
  seconds %= 3'600;

  const auto minutes = seconds / 60;
  seconds %= 60;

  return std::to_string(hours) + ':' + std::to_string(minutes) +
         (seconds < 10 ? ":0" : ":") + std::to_string(seconds);
}
}  // namespace format

#endif  // FORMAT_H