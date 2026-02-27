#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#define CLOCK_WIDTH 47
#define CLOCK_HEIGHT 5

const char *digits[11][5] = {
    {"█████", "█   █", "█   █", "█   █", "█████"}, // 0

    {"   ██", "  ███", "   ██", "   ██", "█████"}, // 1

    {"█████", "    █", "█████", "█    ", "█████"}, // 2

    {"█████", "    █", "█████", "    █", "█████"}, // 3

    {"█   █", "█   █", "█████", "    █", "    █"},
    // 4
    {"█████", "█    ", "█████", "    █", "█████"}, // 5

    {"█████", "█    ", "█████", "█   █", "█████"},
    // 6
    {"█████", "    █", "   █ ", "  █  ", "  █  "}, // 7

    {"█████", "█   █", "█████", "█   █", "█████"}, // 8

    {"█████", "█   █", "█████", "    █", "█████"}, // 9

    {"     ", "  █  ", "     ", "  █  ", "     "} // 10
};

void init_renderer(void) {
#ifdef _WIN32
  // Enable ANSI escape sequences and UTF-8 for Windows Console
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut != INVALID_HANDLE_VALUE) {
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
      dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      SetConsoleMode(hOut, dwMode);
    }
  }
  SetConsoleOutputCP(CP_UTF8);
#endif
  printf("\033[?25l"); // Hide cursor
  printf("\033[2J");   // Clear entire screen
  fflush(stdout);
}

void cleanup_renderer(void) {
  printf("\033[0m");   // Reset text formatting
  printf("\033[2J");   // Clear screen
  printf("\033[H");    // Move cursor to Home
  printf("\033[?25h"); // Show cursor
  fflush(stdout);
}

void render_frame(int h, int m, int s) {
  int cols, rows;
#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
    cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
  } else {
    cols = 80;
    rows = 24;
  }
#else
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  cols = w.ws_col;
  rows = w.ws_row;
#endif

  static int prev_cols = 0;
  static int prev_rows = 0;

  // Enforce minimum window size constraints
  if (cols < CLOCK_WIDTH + 4 || rows < CLOCK_HEIGHT + 4) {
    if (prev_cols != cols || prev_rows != rows) {
      printf("\033[2J"); // Clear screen only on size change to prevent flicker
      prev_cols = cols;
      prev_rows = rows;
    }
    int r = rows / 2;
    int c = (cols - 19) / 2;
    if (c > 0 && r > 0) {
      // Clear the line before writing to avoid trace artifacts
      printf("\033[%d;%dH\033[2KWindow too small!", r, c);
    }
    fflush(stdout);
    return;
  }

  // If the window size changed, clear the entire screen to remove artifacts
  if (prev_cols != cols || prev_rows != rows) {
    printf("\033[2J");
    prev_cols = cols;
    prev_rows = rows;
  }

  // Calculate center offsets (digits remain fixed size CLOCK_WIDTH x
  // CLOCK_HEIGHT)
  int start_y = (rows - CLOCK_HEIGHT) / 2;
  int start_x = (cols - CLOCK_WIDTH) / 2;

  int time_digits[8] = {h / 10, h % 10,
                        10, // colon
                        m / 10, m % 10,
                        10, // colon
                        s / 10, s % 10};

  // Buffer to avoid flickering during the drawing of a single frame
  char buffer[4096] = {0};
  int offset = 0;

  // Set neon cyan color
  offset += sprintf(buffer + offset, "\033[36m");

  for (int row = 0; row < CLOCK_HEIGHT; row++) {
    // Move cursor to the designated row and column start
    offset += sprintf(buffer + offset, "\033[%d;%dH", start_y + row, start_x);
    for (int i = 0; i < 8; i++) {
      offset += sprintf(buffer + offset, "%s", digits[time_digits[i]][row]);
      if (i < 7) {
        offset += sprintf(buffer + offset, " "); // space between characters
      }
    }
  }

  // Print the entire constructed frame at once
  printf("%s", buffer);
  fflush(stdout);
}
