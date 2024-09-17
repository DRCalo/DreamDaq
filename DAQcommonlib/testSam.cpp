#include <stdint.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <iostream>

#define BASE_KEY (0x2000)

int main ( int argc, char** argv )
 {
  int32_t semid;
  do
   {
    usleep(1000);
    semid = semget(BASE_KEY,1,0666);     // get semaphore
   }
  while (semid == -1);

  struct sembuf sops;

  sops.sem_num = 0;
  sops.sem_op = 1;
  sops.sem_flg = 0;

  int rz = semop ( semid, &sops, 1 );     // increase semaphore

  int dt = semctl ( semid, 0, GETVAL );

  // semctl(semid, 0, IPC_RMID);

  std::cout << " val is " << dt << std::endl;

  return 0;
 }
