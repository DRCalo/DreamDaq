#ifndef _MONAISA_THREADS_H_
#define _MONAISA_THREADS_H_

#include <string>
#include <semaphore.h>
#include <pthread.h>

#define _THIS_IS_MONAISA_CORE_
#include "MonaIsa/is_descr.h"

extern "C" void* threadStart ( void* args );

class monaisa_thread
 {
  public:
   monaisa_thread ( std::string & server, is_descr* ptr ) throw (int);
   ~monaisa_thread();
   int get_next();
   bool wait_done();
  private: 
   std::string m_name;
   is_descr * m_obj;
   sem_t m_read, m_done;
   pthread_t m_thread;
   void* f_thread();
   friend void* threadStart ( void* args );

 };

#endif // _MONAISA_THREADS_H_
