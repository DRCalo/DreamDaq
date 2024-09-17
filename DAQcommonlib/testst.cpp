#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#define FIFO_BASE "/tmp/daqfifo"

int main ()
 {
  int nr = 0x100;

  size_t length = strlen( FIFO_BASE ) + snprintf( NULL, 0, "%x", nr );

  char str[length+1];

  std::cout << " strlen( FIFO_BASE ) " << strlen( FIFO_BASE )
            << " snprintf( NULL, 0, \"%x\", nr ) " << snprintf( NULL, 0, "%x", nr )
            << " length " << length << std::endl;

  snprintf( str, length + 1, "%s%x", FIFO_BASE, nr );
  std::cout << " strlen( str ) " << strlen( str )
            << " str is " << str << std::endl;

  std::string name = std::string ( str );

  std::cout << " name.length() " << name.length()
            << " name " << name << std::endl;
  return 0;
 }
