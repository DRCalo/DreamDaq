
#include <iostream>

#include "daqFifo.h"

int main ( int argc, char** argv )
 {
  daqFifo* Fifo;

  do
   {
    try
     {
      Fifo = new daqFifo ( 0x10, fModeSlave );
     }
    catch (...)
     {
      Fifo = NULL;
     }
   }
  while ( Fifo == NULL);

  char msg[1024];

  int re = Fifo->fromFifo( msg, 1024 );

  std::cout << " re is " << re << std::endl;
  std::cout << " msg is " << msg << std::endl;

  delete Fifo;

  return 0;
 }
