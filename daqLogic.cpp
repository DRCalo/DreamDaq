
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <csignal>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sstream>
#include <iomanip>

#include "myModules.h"                         // VME Module Id.s
#include "myV2718.h"                           // CAEN V2718 VME Bridge class
#include "myV513.h"                            // CAEN V513 I/O REG class

#include "daqcommon.h"
#include "daqSem.h"
#include "daqShm.h"

using namespace std;

#define TRIGGER_MASK 7                    // channels 0, 1, 2 -> 0 = PHYS TRIG, 1 = PED TRIG, 2 = IN SPILL
#define TRIGGER_OR_IN 7                   // channel 7 -> TRIGGER OR INPUT

#define PEDESTAL_VETO 6                   // channel 8 -> PEDESTAL VETO
#define DAQ_VETO 7                        // channel 9 -> DAQ VETO
#define UNLOCK_PED_TRIGGER 8              // channel 10 -> UNLOCK PEDESTAL TRIGGER
#define UNLOCK_PHYS_TRIGGER 9             // channel 11 -> UNLOCK PHYSICS TRIGGER
#define SCALER_RESET 10                    // channel 12 -> SCALER RESET

#define V775_35ps 35                         // 35 ps as scale for V775
#define V775_100ps 100                       // 100 ps as scale for V775
#define V775_140ps 140                       // 140 ps as scale for V775

uint32_t hereIam(0);

inline void enableTriggers( v2718& ioreg ) { ioreg.clearOutputBit( DAQ_VETO ); }
inline void disableTriggers( v2718& ioreg ) { ioreg.setOutputBit( DAQ_VETO ); }

inline void enablePedestals( v2718& ioreg ) { ioreg.clearOutputBit( PEDESTAL_VETO ); }
inline void disablePedestals( v2718& ioreg ) { ioreg.setOutputBit( PEDESTAL_VETO ); }

inline void resetScaler( v2718& ioreg )
 {
  ioreg.setOutputBit( SCALER_RESET );
  ioreg.clearOutputBit( SCALER_RESET );
 }

inline void unlockTriggers( v2718& ioreg )
 {
  ioreg.setOutputBit( UNLOCK_PED_TRIGGER );
  ioreg.setOutputBit( UNLOCK_PHYS_TRIGGER );
  ioreg.clearOutputBit( UNLOCK_PED_TRIGGER );
  ioreg.clearOutputBit( UNLOCK_PHYS_TRIGGER );
 }

void initV2718( v2718& ioreg )
 {
  ioreg.print();
  ioreg.systemReset();

  x_usleep(10);

  for ( int out = cvOutput0; out <= cvOutput4; out++ ) 
   {
     CVOutputSelect cvout = static_cast<CVOutputSelect>(out);
     ioreg.setOutputManual (cvout);
   }

  for ( int inp = cvInput0; inp <= cvInput1; inp++ )
   {
    CVInputSelect cvinp = static_cast<CVInputSelect>(inp);
    ioreg.setInput (cvinp);
   }

  disableTriggers( ioreg );
  disablePedestals( ioreg );

 }

void initV513( v513& ioreg )
 {
  uint16_t w;

  ioreg.read16phys(0xFE, &w);
  cout << hex << " V513 FE " << w << endl;
  ioreg.read16phys(0xFC, &w);
  cout << hex << " V513 FC " << w << endl;
  ioreg.read16phys(0xFA, &w);
  cout << hex << " V513 FA " << w << endl;

  ioreg.write16phys(0x48, 0);
  ioreg.write16phys(0x46, 0);
  ioreg.write16phys(0x42, 0);
  
  ioreg.reset();
  
  ioreg.read16phys(0x04, &w); 
  cout << hex << " V513 0x4 " << w << endl;

 }

volatile bool abort_run(false);
volatile bool pause_run(false);

void cntrl_c_handler ( int32_t sig )
 {
  time_t timestr = time(NULL);
  char * stime = ctime(&timestr);
  stime[24] = 0;
  fprintf(stderr,"\n\n%s cntrl_c_handler: sig %d hereIam %d\n\n", stime, sig, hereIam);
  fprintf(stderr,"aborting run\n\n");
  abort_run = true;
 }

void sigusr1_handler ( int32_t sig )
 {
  time_t timestr = time(NULL);
  char * stime = ctime(&timestr);
  stime[24] = 0;
  fprintf(stderr,"\n\n%s sigusr1_handler: sig %d hereIam %d pause_run is %d\n\n", stime, sig, hereIam, pause_run);
  pause_run = not pause_run;
  pause_run ?  fprintf(stderr,"pausing run\n\n") : fprintf(stderr,"resuming run\n\n");
 }

uint64_t xreadtime(0);

int main()
{
  signal( SIGINT, cntrl_c_handler );      // Control-C handler
  signal( SIGUSR1, sigusr1_handler );     // Control-USR1 handler

  v2718 v2718_0( "/V2718/cvA24_U_DATA/0" );            // VME interface
  initV2718( v2718_0 );

  v513 ioreg( 0xa00000, "/V2718/cvA24_U_DATA/0" );          // I/O register
  initV513( ioreg );

  // initialise shared segment
  daqShm mShm ( 0x10, mModeMaster );

  volatile uint32_t* daqReadyToRun = mShm.daqReadyToRun();
  *daqReadyToRun = 0;
  volatile uint32_t* askDaqToRun = mShm.askDaqToRun();
  *askDaqToRun = 0;
  volatile uint32_t* fromDaq = mShm.fromDaq();
  *fromDaq = 0;
  volatile uint32_t* toDaq = mShm.toDaq();
  *toDaq = 0;
  volatile uint32_t* eventNr = mShm.eventNr();
  *eventNr = 0;
  volatile uint32_t* spillNr = mShm.spillNr();
  *spillNr = 0;
  volatile uint32_t* triggerMask = mShm.triggerMask();
  *triggerMask = 0;

  unlockTriggers( v2718_0 );

  resetScaler( v2718_0 );

  daqSem mySem (0x40, sModeMaster );

  while (*daqReadyToRun == 0) { x_usleep(1); }

  *askDaqToRun = 1;

  enableTriggers( v2718_0 );

  bool isSpill(false), wasSpill(false);

  volatile int sv;

  do
   {
    uint32_t trigPat;

    enableTriggers( v2718_0 );

    v2718_0.readInputRegister ( &trigPat );

    isSpill = (( ioreg.readInputRegister() & 0x8000) != 0 );

    if ( isSpill != wasSpill )
     {
      if ( isSpill ) (*spillNr) ++;
      cout << " isSpill: " << isSpill
           << " wasSpill " << wasSpill
           << " spillNr " << *spillNr << endl;
      wasSpill = isSpill;
     }

    bool isTrig = (trigPat != 0);
    if ( isTrig )
     {
      (*eventNr) ++;
      (*toDaq) ++;
      *triggerMask = trigPat | 4;
      while (*triggerMask != 0) { sched_yield(); }

      disableTriggers( v2718_0 );

      unlockTriggers( v2718_0 );
      x_usleep(1);

      v2718_0.readInputRegister ( &trigPat );
      cout << " isSpill: " << isSpill
           << " wasSpill " << wasSpill
           << " spillNr " << *spillNr
           << " trigPat " << trigPat << endl;
      wasSpill = isSpill;
     }
    sv = mySem.semGet();
   }
  while ((sv == 0) && !abort_run);

  disableTriggers( v2718_0 );

  *askDaqToRun = 0;

  while (*daqReadyToRun == 1) { x_usleep(1); }

  return 0;
}
