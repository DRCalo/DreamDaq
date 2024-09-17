
#include <ctime>

#include <iostream>

#include "myV513.h"

using namespace std;

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

#define TRIGGER_MASK 7                    // channels 0, 1, 2 -> 0 = PHYS TRIG, 1 = PED TRIG, 2 = IN SPILL
#define TRIGGER_OR_IN 7                   // channel 7 -> TRIGGER OR INPUT
#define PEDESTAL_VETO 8                   // channel 8 -> PEDESTAL VETO
#define DAQ_VETO 9                        // channel 9 -> DAQ VETO
#define UNLOCK_PED_TRIGGER 10             // channel 10 -> UNLOCK PEDESTAL TRIGGER
#define UNLOCK_PHYS_TRIGGER 11            // channel 11 -> UNLOCK PHYSICS TRIGGER
#define SCALER_RESET 12                   // channel 12 -> SCALER RESET

inline void enableTriggers( v513& ioreg ) { ioreg.clearOutputBit( DAQ_VETO ) ; }
inline void disableTriggers( v513& ioreg ) { ioreg.setOutputBit( DAQ_VETO ); }

inline void enablePedestal( v513& ioreg ) { ioreg.clearOutputBit( PEDESTAL_VETO ); }
inline void disablePedestal( v513& ioreg ) { ioreg.setOutputBit( PEDESTAL_VETO ); }

inline uint32_t ioregValue( v513& ioreg ) { return ioreg.readInputRegister(); }

inline bool isTrig( v513& ioreg ) { return (ioreg.readInputRegister() & 1<<TRIGGER_OR_IN); }
inline uint32_t getTrigMask( v513& ioreg ) { return (ioreg.readInputRegister() & TRIGGER_MASK); }

#define NSLEEPT 500                         // for sleeping 500 ns

inline void resetScaler( v513& ioreg )
 {
  nsleep(NSLEEPT);
  ioreg.setOutputBit( SCALER_RESET );
  nsleep(NSLEEPT);
  ioreg.clearOutputBit( SCALER_RESET );
 }

int initV513( v513& ioreg )
 {
  uint16_t w;
  ioreg.read16phys(0xFE, &w);
  cout << hex << " V513 FE " << w << endl;
  nsleep(NSLEEPT);
  ioreg.read16phys(0xFC, &w);
  cout << hex << " V513 FC " << w << endl;
  nsleep(NSLEEPT);
  ioreg.read16phys(0xFA, &w);
  cout << hex << " V513 FA " << w << endl;
  nsleep(NSLEEPT);
  w=0;
  ioreg.write16phys(0x48, w);
  nsleep(NSLEEPT);
  ioreg.write16phys(0x46, w);
  nsleep(NSLEEPT);
  ioreg.write16phys(0x42, w);
  nsleep(NSLEEPT);

  ioreg.reset();
  nsleep(NSLEEPT);

  ioreg.read16phys(0x04, &w);
  cout << hex << " V513 0x4 " << w << endl;
  nsleep(NSLEEPT);

  for (int i=0; i<8; i++)
   {
    int reg = 0x10+i*2;
    ioreg.setChannelInput(i);
    nsleep(NSLEEPT);
    ioreg.read16phys(reg, &w);
    cout << hex << " V513 input ch " << i << " reg " << reg << " " << w << endl;
    nsleep(NSLEEPT);
   }

  for (int i=8; i<16; i++)
   {
    int reg = 0x10+i*2;
    ioreg.setChannelOutput(i);
    nsleep(NSLEEPT);
    ioreg.read16phys(reg, &w);
    cout << hex << " V513 output ch " << i << " reg " << reg << " " << w << endl;
    nsleep(NSLEEPT);
   }

  disableTriggers( ioreg );

  return 0;
 }

v513 xio(0xa00000,"/V2718/cvA24_U_DATA/0");   // Desy setup // roberto 15.06.2021

int32_t main( int32_t , char** )
 {
  disableTriggers( xio );
  initV513 ( xio );
  return 0;
 }
