#include "ncurses_display.h"
#include "system.h"

int main() {
  main_system system;
  ncurses_display::display(system);
}