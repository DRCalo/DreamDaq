
#include <iostream>

#include "myV259N.h"

using namespace std;

bool isT1T2Trig(v259n& xio)  { return (xio.readPatternRegister() & 1<<7); }
bool isHWBusy(v259n& xio) { return (xio.readPatternRegister() & 1<<4); }
bool isInSpill(v259n& xio) { return (xio.readPatternRegister() & 1<<5); }
bool isPedTrig(v259n& xio) { return (xio.readPatternRegister() & 1<<6); }

int32_t main( int32_t argc, char** argv )
 {
  if (argc == 1) return -1;
  uint32_t ads;
  int result = sscanf (argv[1], "%x", &ads);
  v259n xio(ads,"/V2718/cvA24_U_DATA/0");   // Desy setup // roberto 15.06.2021
  xio.resetAll();
  xio.print();
  cout << " Multiplicity register " << xio.readMultiplicityRegister() << " PatternRegister " << xio.readPatternRegister()
       << " read & reset multiplicity register " << xio.readResetMultiplicityRegister() << " readResetPatternRegisteratternRegister " << xio.readResetPatternRegister() << endl;
  return 0;
 }
