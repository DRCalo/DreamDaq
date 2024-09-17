
#include <iostream>
#include <ctime>

#include "myV513.h"

// v513 xio(0xa00000,"/V2718/cvA24_U_DATA/1");
// v513 xio(0xa00000,"/V2718/cvA24_U_DATA/1");
v513 xio(0xa00000,"/V2718/cvA24_U_DATA/0");   // Desy setup // roberto 15.06.2021

using namespace std;

#define SLEEP1000ns 1000                         // for sleeping 1000 ns
#define NSLEEPT 1000

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

#define DAQ_VETO 11                       // channel 11
#define TRIGGER_UNLOCK 10                 // channel 10
#define SCALER_RESET 9                    // channel 9
#define PEDESTAL_VETO 8                   // channel 8

inline bool isT1T2Trig() { return (xio.readInputRegister() & 1<<7); }
inline bool isHWBusy() { return (xio.readInputRegister() & 1<<4); }
inline bool isInSpill() { return (xio.readInputRegister() & 1<<2); }
inline bool isPedTrig() { return (xio.readInputRegister() & 1<<1); }
inline bool isPhysTrig() { return (xio.readInputRegister() & 1<<0); }
inline void resetTrig() { nsleep(SLEEP1000ns); xio.setOutputBit( TRIGGER_UNLOCK ); nsleep(SLEEP1000ns); xio.clearOutputBit( TRIGGER_UNLOCK ); }
inline void enableTriggers() { xio.clearOutputBit( DAQ_VETO ) ; }
inline void disableTriggers() { xio.setOutputBit( DAQ_VETO ); }
inline void enablePedestal() { xio.clearOutputBit( PEDESTAL_VETO ); }

int initV513( )
 {
  uint16_t w;
  xio.read16phys(0xFE, &w);
  cout << hex << " V513 FE " << w << endl;
  nsleep(NSLEEPT);
  xio.read16phys(0xFC, &w);
  cout << hex << " V513 FC " << w << endl;
  nsleep(NSLEEPT);
  xio.read16phys(0xFA, &w);
  cout << hex << " V513 FA " << w << endl;
  nsleep(NSLEEPT);
  w=0;
  xio.write16phys(0x48, w);
  nsleep(NSLEEPT);
  xio.write16phys(0x46, w);
  nsleep(NSLEEPT);
  xio.write16phys(0x42, w);
  nsleep(NSLEEPT);
  
  xio.reset();
  nsleep(NSLEEPT);

  xio.read16phys(0x04, &w);
  cout << hex << " V513 0x4 " << w << endl;
  nsleep(NSLEEPT);

   for (int i=0; i<8; i++)
   {
    int reg = 0x10+i*2;
    xio.setChannelInput(i);
    nsleep(NSLEEPT);
    xio.read16phys(reg, &w);
    cout << hex << " V513 input ch " << i << " reg " << reg << " " << w << endl;
    nsleep(NSLEEPT);
   }

  for (int i=8; i<16; i++)
   {
    int reg = 0x10+i*2;
    xio.setChannelOutput(i);
    nsleep(NSLEEPT);
    xio.read16phys(reg, &w);
    cout << hex << " V513 output ch " << i << " reg " << reg << " " << w << endl;
    nsleep(NSLEEPT);
   }

  disableTriggers();
  
  return 0;

}



int32_t main( int32_t , char** )
 {
  uint16_t z;
  initV513();
  enableTriggers();
  enablePedestal();
  for (unsigned int i=0; ; i++)
  {
    xio.read16phys(0x04, &z);
    //if(i%100 == 0)
      cout << hex << " V513 0x4 " << z << " isT1T2Trig: " << isT1T2Trig() << " - isPhysTrig: " << isPhysTrig() << " - isPedTrig: " << isPedTrig() << " - isInSpill " << isInSpill() << " - isHWBusy " << isHWBusy() << endl;
    if (isPedTrig() | isPhysTrig()) resetTrig();
    //nsleep(10000000);
    sleep(1);
  } while(1);
  return 0;
 }
