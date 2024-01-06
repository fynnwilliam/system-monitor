#include "process.h"

#include <unistd.h>

#include <cctype>
#include <fstream>
#include <vector>

process::process(const int pid) : pid_{pid} {}

int process::pid() const { return pid_; }

float process::cpu_utilization() const {
  auto command =
      "less /proc/" + std::to_string(pid_) +
      "/stat 2> null | cut -d ' ' -f14,15,16,17,22 > temp_file && less "
      "/proc/uptime | cut -d ' ' -f1 >> temp_file";
  std::system(command.c_str());

  float utime, stime, cutime, cstime, start_time, uptime;
  std::ifstream{"temp_file"} >> utime >> stime >> cutime >> cstime >>
      start_time >> uptime;
  const float wait_time = cutime + cstime;
  const float active_time = utime + stime;
  const float total_time = active_time + wait_time;
  auto time = active_time / total_time;

  const auto hertz = sysconf(_SC_CLK_TCK);
  const auto seconds = uptime - (start_time / hertz);
  if (utime < 1.f) return 0.f;
  return 100.f * ((time / hertz) / seconds);
}

std::string process::command() const {
  auto cmdline = "/proc/" + std::to_string(pid_) + "/cmdline";
  std::getline(std::ifstream{cmdline}, cmdline);
  return cmdline;
}

std::string process::ram() const {
  auto command =
      "less /proc/" + std::to_string(pid_) +
      "/status 2> null | grep VmSize: | tr -d ' \tkB' | cut -d ':' -f2 > "
      "temp_file";
  std::system(command.c_str());
  long vm_size{0};
  std::ifstream{"temp_file"} >> vm_size;

  auto ram = std::to_string(vm_size / 1'024.0);
  auto decimal_place = ram.find_first_of('.');
  return ram.substr(0, decimal_place + 2);
}

std::string process::user() const {
  auto command = "less /etc/passwd | grep :$(less /proc/" +
                 std::to_string(pid_) +
                 "/status 2> null | grep Uid: | sed "
                 "'s/\t/:/g' | cut -d ':' -f3): | cut -d ':' -f1 > temp_file";
  std::system(command.c_str());

  std::string username;
  std::ifstream{"temp_file"} >> username;
  return username;
}

long int process::up_time() const {
  auto command =
      "less /proc/" + std::to_string(pid_) +
      "/stat 2> null | cut -d ' ' -f22 > temp_file && less /proc/uptime "
      "| cut -d ' ' -f1 >> temp_file";
  std::system(command.c_str());

  long start_time, uptime{0};
  std::ifstream{"temp_file"} >> start_time >> uptime;
  if (uptime == 0) return 0;

  return uptime - (start_time / sysconf(_SC_CLK_TCK));
}

bool process::operator<(process const& p) const {
  return cpu_utilization() < p.cpu_utilization();
}