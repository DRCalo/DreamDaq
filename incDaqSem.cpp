
#include <iostream>

#include "daqSem.h"

int main ( int argc, char** argv )
 {
  daqSem Sem ( 0x40, sModeSlave );

  Sem.semInc();

  int dt = Sem.semGet();

  std::cout << " val is " << dt << std::endl;

  return 0;
 }
