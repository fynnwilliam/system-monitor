#ifndef NCURSES_DISPLAY_H
#define NCURSES_DISPLAY_H

#include <curses.h>

#include "system.h"

namespace ncurses_display {
void display(const main_system& sys, int n = 10);
void display_system(const main_system& sys, WINDOW* window);
void display_processes(std::vector<process> processes, WINDOW* window, int n);
std::string progress_bar(float percent);
};  // namespace ncurses_display

#endif // NCURSES_DISPLAY_H