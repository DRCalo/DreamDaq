#ifndef _DAQ_SHM_H_
#define _DAQ_SHM_H_

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <cerrno>
#include <cstring>

#define SHM_KEY 0x4000

#define mModeMaster 0
#define mModeSlave 1

#include <iostream>

class daqShm
 {
  public:
	daqShm( int nr, int mode );
        ~daqShm();
	uint32_t* shmPtr() { return m_addr; }
	uint32_t* daqReadyToRun() { return m_addr; }
	uint32_t* askDaqToRun() { return m_addr+1; }
	uint32_t* fromDaq() { return m_addr+2; }
	uint32_t* toDaq() { return m_addr+3; }
	uint32_t* eventNr() { return m_addr+4; }
	uint32_t* spillNr() { return m_addr+5; }
	uint32_t* triggerMask() { return m_addr+6; }

  private:
	int m_mode;
	int32_t m_shmid;
	uint32_t* m_addr;

 };

daqShm::daqShm ( int nr, int mode ) : m_mode ( mode )
 {
  if ((mode != mModeMaster) && (mode != mModeSlave)) throw -1;

  int shmflg = (mode == mModeMaster) ? (IPC_CREAT | 0666) : 0666;

  m_shmid = shmget( SHM_KEY+nr, 8*sizeof(uint32_t), shmflg);
  
  if (m_shmid < 0)
   {
    // std::cout << " shmget failed: " << std::strerror(errno) << std::endl;
    throw -2;
   }

  m_addr = (uint32_t*) shmat( m_shmid, NULL, 0 );

  if ((int64_t) m_addr == -1)
   {
    // std::cout << " shmat failed: " << std::strerror(errno) << std::endl;
    throw -3;
   }
 }

daqShm::~daqShm()
 {
  shmdt( (void*) m_addr );

  if (m_mode == mModeMaster)
   {
    int re = shmctl( m_shmid , IPC_RMID , 0 );
    if (re  == -1)
     {
      std::cout << " shmctl failed: " << std::strerror(errno) << std::endl;
     }
   }
 }

#endif // _DAQ_SHM_H_
