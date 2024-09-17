
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

#define TRIGGER_MASK 7                    // channels 0, 1, 2 -> 0 = PHYS TRIG, 1 = PED TRIG, 2 = IN SPILL
#define TRIGGER_OR_IN 7                   // channel 7 -> TRIGGER OR INPUT
#define PEDESTAL_VETO 8                   // channel 8 -> PEDESTAL VETO
#define DAQ_VETO 9                        // channel 9 -> DAQ VETO
#define UNLOCK_PED_TRIGGER 10             // channel 10 -> UNLOCK PEDESTAL TRIGGER
#define UNLOCK_PHYS_TRIGGER 11            // channel 11 -> UNLOCK PHYSICS TRIGGER
#define SCALER_RESET 12                   // channel 12 -> SCALER RESET

#define NSLEEPT 500                         // for sleeping 500 ns

#define V775_35ps 35                         // 35 ps as scale for V775
#define V775_100ps 100                       // 100 ps as scale for V775
#define V775_140ps 140                       // 140 ps as scale for V775

const uint32_t nQDCs = 4;
const uint32_t QDCchans = 32;
const uint32_t QDCevsize = QDCchans+2;
const uint32_t QDCtotsize = QDCevsize*nQDCs;

v792ac* qdcPtrs[nQDCs];

// const uint32_t qdcAds[nQDCs] = { 0x03000000, 0x04000000, 0x05000000, 0x06000000 }; // standar addressing
// const uint32_t qdcAds[nQDCs] = { 0x03000000, 0x1B110000, 0x05000000, 0x06000000 }; // modified address 2024.08.29
const uint32_t qdcAds[nQDCs] = { 0x1B110000, 0x05000000, 0x06000000, 0x03000000 }; // new standard addressing 2024.08.29

const uint32_t V775Nchans = 16;

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

inline void enableTriggers( v513& ioreg ) { ioreg.clearOutputBit( DAQ_VETO ) ; }
inline void disableTriggers( v513& ioreg ) { ioreg.setOutputBit( DAQ_VETO ); }

inline void enablePedestals( v513& ioreg ) { ioreg.clearOutputBit( PEDESTAL_VETO ); }
inline void disablePedestals( v513& ioreg ) { ioreg.setOutputBit( PEDESTAL_VETO ); }

inline uint32_t ioregValue( v513& ioreg ) { return ioreg.readInputRegister() & 0xffff; }

inline bool isTrig( v513& ioreg ) { return (ioreg.readInputRegister() & 1<<TRIGGER_OR_IN); }
inline uint32_t getTrigMask( v513& ioreg ) { return (ioreg.readInputRegister() & TRIGGER_MASK); }

inline void resetScaler( v513& ioreg )
 {
  nsleep(NSLEEPT);
  ioreg.setOutputBit( SCALER_RESET );
  nsleep(NSLEEPT);
  ioreg.clearOutputBit( SCALER_RESET );
 }

inline void unlockTrigger( v513& ioreg )
 {
  nsleep(NSLEEPT);
  ioreg.setOutputBit( UNLOCK_PED_TRIGGER );
  nsleep(NSLEEPT);
  ioreg.setOutputBit( UNLOCK_PHYS_TRIGGER );
  nsleep(NSLEEPT);
  ioreg.clearOutputBit( UNLOCK_PED_TRIGGER );
  nsleep(NSLEEPT);
  ioreg.clearOutputBit( UNLOCK_PHYS_TRIGGER );
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
  myusleep ( 10 );
  disablePedestals( ioreg );
  myusleep ( 10 );

  return 0;
 }

void initV792( v792ac* v792adc )
 {
  v792adc->print();
  v792adc->reset();
  v792adc->disableSlide();
  v792adc->disableOverflowSupp();
  v792adc->disableZeroSupp();
  v792adc->clearEventCounter();
  v792adc->clearData();
  uint16_t ped;
  ped = v792adc->getI1();
  uint32_t bid = v792adc->id();
  cout << "v792ac addr 0x" << hex << bid << dec
       << ": default ped I1 value is " << ped << endl;

  ped = 255;                  // >~ minimum possible Iped (see manual par. 2.1.2.1)
  v792adc->setI1(ped);       // set I1 current to define pedestal position
  ped =v792adc->getI1();     // in the range [0x00-0xFF] (see manual par. 4.34.2)
  uint16_t thr = 0x00;
  v792adc->setChannelThreshold(thr);
  cout << "v792ac addr 0x" << hex << bid << dec << ": now ped  I1 value is " << ped << " thr " << thr << endl;
  
  v792adc->enableChannels();

  uint16_t reg1 = v792adc->getStatus1();
  uint16_t reg2 = v792adc->getControl1();
  uint16_t reg3 = v792adc->getStatus2();
  uint16_t reg4 = v792adc->getBit1();
  uint16_t reg5 = v792adc->getBit2();
  uint32_t evc = v792adc->eventCounter();
  cout << "v792ac addr 0x" << hex << bid << " status 1 0x" << reg1
       << " control 1 0x" << reg2 << " status 2 0x" << reg3 << " bit 1 0x" << reg4
       << " bit 2 0x" << reg5 << dec << endl;
  cout << "v792ac event counter " << evc << endl;
 }

