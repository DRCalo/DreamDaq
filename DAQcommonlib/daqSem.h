#ifndef _DAQ_SEM_H_
#define _DAQ_SEM_H_

#include <stdint.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define BASE_KEY (0x1000)

#define sModeMaster 0
#define sModeSlave 1

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
} semun;

class daqSem
 {
  public:
	daqSem( int nr, int mode );
        ~daqSem();
	int semGet() { return semctl ( m_semid, 0, GETVAL ); }
	int semInc() { return semop ( m_semid, &m_seminc, 1 ); }
	int semDec() { return semop ( m_semid, &m_semdec, 1 ); }
	int semSet( int val )
	 {
	  if (val >= 0x100) return -10;        // max value is 255
	  union semun sm;
	  sm.val = val;
	  return semctl( m_semid, 0, SETVAL, sm );
	 }
  private:
	int m_mode;
	int32_t m_semid;
	struct sembuf m_seminc;
	struct sembuf m_semdec;
 };

daqSem::daqSem ( int nr, int mode )
 : m_mode ( mode )
 {
  if (mode == sModeMaster)
   {
    m_semid = semget( BASE_KEY+nr, 1, IPC_CREAT|0666 );   // create semaphore
    if (m_semid == -1) throw -1;
    union semun sm;
    sm.val = 0;
    int re = semctl( m_semid, 0, SETVAL, sm );    // initialise semaphore to zero
    if (re == -1) throw -2;
   }
  else if (mode == sModeSlave)
   {
    m_semid = semget( BASE_KEY+nr, 1, 0666);             // get semaphore
    if (m_semid == -1) throw -3;
   }
  else
   {
    throw -4;
   }

  m_seminc.sem_num = 0;
  m_seminc.sem_op = 1;
  m_seminc.sem_flg = 0;

  m_semdec.sem_num = 0;
  m_semdec.sem_op = -1;
  m_semdec.sem_flg = 0;

 }

daqSem::~daqSem()
 {
  if (m_mode == sModeMaster)
   {
    semctl( m_semid, 0, IPC_RMID );                  // remove semaphore
   }
 }

#endif // _DAQ_SEM_H_
