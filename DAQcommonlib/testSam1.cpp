#include <cerrno>
#include <cstring>
#include <iostream>

#include "daqSem.h"

int main ( int argc, char** argv )
 {
  daqSem* Sem(NULL);
  do
   {
    try
     {
      Sem = new daqSem ( 0x10, sModeSlave );
     }
    catch (...)
     {
      Sem = NULL;
     }
   }
  while ( Sem == NULL);

  int re = Sem->semSet(0x80);
  std::cout << " re is " << re << std::endl;
  if (re == -1)
   {
    std::cout << " semSet failed: " << std::strerror(errno) << std::endl;
   }

  int dt = Sem->semGet();

  std::cout << " val is " << dt << std::endl;

  delete Sem;

  return 0;
 }
