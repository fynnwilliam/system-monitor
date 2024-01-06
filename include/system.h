#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>

#include "process.h"
#include "processor.h"

class main_system {
 public:
  const processor& cpu() const;
  static std::vector<process> processes();
  static float memory_utilization();
  static long up_time();
  static int total_processes();
  static int running_processes();
  static const std::string& kernel();
  static const std::string& operating_system();

 private:
  processor cpu_;
};

#endif // SYSTEM_H