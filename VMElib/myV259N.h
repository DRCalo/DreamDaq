#ifndef _MY_V259N_H_
#define _MY_V259N_H_

/*****************************************

  myV488.h
  ---------

  Definition of the CAEN I/O REG V259N class.
  This class is a class derived from the class vme.


*****************************************/

#include <iostream>

#include "myVme.h"

#define V259N_VER (0xFE)
#define V259N_MAN (0xFC)
#define V259N_FIX (0xFA)

#define V259N_MULT_REG (0x28)
#define V259N_PATT_REG (0x26)
#define V259N_MULTI_REG_READ_RESET (0x24)
#define V259N_PATT_REG_READ_RESET (0x22)
#define V259N_RESET_ALL (0x20)

/*****************************************/
// The v259n class 
/*****************************************/
class v259n : public vme {
  
 public:
  
  // Constructor
  v259n(uint32_t base, const char* dev);
  
  inline uint16_t readMultiplicityRegister();

  inline uint16_t readPatternRegister();

  inline uint16_t readResetMultiplicityRegister();

  inline uint16_t readResetPatternRegister();

  inline void resetAll();

  inline uint16_t getVersion();

  inline uint16_t getManCode();

  inline uint16_t getFixed();
};

  inline uint16_t v259n::readMultiplicityRegister()
   { uint16_t val; read16phys(V259N_MULT_REG,&val); return val; }

  inline uint16_t v259n::readPatternRegister()
   { uint16_t val; read16phys(V259N_PATT_REG,&val); return val; }

  inline uint16_t v259n::readResetMultiplicityRegister()
   { uint16_t val; read16phys(V259N_MULTI_REG_READ_RESET,&val); return val; }

  inline uint16_t v259n::readResetPatternRegister()
   { uint16_t val; read16phys(V259N_PATT_REG_READ_RESET,&val); return val; }

  inline void v259n::resetAll()
   { write16phys(V259N_RESET_ALL,0); }

  inline uint16_t v259n::getVersion()
   { uint16_t val; read16phys(V259N_VER,&val); return val; }

  inline uint16_t v259n::getManCode()
   { uint16_t val; read16phys(V259N_MAN,&val); return val; }

  inline uint16_t v259n::getFixed()
   { uint16_t val; read16phys(V259N_FIX,&val); return val; }

#endif // _MY_V259N_H_
