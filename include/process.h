#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class process {  // Basic class for process representation
 public:
  int pid() const;
  std::string user() const;
  std::string command() const;
  float cpu_utilization() const;
  std::string ram() const;
  long int up_time() const;
  bool operator<(process const& a) const;

  process(const int);

 private:
  int pid_;
};

#endif // PROCESS_H