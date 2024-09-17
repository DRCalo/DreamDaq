
#include <iostream>

#include "daqShm.h"

int main ( int argc, char** argv )
 {
  daqShm* mShm;

  do
   {
    try
     {
      mShm = new daqShm ( 0x10, mModeSlave );
     }
    catch (...)
     {
      mShm = NULL;
     }
   }
  while ( mShm == NULL);

  volatile uint32_t* data = mShm->shmPtr();
  volatile uint32_t* fromDaq = mShm->fromDaq();
  volatile uint32_t* toDaq = mShm->toDaq();
  volatile uint32_t* eventNr = mShm->eventNr();
  volatile uint32_t* spillNr = mShm->spillNr();
  volatile uint32_t* triggerMask = mShm->triggerMask();

  std::cout << std::hex << " data[0] " << data[0]
            << " data[1] " << data[1]
            << " data[2] " << data[2]
            << " data[3] " << data[3]
            << " data[4] " << data[4] << std::endl;
 
  std::cout << std::hex << " fromDaq " << *fromDaq
            << " toDaq " << *toDaq
            << " eventNr " << *eventNr
            << " spillNr " << *spillNr
            << " triggerMask " << *triggerMask << std::endl;
 
  delete mShm;

  return 0;
 }
