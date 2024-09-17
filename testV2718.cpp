
#include <ctime>
#include <cstdint>
#include <iostream>

#include "myV2718.h"


using namespace std;
// inline bool isT1T2Trig(v513& xio)  { return (xio.readInputRegister() & 1<<7); }
// inline bool isHWBusy(v513& xio) { return (xio.readInputRegister() & 1<<4); }
// inline bool isInSpill(v513& xio) { return (xio.readInputRegister() & 1<<5); }
// inline bool isPedTrig(v513& xio) { return (xio.readInputRegister() & 1<<6); }

const uint32_t ONEK = 1000;
const uint32_t ONEM = ONEK*ONEK;
const uint32_t ONEG = ONEK*ONEM;

void nsleep ( uint32_t ns )
 {
  uint32_t secs = ns / ONEG;
  uint32_t nsrem = ns % ONEG;
  struct timespec ndelay = { secs, nsrem };
  nanosleep ( &ndelay, NULL );
 }

void initV2718 (v2718* ioreg)
 {
  for (int i = cvInput0; i <= cvInput1; i++)
   {
    CVInputSelect is = static_cast<CVInputSelect>(i);
    cout << " setInput " << ioreg->setInput (is) << endl;
   }
  for (int i = cvOutput0; i <= cvOutput4; i++)
   {
    CVOutputSelect os = static_cast<CVOutputSelect>(i);
    cout << " setOutputManual " << ioreg->setOutputManual (os) << endl;
   }
 }

int32_t main( int32_t argc, char** argv )
 {
  v2718 xio("/V2718/cvA24_U_DATA/0");   // Desy setup // roberto 15.06.2021
  xio.print();
  initV2718 ( &xio );
  uint32_t z;
  uint32_t readinreg = xio.readInputRegister(&z);
  cout << " readInputRegister " << readinreg << hex << " V2718 " << z << endl;

  sleep(1);

  readinreg = xio.readInputRegister(&z);
  cout << " readInputRegister " << readinreg << hex << " V2718 " << z << endl;

  while (1)
   {
    xio.setOutputBit(6);
    xio.readInputRegister(&z);
    xio.clearOutputBit(6);
    xio.readInputRegister(&z);
   }

  cout << " readInputRegister " << readinreg << hex << " V2718 " << z << endl;

  return 0;
 }
