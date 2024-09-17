
#include <iostream>

#include "myV792AC.h"

using namespace std;

uint32_t addresses[4]={0x04000000,0x05000000,0x06000000,0x03000000};

int32_t main (int32_t argc, char** argv)
 {
  uint32_t addr;
  if (argc==1)
    addr=0x05000000;
  else
   {
    int32_t i = atoi(argv[1]);
    addr = (i<4) ? addresses[i] : 0x05000000;
   }
    
  cout << "instantiating V792ac " << hex << addr << dec << endl;
  v792ac adcX(addr, "/V2718/cvA32_U_DATA/0");
  int32_t busy = adcX.busy(); 
  cout << "busy " << busy;
  int32_t dready = adcX.dready(); 
  cout << "\ndready " << dready;
  int32_t evBufferEmpty = adcX.eventBufferEmpty();
  cout << "\nbufferEmpty " << evBufferEmpty;
  uint32_t evCounter = adcX.eventCounter();
  cout << "\nevent counter " << evCounter << endl;

  const uint32_t QDCchans = 32;
  const uint32_t QDCevsize = QDCchans+2;

  if (!evBufferEmpty)
   {
    while (adcX.dready())
     {
      uint32_t buffer[256];
      uint32_t size = adcX.readEvent(buffer);
      cout << " Event size " << size << endl << hex;
      for (uint32_t k=0; k<size; k++) cout << buffer[k] << " ";
      cout << endl << dec;
     }
   }

  cout << " ************ NOW ********************" << endl;
  busy = adcX.busy(); 
  cout << "busy " << busy;
  dready = adcX.dready(); 
  cout << "\ndready " << dready;
  evBufferEmpty = adcX.eventBufferEmpty();
  cout << "\nbufferEmpty " << evBufferEmpty;
  evCounter = adcX.eventCounter();
  cout << "\nevent counter " << evCounter << endl;

  return 0;
 }
