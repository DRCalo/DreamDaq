#include "monaisa_threads.h"

extern "C" void* threadStart ( void* args )
 {
  monaisa_thread* ithr = (monaisa_thread*)args;
  return ithr->f_thread();
 }

void* monaisa_thread::f_thread()
 {
  while (1)
   {
    sem_wait(&m_read);
    try { m_obj->getServer(m_name); } catch (...) { }
    sem_post(&m_done);
   }
  return NULL;
 }

monaisa_thread::monaisa_thread ( std::string & server, is_descr* ptr ) throw (int)
  : m_name(server), m_obj(ptr)
 {
  sem_init(&m_read, 0, 0);
  sem_init(&m_done, 0, 0);
  pthread_attr_t attr;
  pthread_attr_init ( &attr );
  pthread_attr_setdetachstate ( &attr, PTHREAD_CREATE_JOINABLE );

  int status = pthread_create ( &m_thread, &attr, threadStart, (void*)this );
  if ( status < 0 ) throw status;
 }

monaisa_thread::~monaisa_thread ()
 {
  pthread_cancel ( m_thread );
  pthread_join ( m_thread, NULL );
  sem_destroy(&m_done);
  sem_destroy(&m_read);
 }

int monaisa_thread::get_next()
 {
  sem_post(&m_read);
  return 0;
 }

bool monaisa_thread::wait_done()
 {
  sem_wait(&m_done);
  return 0;
 }
