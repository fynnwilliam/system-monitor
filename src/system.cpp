#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <fstream>

namespace {
std::string pretty_name() {
  std::system(
      "less /etc/os-release | grep PRETTY | cut -d '=' -f2 | cut -d '\"' -f2 > "
      "temp_file");
  std::string os;
  std::getline(std::ifstream{"temp_file"}, os);
  return os;
}

std::string kernel_version() {
  std::system("uname -r > temp_file");
  std::string kernel;
  std::getline(std::ifstream{"temp_file"}, kernel);
  return kernel;
}
}  // namespace

const processor& main_system::cpu() const { return cpu_; }

std::vector<process> main_system::processes() {
  std::system("ls /proc | grep -E ^[0-9]+$ > temp_file");
  std::vector<process> processes;
  std::ifstream temp_file{"temp_file"};

  int process_id;
  while (temp_file >> process_id) processes.emplace_back(process_id);

  return processes;
}

const std::string& main_system::kernel() {
  static const auto kernel = kernel_version();
  return kernel;
}

float main_system::memory_utilization() {
  std::system(
      "less /proc/meminfo | head -n 4 | tr -d ' kB' | cut -d ':' -f2 > "
      "temp_file");
  long double total_memory, free_memory;
  std::ifstream{"temp_file"} >> total_memory >> free_memory;

  return (total_memory - free_memory) / total_memory;
}

const std::string& main_system::operating_system() {
  static auto os = pretty_name();
  return os;
}

int main_system::running_processes() {
  std::system(
      "less /proc/stat | grep procs_running | cut -d ' ' -f2 > temp_file");
  int processes;
  std::ifstream{"temp_file"} >> processes;

  return processes;
}

int main_system::total_processes() {
  std::system("less /proc/stat | grep processes | cut -d ' ' -f2 > temp_file");
  int processes;
  std::ifstream{"temp_file"} >> processes;

  return processes;
}

long int main_system::up_time() {
  long main_uptime;
  std::ifstream{"/proc/uptime"} >> main_uptime;
  return main_uptime;
}