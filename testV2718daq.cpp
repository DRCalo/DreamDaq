
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

#include "myV2718.h"                           // CAEN V2718 VME Bridge class

using namespace std;

#define TRIGGER_MASK 3                    // in 0, 1 -> 0 = PHYS TRIG, 1 = PED TRIG, 2 = IN SPILL

#define PEDESTAL_VETO 6                   // out 0 -> PEDESTAL VETO
#define DAQ_VETO 7                        // out 1 -> DAQ VETO
#define UNLOCK_PED_TRIGGER 8              // out 2 -> UNLOCK PEDESTAL TRIGGER
#define UNLOCK_PHYS_TRIGGER 9             // out 3 -> UNLOCK PHYSICS TRIGGER
#define SCALER_RESET 10                   // out 4 -> SCALER RESET

void dlwait( string msg )
 {
  cout << "Press any key to continue ... " << msg;
  getchar();
  cout << endl;
 }

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

void myusleep (uint32_t us )
 {
  uint32_t secs = us / ONEM;
  uint32_t usrem = us % ONEM;
  struct timespec ndelay = { secs, usrem*ONEK };
  nanosleep ( &ndelay, NULL );
 }

inline void enableTriggers( v2718& ioreg ) { ioreg.clearOutputBit( DAQ_VETO ); }
inline void disableTriggers( v2718& ioreg ) { ioreg.setOutputBit( DAQ_VETO ); }

inline void enablePedestals( v2718& ioreg ) { ioreg.clearOutputBit( PEDESTAL_VETO ); }
inline void disablePedestals( v2718& ioreg ) { ioreg.setOutputBit( PEDESTAL_VETO ); }

inline void resetScaler( v2718& ioreg )
 {
  ioreg.setOutputBit( SCALER_RESET );
  ioreg.clearOutputBit( SCALER_RESET );
 }

inline void unlockTrigger( v2718& ioreg )
 {
  ioreg.setOutputBit( UNLOCK_PED_TRIGGER );
  ioreg.setOutputBit( UNLOCK_PHYS_TRIGGER );
  ioreg.clearOutputBit( UNLOCK_PED_TRIGGER );
  ioreg.clearOutputBit( UNLOCK_PHYS_TRIGGER );
 }

volatile bool abort_run(false);
volatile bool pause_run(false);

void cntrl_c_handler ( int32_t sig )
 {
  time_t timestr = time(NULL);
  char * stime = ctime(&timestr);
  stime[24] = 0;
  fprintf(stderr,"%s cntrl_c_handler: sig%d\n\n", stime, sig);
  fprintf(stderr,"aborting run\n");
  abort_run = true;
 }

void sigusr1_handler ( int32_t sig )
 {
  time_t timestr = time(NULL);
  char * stime = ctime(&timestr);
  stime[24] = 0;
  fprintf(stderr,"%s sigusr1_handler: sig%d  pause_run is %d\n\n", stime, sig, pause_run);
  pause_run = not pause_run;
  pause_run ?  fprintf(stderr,"pausing run\n") : fprintf(stderr,"resuming run\n");
 }

uint32_t initV2718 ( v2718 & v2718_m )
 {
  for ( int out = cvOutput0; out <= cvOutput4; out++ ) 
   {
     CVOutputSelect cvout = static_cast<CVOutputSelect>(out);
     v2718_m.setOutputManual (cvout);
   }

  for ( int inp = cvInput0; inp <= cvInput1; inp++ )
   {
    CVInputSelect cvinp = static_cast<CVInputSelect>(inp);
    v2718_m.setInput (cvinp);
   }

  disableTriggers( v2718_m );
  disablePedestals( v2718_m );
 }

int main( int argc, char** argv )
{
  uint32_t trignum(0);
  uint32_t Tcts[4] = { 0, 0, 0, 0 };

  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  signal(SIGUSR1, sigusr1_handler);     // Control-USR1 handler

  v2718 v2718_m("/V2718/cvA24_U_DATA/0");            // VME interface
  v2718_m.print();

  initV2718 ( v2718_m );

  unlockTrigger ( v2718_m );

  bool running (false);
  bool in_spill (false);

  while (1)
  {
    if (abort_run) exit_now = true;
    if (exit_now) disableTriggers(v2718_0);

    uint32_t z;

    xio.readInputRegister (&z);

    if (z == 0) 
      
  }

  disableTriggers( v2718_m );

  return 0;
}
