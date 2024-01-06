#include "processor.h"

#include <chrono>
#include <fstream>
#include <string>
#include <thread>

namespace {
auto cpu_stat() noexcept {
  unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
  std::string _;
  std::ifstream{"/proc/stat"} >> _ >> user >> nice >> system >> idle >>
      iowait >> irq >> softirq >> steal;

  idle += iowait;
  system += irq + softirq;
  auto total = user + nice + system + steal + idle;

  return std::pair{total, idle};
}
}  // namespace

float processor::utilization() const {
  auto [prev_total, prev_idle] = cpu_stat();

  std::this_thread::sleep_for(std::chrono::seconds{1});

  auto [total, idle] = cpu_stat();

  total -= prev_total;
  idle -= prev_idle;

  const float delta = total - idle;
  return delta < 0.1f ? 0.01f : delta / total;
}