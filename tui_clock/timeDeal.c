#include "timeDeal.h"
#include <time.h>

struct tm *getNowTime(void) {
  time_t now;
  time(&now);
  return localtime(&now);
}