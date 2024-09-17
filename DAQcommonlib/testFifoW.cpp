
#include <iostream>

#include "daqFifo.h"

int main ( int argc, char** argv )
 {
  daqFifo Fifo ( 0x10, fModeMaster );

  std::string sst( "test" );

  std::cout << " msg is " << sst << std::endl;

  int re = Fifo.toFifo( sst.c_str() );

  std::cout << " re is " << re << std::endl;

  sleep(5);

  return 0;
 }
 
