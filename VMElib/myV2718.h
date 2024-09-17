#ifndef _MY_V2718_H_
#define _MY_V2718_H_

#include <stdint.h>

#ifndef LINUX
#define LINUX
#endif

#include "myVme.h"

/*****************************************/
// The v2718 class
/*****************************************/
class v2718 : public vme {

 public:

  v2718(const char* dev);
  ~v2718() {}

  uint32_t setPulserA( uint64_t period,  uint64_t width);
  uint32_t setPulserB( uint64_t period,  uint64_t width);
  uint32_t resetPulserA();
  uint32_t resetPulserB();

  uint32_t readInputRegister( uint32_t* w );
  uint32_t writeOutputRegister( uint32_t w );
  uint32_t setOutputRegister( uint32_t w );
  uint32_t clearOutputRegister( uint32_t w );

  uint32_t clearOutputBit( uint32_t bit );
  uint32_t setOutputBit( uint32_t bit );

  uint32_t setOutputManual( CVOutputSelect output );
  uint32_t setInput( CVInputSelect input );

 protected:

 private:
};

#endif // _MY_V2718_H_
