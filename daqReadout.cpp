
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

#include "daqcommon.h"
#include "daqSem.h"
#include "daqShm.h"

using namespace std;

uint32_t hereIam(0);

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

void readEvent() {}
void printEvent() {}

int main()
{
  signal( SIGINT, cntrl_c_handler );      // Control-C handler
  signal( SIGUSR1, sigusr1_handler );     // Control-USR1 handler

  daqSem mySem (0x40, sModeSlave );

  // initialise shared segment
  daqShm mShm ( 0x10, mModeSlave );

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

  *daqReadyToRun = 1;

  while (*askDaqToRun == 0) { x_usleep(1); }

  do
   {
    if (*triggerMask == 0)
     {
      sched_yield();
     }
    else
     {
      readEvent();
      printEvent();
      (*fromDaq) ++;
      cout << " *daqReadyToRun " << *daqReadyToRun
           << " *askDaqToRun " << *askDaqToRun
           << " *fromDaq " << *fromDaq
           << " *toDaq " << *toDaq
           << " *eventNr " << *eventNr
           << " *spillNr " << *spillNr
           << " *triggerMask " << *triggerMask << endl;
      *triggerMask = 0;
     }
   }
  while (*askDaqToRun == 1);

  *daqReadyToRun = 0;

  return 0;
}
