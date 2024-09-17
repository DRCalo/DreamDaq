
#include <iostream>

#include "myV513.h"


using namespace std;

int32_t main()
 {
  v513 ioreg(0xa00000,"/V2718/cvA24_U_DATA/0");          // I/O register

  uint16_t v = ioreg.readInputRegister();
  bool isSpill = ((v & 0x8000) != 0);

  cout << " v " << hex << v << " isSpill " << dec << isSpill << endl;

  return 0;
 }
