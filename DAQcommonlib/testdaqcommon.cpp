
#include <iostream>

#include "daqcommon.h"

int main()
 {
  uTimer u;
  std::cout << " now is " << u.utime() << std::endl;
  u.setOffset();
  std::cout << " now is " << u.utime() << std::endl;

  u.start();
  
  // dlwait("next: sleep for 1s");
  std::cout << "next, sleep for 1s" << std::endl;
  x_nsleep(ONEG);
  std::cout << " time passed " << u.elapsedSeconds() << std::endl;

  // dlwait("next: sleep for 2s");
  std::cout << "next, sleep for 0.1s" << std::endl;
  x_usleep(100*ONEK);
  u.stop();
  std::cout << " time passed " << u.elapsedSeconds() << std::endl;

  return 0;
 }
