
#include <iostream>

#include "daqShm.h"

int main ( int argc, char** argv )
 {
  daqShm mShm ( 0x10, mModeMaster );

  volatile uint32_t* data = mShm.shmPtr();

  data[0] = 0xcafe;
  data[1] = 0xdeca;
  data[2] = 0xcade;
  data[3] = 0x8765;
  data[4] = 0x1234;

  sleep(5);

  return 0;
 }