void initV775N( v775n& xtdc )
 {
  xtdc.swReset();
  xtdc.setCommon ( v775n::commonStart );
  xtdc.zeroSuppression(false);
  xtdc.overSuppression(false);
  xtdc.setLSB(V775_140ps);   // set the LSB (ps)  full scale = 4096*LSB ==> 100 ps * 4096 ~ 410 ns
  xtdc.storeEmpty();
  xtdc.evCntReset();
  xtdc.dataReset();
  xtdc.printRegisters();
  cout << "v775 mode " << xtdc.getMode() << endl;
  cout << "v775 fsr (ns) " << 4096*8.9/xtdc.fullScale() << endl;
  cout << "v775 LSB (ns) " << 8.9/xtdc.fullScale() << endl;
 }

void initV775( v775& xtdc )
 {
  xtdc.swReset();
  xtdc.setCommon ( v775::commonStart );
  xtdc.zeroSuppression(false);
  xtdc.overSuppression(false);
  xtdc.setLSB(V775_35ps);   // set the LSB (ps)  full scale = 4096*LSB ==> 100 ps * 4096 ~ 410 ns
  xtdc.storeEmpty();
  xtdc.evCntReset();
  xtdc.dataReset();
  xtdc.printRegisters();
  cout << "v775 mode " << xtdc.getMode() << endl;
  cout << "v775 fsr (ns) " << 4096*8.9/xtdc.fullScale() << endl;
  cout << "v775 LSB (ns) " << 8.9/xtdc.fullScale() << endl;
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

uint32_t resetHardware ( const uint32_t nQDCs, v792ac* qdcPtrs[], v775n& xtdc0 )
 {
  for (uint32_t j=0; j<nQDCs; j++)
   {
    initV792 ( qdcPtrs[j] );
    sched_yield();
   }

  initV775N ( xtdc0 );
  sched_yield();

  return 0;
 }

uint32_t alignEventCounter ( uint32_t evtnumber, const uint32_t nQDCs, v792ac* qdcPtrs[], v775n& xtdc0 )
 {
  for (uint32_t j=0; j<nQDCs; j++)
   {
    qdcPtrs[j]->eventCounterOffset ( evtnumber );
   }
  xtdc0.eventCounterOffset ( evtnumber );

  return 0;
 }

uint64_t xreadtime(0);

uint32_t readEvent ( const uint32_t nQDCs, v792ac* qdcPtrs[], v775n& xtdc0, uint32_t ntriggers, uint32_t* buffer, uint32_t* qdcsize, uint32_t* tdcsize )
 {
  struct timeval startread;
  gettimeofday ( &startread, NULL );
  uint32_t nevqdc[nQDCs];
  uint32_t nevtdc0;
  // cerr << " evt " << ntriggers << " event counters ";
  for (uint32_t j=0; j<nQDCs; j++)
   {
    int32_t dr;
    do
     dr = qdcPtrs[j]->dready(1);
    while (!dr);
    nevqdc[j] = qdcPtrs[j]->eventCounter();
    // cerr << " " << j << " " << nevqdc[j];
   }
  nevtdc0 = xtdc0.eventCounter();
  // cerr << " TDC " << nevtdc0;
  
  uint32_t qdcsz[nQDCs];
  *qdcsize = 0;
  for (uint32_t j=0; j<nQDCs; j++)
   {
    qdcPtrs[j]->setEvents( ntriggers );
    qdcsz[j] = qdcPtrs[j]->readEvent( buffer );
    buffer += qdcsz[j];
    *qdcsize += qdcsz[j];
   }
  
  uint32_t sizetdc = xtdc0.readSingleEvent( buffer );
  buffer += sizetdc;
  *tdcsize = sizetdc;
  uint32_t errcode = 0;
  for (uint32_t j=0; j<nQDCs; j++)
   {
    errcode |= (ntriggers != nevqdc[0]) << j;
   }
  errcode |= (ntriggers != nevtdc0) << nQDCs;
  if ((errcode == 31) && (nevqdc[0] == ntriggers+1)) errcode = 0xff;
  if (errcode)
   {
    time_t rtime = time(NULL);
    struct tm myrtime;
    localtime_r( &rtime, &myrtime );
    cerr << " time is " << dec << rtime << " " << myrtime.tm_year+1900 << "." << myrtime.tm_mon+1 << "." << myrtime.tm_mday << "-" << myrtime.tm_hour << ":" << myrtime.tm_min << ":" << myrtime.tm_sec << endl; 
    cerr << dec << ntriggers;
    for (uint32_t j=0; j<nQDCs; j++)
     {
      cerr << " nq[" << j << "] " << nevqdc[j];
     }
    cerr << " nt0 " << nevtdc0 << " - error code " << hex << errcode << endl;
   }
  uint32_t productsiz = qdcsz[0];
  for (uint32_t j=1; j<nQDCs; j++)
   {
    productsiz *= qdcsz[j];
   }
   
  if (productsiz == 0)
   {
    errcode |= 1 << 7;
    struct timeval tod;
    gettimeofday ( &tod, NULL );
    struct tm * timeinfo = localtime ( &tod.tv_sec );
    double dsecs = timeinfo->tm_sec + tod.tv_usec*1E-6;
    cerr << " time is " << dec << timeinfo->tm_yday << ":" << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << dsecs << " ev # " << ntriggers;
    for (uint32_t j=0; j<nQDCs; j++)
     {
      cerr << " size" << j << qdcsz[j];
     }
    cerr << " sizetdc " << sizetdc << " - error code " << hex << errcode << endl;
   }
  struct timeval endread;
  gettimeofday ( &endread, NULL );
  xreadtime += (endread.tv_sec-startread.tv_sec)*ONEM
		+ (endread.tv_usec-startread.tv_usec);
  return errcode;
 }

void print_event ( ofstream& ofs, uint32_t trignum, struct timeval tod,  uint32_t spill_nr, uint32_t* Tcts, uint32_t tmask, uint32_t* myBuffer, uint32_t qdcsize, uint32_t tdcsize )
 {
  struct tm * timeinfo = localtime ( &tod.tv_sec );
  ofs << dec << " ev # " << trignum << " time " << timeinfo->tm_yday
     << "-" << timeinfo->tm_hour << "-" << timeinfo->tm_min << "-"
     << timeinfo->tm_sec << "-" << tod.tv_usec << " spill " << spill_nr
     << " tow cts " << Tcts[0] << " " << Tcts[1] << " " << Tcts[2] << " trigger mask " << hex << tmask << " values:";
  for (uint32_t kqdc=0; kqdc<nQDCs; kqdc++)
   {
    uint32_t offs = kqdc*QDCevsize;                     // QDCevsize = 34
    uint32_t ch_offs = kqdc*QDCchans;                   // QDCchans = 32
    for (uint32_t j=offs+1; j<offs+QDCevsize-1; j++)    // j=k*34+1; j<k*34+33
     {
      uint32_t chan = (myBuffer[j] >> 16) & 0x1f;
      chan += ch_offs;
      ofs << " " << dec << chan << " " << hex << (myBuffer[j] & 0x3fff);
     }
   }
  ofs << " TDC size " << dec << tdcsize << " val.s ";
  uint32_t* tdcbuffer = &myBuffer[qdcsize];
  if (tdcsize != 0xffffffff) for (uint32_t j=0; j<tdcsize; j++)
   {
    uint32_t _v = tdcbuffer[j];
    uint32_t chan = dataDecodeNChannel(_v);
    uint32_t mk = dataDecodeNFlags(_v);
    uint32_t val = dataDecodeNValue(_v);
    ofs << " " << chan << " " << mk << " " << val;
   }
  ofs << endl;
  return;
 }

void print_fake_event ( ofstream& ofs, uint32_t trignum, struct timeval tod,  uint32_t spill_nr, uint32_t* Tcts, uint32_t* myBuffer )
 {
  struct tm * timeinfo = localtime ( &tod.tv_sec );
  ofs << dec << " ev # " << trignum << " time " << timeinfo->tm_yday
     << "-" << timeinfo->tm_hour << "-" << timeinfo->tm_min << "-"
     << timeinfo->tm_sec << "-" << tod.tv_usec << " spill " << spill_nr
     << " tow cts " << Tcts[0] << " " << Tcts[1] << " " << Tcts[2] << " trigger mask " << hex << 0xffffffff << " values:";
  for (uint32_t kqdc=0; kqdc<nQDCs; kqdc++)
   {
    uint32_t offs = kqdc*QDCevsize;
    uint32_t ch_offs = kqdc*QDCchans;
    for (uint32_t j=offs+1; j<offs+QDCevsize-1; j++)
     {
      uint32_t chan = (myBuffer[j] >> 16) & 0x1f;
      chan += ch_offs;
      ofs << " " << dec << chan << " " << 0;
     }
   }
  ofs << " TDC size " << dec << 0 << " val.s ";
  ofs << endl;
  return;
 }


int main( int argc, char** argv )
{
  ofstream ofs;
  ifstream ifs;

  uint32_t trignum(0);
  uint32_t numevofs(0);
  uint32_t Tcts[4] = { 0, 0, 0, 0 };
  uint32_t myBuffer[4096];

  signal(SIGINT, cntrl_c_handler);      // Control-C handler
  signal(SIGUSR1, sigusr1_handler);     // Control-USR1 handler

  // v2718 v2718_0(0,"/V2718/cvA24_U_DATA/0");            // VME interface
  v513 ion(0xa00000,"/V2718/cvA24_U_DATA/0");          // I/O register
  myusleep ( 100000 );

  ion.print();
  myusleep ( 100000 );

  // Init I/O Register
  initV513 ( ion );
  sched_yield();

  for (uint32_t j=0; j<nQDCs; j++) qdcPtrs[j] = new v792ac(qdcAds[j], "/V2718/cvA32_U_DATA/0");

  cout << " nQDCs " << nQDCs << " ads ";
  for (uint32_t j=0; j<nQDCs; j++) cout << hex << " " << j << " " << qdcAds[j] << " pointer " << qdcPtrs[j];
  cout << endl;
  
  v775n xtdc0(0x08000000,"/V2718/cvA32_U_DATA/0");      // V775N TDC

  // Init QDCs and TDCs
  resetHardware ( nQDCs, qdcPtrs, xtdc0 );

  resetScaler( ion );

  unlockTrigger( ion );

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

  if (argc == 2) fname = argv[1];

  cout << " filename is " << fname << endl;

  ofs.open (fname.c_str(), std::ofstream::out);

  time_t tr0 = time(NULL);
  bool running(true);

  volatile bool exit_now(false);
  volatile bool on_error(false);

  uint32_t spill_nr(0);

  enableTriggers( ion );

  uint32_t prevTrig(0), prevCts[4] = {0};

  bool is_spill(false);

  while (1)
  {
    if (abort_run) exit_now = true;
    if (exit_now) disableTriggers( ion );
    if (running and pause_run)
    {
      disableTriggers(ion);
      running = false;
      time_t tr = time(NULL) - tr0;
      cout << dec << tr << "(sec) " << trignum << "(evt) ioreg " << ioregValue( ion ) 
           << " running was true (now false) and pause_run true" << endl;
    }
    else if (not running and not pause_run)
    {
      enableTriggers(ion);
      running = true;
      time_t tr = time(NULL) - tr0;
      cout << dec << tr << "(sec) " << trignum << "(evt) ioreg " << ioregValue( ion ) 
           << " running was false (now true) and pause_run false" << endl;
    }
    bool isPh = isTrig( ion );

    uint32_t tmask = getTrigMask( ion );
    bool spill_now = (tmask & 4);
    if (is_spill != spill_now)
     {
      is_spill = spill_now;
      if (is_spill)
       {
        spill_nr ++;
       }
      else
       {
        uint32_t triginspill, nowCts[4];
        triginspill = trignum - prevTrig;
        nowCts[0] = Tcts[0]-prevCts[0];
        nowCts[1] = Tcts[1]-prevCts[1];
        nowCts[2] = Tcts[2]-prevCts[2];
        nowCts[3] = Tcts[3]-prevCts[3];
        time_t tr = time(NULL) - tr0;
        cout << " END of spill: " << dec << spill_nr << " at " << tr << "(sec) " << triginspill << "(evt) "
	       << "beam/pede/inSpill/beamInSpill: "
	       << nowCts[0] << "/" << nowCts[1] << "/" << nowCts[2] << "/" << nowCts[3] << endl;
        prevTrig = trignum;
        prevCts[0] = Tcts[0];
        prevCts[1] = Tcts[1];
        prevCts[2] = Tcts[2];
        prevCts[3] = Tcts[3];
       }
     }
    if (isPh) do 
    {
      if ((trignum % 10000) == 0)
      {
        time_t tr = time(NULL) - tr0;
        double rate = trignum/double(tr);
        double daqtime = xreadtime/double(trignum);
        cout << dec << tr << "(sec) " << trignum << "(evt) "
	     << "beam/pede/inSpill/beamInSpill: "
	     << Tcts[0] << "/" << Tcts[1] << "/" << Tcts[2] << "/" << Tcts[3]
	     << " daqtime " << daqtime << " "
	     << rate << "(evt/sec) last isPh " << isPh << " tmask " << hex << tmask
	     << " ioreg " << ioregValue( ion ) << endl;
      }
      if (tmask & 1) Tcts[0] ++;            // physics triggers
      if (tmask & 2) Tcts[1] ++;            // pedestal triggers
      if (tmask & 4) Tcts[2] ++;            // in spill
      if ((tmask & 5) == 5) Tcts[3] ++;    // physics in spill
      (Tcts[1]*10 < Tcts[0]) ? enablePedestals( ion ) : disablePedestals( ion );
      uint32_t qdcsize, tdcsize;
      uint32_t retcode = readEvent ( nQDCs, qdcPtrs, xtdc0, trignum, myBuffer, &qdcsize, &tdcsize );
      uint32_t head = myBuffer[0];
      if (qdcsize == 0) break;
      uint32_t trail = myBuffer[qdcsize-1] & 0xff000000;
      uint32_t evnum = numevofs + (myBuffer[qdcsize-1] & 0xffffff);
      if (head != 0xfa002000)
      {
        cerr << dec << trignum << " Error in header " << hex << head << endl;
        tmask |= 0xffff0000;
        on_error = true;
        disableTriggers( ion );
      }
      if (trail != 0xfc000000)
      {
        cerr << dec << trignum << " Error in trailer " << hex << trail << endl;
        tmask |= 0xffff0000;
        on_error = true;
        disableTriggers( ion );
      }
      if (qdcsize != QDCtotsize)
      {
	cerr << dec << trignum << " Size error " << qdcsize << endl;
        tmask |= 0xffff0000;
        on_error = true;
        disableTriggers( ion );
      }
      if (evnum != trignum)
      {
        cerr << dec << trignum << " Error in V792 event number " << evnum << endl;
        tmask |= 0xffff0000;
        on_error = true;
        disableTriggers( ion );
      }
      if (retcode)
      {
        cerr << dec << trignum << " Error return code " << retcode << endl;
        tmask |= 0xffff0000;
        on_error = true;
        disableTriggers( ion );
      }
      
      struct timeval tod;
      gettimeofday ( &tod, NULL );
      print_event ( ofs, trignum, tod, spill_nr, Tcts, tmask, myBuffer, qdcsize, tdcsize );
      // while (trignum <= evnum) trignum ++; // evt_realign ( ion, qdc, Tcts, ofs, trignum );
      trignum ++;

      if (retcode == 0xff)     // spurious trigger to all elx -- add one fake event to align VME data with FERS data
      {
        print_fake_event ( ofs, trignum, tod, spill_nr, Tcts, myBuffer );
        trignum ++;
      }

      if (on_error)
      {
        // Re-init QDCs and TDCs
        cerr << dec << trignum << " ********* resetting DAQ ********* " << endl;

	struct timeval tod;
        gettimeofday ( &tod, NULL );
        struct tm * timeinfo = localtime ( &tod.tv_sec );
	double dsecs = timeinfo->tm_sec + tod.tv_usec*1E-6;
        cerr << " time is " << timeinfo->tm_yday << ":" << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << dsecs << endl;

        tmask |= 0xffff0000;
        resetHardware ( nQDCs, qdcPtrs, xtdc0 );
        myusleep ( 100000 );
        alignEventCounter ( trignum, nQDCs, qdcPtrs, xtdc0 );
        numevofs = trignum;
        myusleep ( 100000 );
        cerr << dec << trignum << " ********* reenabling triggers ********* " << endl;
        enableTriggers( ion );
        myusleep ( 100000 );
        on_error = false;
      }

      if (!exit_now) unlockTrigger( ion );
    } while(0);
    if (exit_now) break;
    nsleep( NSLEEPT );
  }
  disableTriggers( ion );
  ofs.close();

  for (uint32_t j=0; j<nQDCs; j++) delete qdcPtrs[j];

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