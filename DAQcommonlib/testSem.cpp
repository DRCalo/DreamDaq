#include <stdint.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <iostream>

#define BASE_KEY (0x2000)

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
} arg;

int main ( int argc, char** argv )
 {
  int32_t semid = semget(BASE_KEY,1,IPC_CREAT|0666);   // create semaphore
  union semun sm;
  sm.val = 0;
  int re = semctl ( semid, 0, SETVAL, sm );            // initialise semaphore to zero

  sleep(1);

  int dt = semctl ( semid, 0, GETVAL );                // check semaphore value

  semctl(semid, 0, IPC_RMID);                          // remove semaphore

  std::cout << " val is " << dt << std::endl;

  return 0;
 }
