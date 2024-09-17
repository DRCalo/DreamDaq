#ifndef _DAQ_FIFO_H_
#define _DAQ_FIFO_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define FIFO_BASE "/tmp/daqfifo"

#define fModeMaster 0
#define fModeSlave 1

#include <iostream>

class daqFifo
 {
  public:
	daqFifo( int nr, int mode );
        ~daqFifo();
	int toFifo( const char* msg ) { return write ( m_fd, msg, strlen(msg)+1 ); }
	int fromFifo( char* msg, size_t len ) { return read ( m_fd, msg, len ); }
  private:
	int m_mode;
	std::string m_name;
	int m_fd;

 };

daqFifo::daqFifo ( int nr, int mode )
 : m_mode ( mode )
 {
  size_t length = strlen( FIFO_BASE ) + snprintf( NULL, 0, "%x", nr ) + 1;

  char myfifo [ length ];

  snprintf( myfifo, length, "%s%x", FIFO_BASE, nr );

  std::cout << " myfifo is " << myfifo << " len " << length << std::endl;

  int re = mkfifo( myfifo, 0666 );
  if ((re == -1) && (errno != EEXIST)) throw -1;

  m_name = std::string( myfifo );

  if (mode == fModeMaster)
   {
    m_fd = open( myfifo, O_WRONLY );
    if ( m_fd == -1 ) throw -2;
   }
  else if (mode == fModeSlave)
   {
    m_fd = open( myfifo, O_RDONLY );
    if ( m_fd == -1 ) throw -3;
   }
  else
   {
    throw -4;
   }
 }

daqFifo::~daqFifo()
 {
  close( m_fd );
 }

#endif // _DAQ_FIFO_H_
