/*
 * utility functions
 * (c) 2020 D. Brian Larkins - larkinsb@rhodes.edu
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "util.h"


/*
 * get_wctime() - returns wall clock time in seconds as double
 */
double get_wctime(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec + 1E-6 * tv.tv_usec);
}
