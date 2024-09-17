
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
#include "myV792AC.h"                          // CAEN V792AC QDC class
#include "myV862AC.h"
#include "myV775N.h"                           // CAEN V775N TDC class
#include "myV775.h"                            // CAEN V775 TDC class

using namespace std;

uint32_t tracevalue(0);

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

const uint32_t nQDCs = 4;
const uint32_t QDCchans = 32;
const uint32_t QDCevsize = QDCchans+2;
const uint32_t QDCtotsize = QDCevsize*nQDCs;

const uint32_t ONEK = 1000;
const uint32_t ONEM = ONEK*ONEK;
const uint32_t ONEG = ONEK*ONEM;

const uint32_t V775Nchans = 16;

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
  fprintf(stderr,"%s cntrl_c_handler: sig %d tracevalue %d\n", stime, sig, tracevalue);
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

uint64_t xreadtime(0);

int main( int argc, char** argv )
{
  ofstream ofs;
  ifstream ifs;

  double readoutTime(0);

  uint32_t trignum(0);
  uint32_t Tcts[4] = { 0, 0, 0, 0 };

  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  signal(SIGUSR1, sigusr1_handler);     // Control-USR1 handler

  v2718 v2718_0("/V2718/cvA24_U_DATA/0");            // VME interface
  v2718_0.print();
  v2718_0.systemReset();
  sleep(1);

  for ( int out = cvOutput0; out <= cvOutput4; out++ ) 
   {
     CVOutputSelect cvout = static_cast<CVOutputSelect>(out);
     v2718_0.setOutputManual (cvout);
   }

  for ( int inp = cvInput0; inp <= cvInput1; inp++ )
   {
    CVInputSelect cvinp = static_cast<CVInputSelect>(inp);
    v2718_0.setInput (cvinp);
   }

  disableTriggers( v2718_0 );
  disablePedestals( v2718_0 );

  v513 ioreg(0xa00000,"/V2718/cvA24_U_DATA/0");          // I/O register

  resetScaler( v2718_0 );

  cout << " ****************** start of run ****************** " << endl << endl;
  time_t stime = time(NULL);
  cout << " time is " << stime << endl;
  struct tm mytime;
  localtime_r( &stime, &mytime );

  cout << mytime.tm_year+1900 << "." << mytime.tm_mon+1 << "." << mytime.tm_mday << "." << mytime.tm_hour 
                              << ":" << mytime.tm_min << ":" << mytime.tm_sec << endl;
  cout << asctime(&mytime) << endl;

  uint32_t runnbr;
  ifs.open ("/home/dreamtest/working/run.number",std::ofstream::in);
  ifs >> runnbr;
  ifs.close();
  ofs.open ("/home/dreamtest/working/run.number",std::ofstream::out);
  ofs << ++runnbr;
  ofs.close();

  stime -= 1600000000;
  std::stringstream s;
  s << std::setfill ('0') << std::setw (4) << runnbr; 
  string fname = "sps2024data.run" + s.str() + ".txt";
  string fname_r = "sps2024data_r.run" + s.str() + ".txt";
  string fname_z = "sps2024data_z.run" + s.str() + ".txt";

  if (argc == 2) fname = argv[1];

  cout << " filename is " << fname << endl;

  ofs.open (fname.c_str(), std::ofstream::out);

  time_t tr0 = time(NULL);
  bool running(true);

  volatile bool exit_now(false);

  uint32_t spillNr(0);

  uint32_t prevTrig(0), prevCts[4] = {0};

  struct timeval tode;

  unlockTrigger( v2718_0 );

  enableTriggers( v2718_0 );

  bool isSpill(false), wasSpill(false);

  while (1)
   {
    tracevalue = 1;
    isSpill = ((ioreg.readInputRegister() & 0x8000) != 0);
    tracevalue = 2;

    tracevalue = 3;
    if (!isSpill)
     {
      // cout << " 2 isSpill " << isSpill << " wasSpill " << wasSpill << endl;

      if (isSpill != wasSpill)
       {
        tracevalue = 4;
        wasSpill = false;
       }
      // cout << " 3 isSpill " << isSpill << " wasSpill " << wasSpill << endl;
      tracevalue = 5;
     }
    else
     {
      tracevalue = 6;
      if (isSpill != wasSpill)
       {
        spillNr ++;
        wasSpill = true;
       }

      uint32_t patt_a;

      v2718_0.readInputRegister ( &patt_a );
      patt_a &= 3;

      bool isTrig = (patt_a != 0);

      if (isTrig)
       {
        uint32_t tmask_a = patt_a | 4;

        if (!exit_now) unlockTrigger( v2718_0 );
       }
      tracevalue = 7;
     }
    if (abort_run) exit_now = true;
    if (exit_now)
     {
      disableTriggers(v2718_0);
      break;
     }
   }

  disableTriggers( v2718_0 );
  ofs.close();

  time_t etime = time(NULL);

  cout << " time is " << dec << etime << endl;
  struct tm myetime;
  localtime_r( &etime, &myetime );

  cout << myetime.tm_year+1900 << "." << myetime.tm_mon+1 << "." << myetime.tm_mday << "." 
       << myetime.tm_hour << ":" << myetime.tm_min << ":" << myetime.tm_sec << endl;
  cout << asctime(&myetime) << endl;

  etime -= 1600000000;
  uint32_t tdiff = etime-stime;
  double rate = trignum/double(tdiff);
  cout << dec << " time is " << etime << " diff(sec) " << (etime-stime) << " events " << trignum
       << " rate(Hz) " << rate << endl;

  cout << " ****************** end of run ****************** " << endl << endl;

  return 0;
}
