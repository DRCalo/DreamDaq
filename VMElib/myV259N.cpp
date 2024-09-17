#include <iostream>

#include "myV259N.h"
#include "myModules.h"

using namespace std;

/*****************************************/
// Constructor
/*****************************************/

v259n::v259n(uint32_t base, const char* dev):vme(base, 0x100, dev)
 {
  m_id = base | ID_V259N;
  m_name = "CAEN V259N I/O Register";
  uint16_t ver = getVersion();
  uint16_t cod = getManCode();
  uint16_t fix = getFixed();
  cout << m_name << " id 0x" << hex << m_id << " ver 0x" << ver
       << " cod 0x" << cod << " fix 0x" << fix << dec << endl;
  if (0) if ((cod != 0x801) || (fix != 0xfaf5))
   {
    cerr << "Wrong address or module type\n";
    exit(1);
   }
  resetAll();
 }

