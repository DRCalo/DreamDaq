
#include <iostream>

#include "myV513.h"


using namespace std;
inline bool isT1T2Trig(v513& xio)  { return (xio.readInputRegister() & 1<<7); }
inline bool isHWBusy(v513& xio) { return (xio.readInputRegister() & 1<<4); }
inline bool isInSpill(v513& xio) { return (xio.readInputRegister() & 1<<5); }
inline bool isPedTrig(v513& xio) { return (xio.readInputRegister() & 1<<6); }

uint32_t checkRW (uint32_t k, v513& xio, uint16_t w )
 {
    uint16_t z;
    std::cout << dec << k << " W " << hex << w;
    xio.write16phys(0x04, w);
    usleep(1);
    xio.read16phys(0x04, &z);
    std::cout << " -> R " << z << endl;
    usleep(1);
    return z;
 }

int32_t main( int32_t argc, char** argv )
 {
  uint32_t ads;
  ads = 0xa00000;
  if (argc == 2)
   {
    int result = sscanf (argv[1], "%x", &ads);
    if (result != 1) return -1;
   }
  cout << hex << " V513 addres is " << ads << endl;
  v513 xio(ads,"/V2718/cvA24_U_DATA/0");   // Desy setup // roberto 15.06.2021
  uint16_t z;
  xio.print();
  xio.reset();
  xio.read16phys(0x04, &z);
  cout << hex << " V513 0x4 " << z << endl;
  cout << "isT1T2Trig: " << isT1T2Trig(xio) << " - isPedTrig: " << isPedTrig(xio) << " - isInSpill " << isInSpill(xio) << " - isHWBusy " << isHWBusy(xio) << endl;
 // exit(0);

sleep(1);
do
 {
  uint16_t w;
  xio.read16phys(0xFE, &w);
  cout << hex << " V513 FE " << w << endl;
sleep(1);
  xio.read16phys(0xFC, &w);
  cout << hex << " V513 FC " << w << endl;
sleep(1);
  xio.read16phys(0xFA, &w);
  cout << hex << " V513 FA " << w << endl;
usleep(1);
  w=0;
  xio.write16phys(0x48, w);
usleep(1);
  xio.write16phys(0x46, w);
usleep(1);
  xio.write16phys(0x42, w);
usleep(1);

  xio.read16phys(0x04, &w);
  cout << hex << " V513 0x4 " << w << endl;
usleep(1);

  for (int i=0; i<8; i++)
   {
    int reg07 = 0x10+i*2;
    // xio.write16phys(reg07, 7);
    xio.setChannelInput(i);
usleep(1);
    xio.read16phys(reg07, &w);
    cout << hex << " V513 " << reg07 << " " << w << " ------- ";
usleep(1);
    int reg8f = 0x20+i*2;
    // xio.write16phys(reg8f, 6);
    xio.setChannelOutput(8+i);
usleep(1);
    xio.read16phys(reg8f, &w);
    cout << hex << " V513 " << reg8f << " " << w << endl;
usleep(1);
   }

    for (int i=0; i<8; i++) xio.setChannelInput(i);
    for (int i=8; i<16; i++) xio.setChannelOutput(i);


usleep(1);
 }
while(0);

  uint32_t i(0);
  while(0)
   {
    uint16_t w, z;
    i ++;

    w = 0xff00;
    z = checkRW(i, xio, w);
    if (z != (w|4)) { cout << " ******** ERROR w != z " << w << " " << z << endl; break; }

    w = 0xf000;
    z = checkRW(i, xio, w);
    if (z != (w|4)) { cout << " ******** ERROR w != z " << w << " " << z << endl; break; }

    w = 0x0f00;
    z = checkRW(i, xio, w);
    if (z != (w|4)) { cout << " ******** ERROR w != z " << w << " " << z << endl; break; }

    w = 0x0300;
    z = checkRW(i, xio, w);
    if (z != (w|4)) { cout << " ******** ERROR w != z " << w << " " << z << endl; break; }

   }
  while (1)
   {
    uint16_t w;
    xio.write16phys(0x04, 0x8000);
    if (0) for (int i=0; i<1; i++) xio.read16phys(0x04, &w);
    xio.write16phys(0x04, 0x0);
    if (0) for (int i=0; i<1; i++) xio.read16phys(0x04, &w);
   }

  return 0;
 }
