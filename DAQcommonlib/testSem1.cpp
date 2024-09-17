
#include <iostream>

#include "daqSem.h"

int main ( int argc, char** argv )
 {
  daqSem Sem ( 0x10, sModeMaster );

  int dt = Sem.semGet();

  std::cout << " before val is " << dt << std::endl;

  sleep(5);

  int dz = Sem.semGet();

  std::cout << " after val is " << dz << std::endl;

  Sem.semDec();

  dz = Sem.semGet();

  std::cout << " after semDec val is " << dz << std::endl;

  return 0;
 }
 
