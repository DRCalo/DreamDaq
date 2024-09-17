
#include <iostream>
#include <cstdio>

#include "daqcommon.h"

void x_nsleep ( uint32_t ns )
 {
  uint32_t secs = ns / ONEG;
  uint32_t nsrem = ns % ONEG;
  struct timespec ndelay = { secs, nsrem };
  nanosleep ( &ndelay, NULL );
 }

void x_usleep (uint32_t us )
 {
  uint32_t secs = us / ONEM;
  uint32_t usrem = us % ONEM;
  struct timespec ndelay = { secs, usrem*ONEK };
  nanosleep ( &ndelay, NULL );
 }

void dlwait( std::string msg )
 {
  std::cout << "Press any key to continue ... " << msg;
  getchar();
  std::cout << std::endl;
 }

