// uptime. How long has xv6 been up
#include "types.h"
#include "user.h"

// seconds per minute
#define SPM (60)
// seconds per hour
#define SPH (60 * SPM)

#define pad(x, y) if ((x) < 10) printf(1, "0"); printf(1, "%d%s", (x), (y));

int
main(void)
{
  int ticks = uptime();
  int ms = ticks % TPS; // TPS in pdx.h
  int s  = ticks / TPS;
  int hours = (s / SPH);
  int mins  = (s - (SPH * hours)) / 60;
  int secs  = (s - (hours * SPH) - (mins * SPM));

  pad(hours, ":"); // note that hours is not bounded, so may take more than 2 digits
  pad(mins,  ":");
  pad(secs,  ".");

  // milliseconds
  if (ms < 10)  printf(1, "0");
  if (ms < 100) printf(1, "0");
  printf(1, "%d\n", ms);

  exit();
}
