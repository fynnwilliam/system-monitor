#include "ncurses_display.h"

#include <curses.h>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "format.h"
#include "system.h"

// 50 bars uniformly displayed from 0 - 100 %
// 2% is one bar(|)
std::string ncurses_display::progress_bar(float percent) {
  std::string result{"0%"};
  int size{50};
  float bars{percent * size};

  for (int i{0}; i < size; ++i) {
    result += i <= bars ? '|' : ' ';
  }

  auto display{std::to_string(percent * 100).substr(0, 4)};
  if (percent < 0.1 || percent == 1.0)
    display = ' ' + std::to_string(percent * 100).substr(0, 3);
  return result + ' ' + display + "/100%";
}

void ncurses_display::display_system(const main_system& system,
                                     WINDOW* window) {
  int row{0};
  mvwprintw(window, ++row, 2, ("OS: " + system.operating_system()).c_str());
  mvwprintw(window, ++row, 2, ("Kernel: " + system.kernel()).c_str());
  mvwprintw(window, ++row, 2, "CPU: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "");
  wprintw(window, progress_bar(system.cpu().utilization()).c_str());
  wattroff(window, COLOR_PAIR(1));
  mvwprintw(window, ++row, 2, "Memory: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "");
  wprintw(window, progress_bar(system.memory_utilization()).c_str());
  wattroff(window, COLOR_PAIR(1));
  mvwprintw(
      window, ++row, 2,
      ("Total Processes: " + std::to_string(system.total_processes())).c_str());
  mvwprintw(window, ++row, 2,
            ("Running Processes: " + std::to_string(system.running_processes()))
                .c_str());
  mvwprintw(window, ++row, 2,
            ("Up Time: " + format::elapsed_time(system.up_time())).c_str());
  wrefresh(window);
}

void ncurses_display::display_processes(std::vector<process> processes,
                                        WINDOW* window, int n) {
  int row{0};
  int const pid_column{2};
  int const user_column{9};
  int const cpu_column{16};
  int const ram_column{26};
  int const time_column{35};
  int const command_column{46};
  wattron(window, COLOR_PAIR(2));
  mvwprintw(window, ++row, pid_column, "PID");
  mvwprintw(window, row, user_column, "USER");
  mvwprintw(window, row, cpu_column, "CPU[%%]");
  mvwprintw(window, row, ram_column, "RAM[MB]");
  mvwprintw(window, row, time_column, "TIME+");
  mvwprintw(window, row, command_column, "COMMAND");
  wattroff(window, COLOR_PAIR(2));
  for (int i = 0; i < n; ++i) {
    // You need to take care of the fact that the cpu utilization has already
    // been multiplied by 100.
    //  Clear the line
    mvwprintw(window, ++row, pid_column,
              (std::string(window->_maxx - 2, ' ').c_str()));

    mvwprintw(window, row, pid_column,
              std::to_string(processes[i].pid()).c_str());
    mvwprintw(window, row, user_column, processes[i].user().c_str());
    float cpu = processes[i].cpu_utilization() * 100;
    mvwprintw(window, row, cpu_column,
              std::to_string(cpu).substr(0, 4).c_str());
    mvwprintw(window, row, ram_column, processes[i].ram().c_str());
    mvwprintw(window, row, time_column,
              format::elapsed_time(processes[i].up_time()).c_str());
    mvwprintw(window, row, command_column,
              processes[i].command().substr(0, window->_maxx - 46).c_str());
  }
}

void ncurses_display::display(const main_system& system, int n) {
  initscr();      // start ncurses
  noecho();       // do not print input values
  cbreak();       // terminate ncurses on ctrl + c
  start_color();  // enable color

  int x_max{getmaxx(stdscr)};
  WINDOW* system_window = newwin(9, x_max - 1, 0, 0);
  WINDOW* process_window =
      newwin(3 + n, x_max - 1, system_window->_maxy + 1, 0);

  while (1) {
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    box(system_window, 0, 0);
    box(process_window, 0, 0);
    display_system(system, system_window);
    display_processes(system.processes(), process_window, n);
    wrefresh(system_window);
    wrefresh(process_window);
    refresh();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  endwin();
}