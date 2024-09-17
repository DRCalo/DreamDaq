
#include <iostream>

#include "daqcommon.h"
#include "daqSem.h"
#include "myV513.h"

using namespace std;

int main()
 {
  v513 ioreg(0xa00000,"/V2718/cvA24_U_DATA/0");          // I/O register

  daqSem Sem ( 0x40, sModeSlave );                       // DAQ semaphore

  cout << "going to raise semaphore at end of spill ... " << endl;

  bool isSpill;
  uint16_t v;

  do
   {
    x_usleep (10000);                                    // sleep for 10 ms
    v = ioreg.readInputRegister();                       // read V513 I/O reg
    isSpill = ((v & 0x8000) != 0);                       // check spill status
   }
  while (isSpill);                                       // repeat if spill is ON

  Sem.semInc();                                          // raise semaphore

  int dt = Sem.semGet();

  cout << "v: " << hex << v << " - isSpill: " << dec << isSpill << " - semaphore: " << dt << endl;


  return 0;
 }
