#include <stdexcept>
#include <sstream>
#include <string>
#include <execinfo.h>
#include <cstdio>
#include <ctime>

#define _THIS_IS_MONAISA_CORE_
#include "MonaIsa/is_descr.h"
#include "monaisa_threads.h"

std::string time_print()
 {
  time_t now = time(NULL);
  char cbf[26];
  ctime_r(&now,cbf);
  for (uint i=0; i<26; i++) if (cbf[i]==10) cbf[i]=0;
  return std::string(cbf);
 }

#define PRINT_BTRACE 1

#define BTRACE(A) if (PRINT_BTRACE) do \
 { std::ostringstream strstr; strstr << A; bt(strstr.str()); } while(0);

void bt( std::string msg )
 {
  void * array[64];
  int32_t nf = backtrace(array, 64);
  char ** symbols = backtrace_symbols(array, nf);
  std::cerr << msg << "\n";
  std::cerr << time_print() << " " << nf << " stack frames:\n";
 
  if (symbols != NULL)
   {
    for (int32_t i = 0; i < nf; i++) std::cerr << symbols[i] << std::endl;
    free(symbols);
   }
 }

is_descr::is_descr( const stringList & snames, bool mode ) :
  m_mode(mode),
  Pname_(getenv("TDAQ_PARTITION")),
  Partition_(new IPCPartition(Pname_)),
  Snames_ (snames),
  Regexp_ (".*"),
  tRun (new timeRun(Pname_)),
  tSOR(0),
  tEOR(0),
  rSOR(0),
  rEOR(0),
  iskip(0),
  m_items(0),
  m_selected(0),
  m_nevt(0)
 {
  pthread_mutex_init ( &m_lockaccess, NULL );
  this->clear();
  monaisa_thread* thr;
  for (uint32_t i=0; i<Snames_.size(); i++) try
   {
    thr = new monaisa_thread(Snames_[i], this);
    m_threads.push_back(thr);
   }
  catch (...) { }
  std::cout << "*** " << MONAISA_BUILT_INFO << " ***\n"
            << "*** " << m_threads.size() << " threads running in "
            << (m_mode == MonaIsa::SERIAL ? "SERIAL" : "PARALLEL")
            << " mode ***\n";
 }

is_descr::~is_descr()
 {
  this->clear();
  for (uint32_t i=0; i<m_threads.size(); i++) delete m_threads[i];
  m_threads.clear();
  pthread_mutex_destroy ( &m_lockaccess );
  Pname_.clear();
  Regexp_.clear();
  Snames_.clear();
  delete Partition_;
  delete tRun;
 }

std::string is_descr::sinfo ( double * p)
 {
  std::string erInfo = "may be you are confused ...";
  uint32_t idx = urint (*p);
  return ((idx < pstring.size()) && (p == pstring[idx])) ?
   vstring[idx] : erInfo;
 }

void is_descr::vgetv ( double * p, std::vector<uint32_t> & uvv)
 {
  uvv.clear();
  uint32_t idx = urint (*p);
  if ((idx < pu32vect.size()) && (p == pu32vect[idx]))
   {
    std::istringstream strstr(vu32vect[idx]);
    while (!strstr.eof())
     {
      uint32_t v;
      strstr >> v;
      uvv.push_back(v);
     }
   }
  return;
 }

void is_descr::vgetv ( double * p, std::vector<int32_t> & lvv)
 {
  lvv.clear();
  uint32_t idx = urint (*p);
  if ((idx < pu32vect.size()) && (p == pu32vect[idx]))
   {
    std::istringstream strstr(vu32vect[idx]);
    while (!strstr.eof())
     {
      int32_t v;
      strstr >> v;
      lvv.push_back(v);
     }
   }
  return;
 }

void is_descr::vgetv ( double * p, std::vector<double> & dvv)
 {
  dvv.clear();
  uint32_t idx = urint (*p);
  if ((idx < pu32vect.size()) && (p == pu32vect[idx]))
   {
    std::istringstream strstr(vu32vect[idx]);
    while (!strstr.eof())
     {
      double v;
      strstr >> v;
      dvv.push_back(v);
     }
   }
  return;
 }

uint32_t is_descr::uvtot ( double * p)
 {
  uint32_t idx = urint (*p);
  uint32_t retv = 999999;
  if ((idx < pu32vect.size()) && (p == pu32vect[idx]))
   {
    std::istringstream strstr;
    strstr.str(vu32vect[idx]);
    retv = 0;
    while (!strstr.eof())
     {
      uint32_t v;
      strstr >> v;
      retv += v;
     }
   }
  return retv;
 }

int32_t is_descr::lvtot ( double * p)
 {
  uint32_t idx = urint (*p);
  int32_t retv = 999999;
  if ((idx < pu32vect.size()) && (p == pu32vect[idx]))
   {
    std::istringstream strstr;
    strstr.str(vu32vect[idx]);
    retv = 0;
    while (!strstr.eof())
     {
      int32_t v;
      strstr >> v;
      retv += v;
     }
   }
  return retv;
 }

void is_descr::vsumv ( uint32_t el, uint32_t item, std::vector<uint32_t> & uvv)
 {
  try
   {
    uvv.clear();
    for (uint32_t i=0; i<vsizes[el]; i++)
     {
      std::vector<uint32_t> x;
      vgetv ( &(*anyInfoP[el].at(i)).at(item), x );
      size_t N = (x.size() <= uvv.size()) ? x.size() : uvv.size();
      for (size_t j=0; j<N; j++) uvv[j]+=x[j];
      for (size_t j=uvv.size(); j<x.size(); j++) uvv.push_back(x[j]);
     }
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vsumv ( " << el << ", " << item << ", std::vector<uint32_t> & uvv) : " << vsizes[el] << " " << exc.what() );
    uvv.clear();
   }
  return;
 }

void is_descr::vsumv ( uint32_t el, uint32_t item, std::vector<int32_t> & lvv)
 {
  try
   {
    lvv.clear();
    for (uint32_t i=0; i<vsizes[el]; i++)
     {
      std::vector<int32_t> x;
      vgetv ( &(*anyInfoP[el].at(i)).at(item), x );
      size_t N = (x.size() <= lvv.size()) ? x.size() : lvv.size();
      for (size_t j=0; j<N; j++) lvv[j]+=x[j];
      for (size_t j=lvv.size(); j<x.size(); j++) lvv.push_back(x[j]);
     }
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vsumv ( " << el << ", " << item << ", std::vector<int32_t> & lvv) : " << vsizes[el] << " " << exc.what() );
    lvv.clear();
   }
  return;
 }

void is_descr::vsumv ( uint32_t el, uint32_t item, std::vector<double> & dvv)
 {
  try
   {
    dvv.clear();
    for (uint32_t i=0; i<vsizes[el]; i++)
     {
      std::vector<double> x;
      vgetv ( &(*anyInfoP[el].at(i)).at(item), x );
      size_t N = (x.size() <= dvv.size()) ? x.size() : dvv.size();
      for (size_t j=0; j<N; j++) dvv[j]+=x[j];
      for (size_t j=dvv.size(); j<x.size(); j++) dvv.push_back(x[j]);
     }
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vsumv ( " << el << ", " << item << ", std::vector<double> & dvv) : " << vsizes[el] << " " << exc.what() );
    dvv.clear();
   }
  return;
 }

uint32_t is_descr::howmany() { return m_nevt; }

uint32_t is_descr::usizes( uint32_t el) { return vsizes[el]; }

namespace { const std::string EmptyString(""); }

std::string is_descr::sname( uint32_t el, uint32_t cnt)
 {
  try {
    return anyInfoN[el].at(cnt);
  } catch (std::exception &exc) {
    BTRACE( "is_descr::sname ( " << el << ", " << cnt << ") : " << vsizes[el] << " " << exc.what() );
    return EmptyString;
  }
 }

double is_descr::dval( uint32_t el, uint32_t cnt, uint32_t item)
 {
  try {
    return (*anyInfoP[el].at(cnt)).at(item);
  } catch (std::exception &exc) {
    BTRACE( "is_descr::dval ( " << el << ", " << cnt << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint32_t is_descr::uval( uint32_t el, uint32_t cnt, uint32_t item)
 {
  try {
    return urint( (*anyInfoP[el].at(cnt)).at(item) );
  } catch (std::exception &exc) {
    BTRACE( "is_descr::uval ( " << el << ", " << cnt << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

int32_t is_descr::lval( uint32_t el, uint32_t cnt, uint32_t item)
 {
  try {
    return irint( (*anyInfoP[el].at(cnt)).at(item) );
  } catch (std::exception &exc) {
    BTRACE( "is_descr::lval ( " << el << ", " << cnt << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint64_t is_descr::ULval( uint32_t el, uint32_t cnt, uint32_t item)
 {
  try {
    return ULrint( (*anyInfoP[el].at(cnt)).at(item) );
  } catch (std::exception &exc) {
    BTRACE( "is_descr::ULval ( " << el << ", " << cnt << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

std::string is_descr::sval( uint32_t el, uint32_t cnt, uint32_t item)
 {
  try {
    return sinfo( &(*anyInfoP[el].at(cnt)).at(item) );
  } catch (std::exception &exc) {
    BTRACE( "is_descr::sval ( " << el << ", " << cnt << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
  return EmptyString;
  }
 }

void is_descr::vgetv( uint32_t el, uint32_t cnt, uint32_t item, std::vector<uint32_t> & uvv)
 {
  try {
    vgetv ( &(*anyInfoP[el].at(cnt)).at(item), uvv ); return;
  } catch (std::exception &exc) {
    BTRACE( "is_descr::vgetv ( " << el << ", " << cnt << ", " << item << ", std::vector<uint32_t> & uvv) : " << vsizes[el] << " " << exc.what() );
    uvv.clear();
  }
 }

void is_descr::vgetv( uint32_t el, uint32_t cnt, uint32_t item, std::vector<int32_t> & lvv)
 {
  try {
    vgetv ( &(*anyInfoP[el].at(cnt)).at(item), lvv ); return;
  } catch (std::exception &exc) {
    BTRACE( "is_descr::vgetv ( " << el << ", " << cnt << ", " << item << ", std::vector<int32_t> & lvv) : " << vsizes[el] << " " << exc.what() );
    lvv.clear();
  }
 }

void is_descr::vgetv( uint32_t el, uint32_t cnt, uint32_t item, std::vector<double> & dvv)
 {
  try {
    vgetv ( &(*anyInfoP[el].at(cnt)).at(item), dvv ); return;
  } catch (std::exception &exc) {
    BTRACE( "is_descr::vgetv ( " << el << ", " << cnt << ", " << item << ", std::vector<double> & dvv) : " << vsizes[el] << " " << exc.what() );
    dvv.clear();
  }
 }

uint32_t is_descr::uvtot( uint32_t el, uint32_t cnt, uint32_t item)
 {
  try {
    return uvtot ( &(*anyInfoP[el].at(cnt)).at(item) );
  } catch (std::exception &exc) {
    BTRACE( "is_descr::uvtot ( " << el << ", " << cnt << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

int32_t is_descr::lvtot( uint32_t el, uint32_t cnt, uint32_t item)
 {
  try {
    return lvtot ( &(*anyInfoP[el].at(cnt)).at(item) );
  } catch (std::exception &exc) {
    BTRACE( "is_descr::lvtot ( " << el << ", " << cnt << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

double is_descr::dval( uint32_t el, std::string iname, uint32_t item)
 {
  try {
    return anyInfo[el][iname].at(item);
  } catch (std::exception &exc) {
    BTRACE( "is_descr::dval ( " << el << ", " << iname << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint32_t is_descr::uval( uint32_t el, std::string iname, uint32_t item)
 {
  try {
    return urint(anyInfo[el][iname].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::uval ( " << el << ", " << iname << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

int32_t is_descr::lval( uint32_t el, std::string iname, uint32_t item)
 {
  try {
    return irint(anyInfo[el][iname].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::lval ( " << el << ", " << iname << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint64_t is_descr::ULval( uint32_t el, std::string iname, uint32_t item)
 {
  try {
    return ULrint(anyInfo[el][iname].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::ULval ( " << el << ", " << iname << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

std::string is_descr::sval( uint32_t el, std::string iname, uint32_t item)
 {
  try {
    return sinfo(&anyInfo[el][iname].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::sval ( " << el << ", " << iname << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
  return EmptyString;
  }
 }

void is_descr::vgetv( uint32_t el, std::string iname, uint32_t item, std::vector<uint32_t> & uvv)
 {
  try {
    vgetv ( &anyInfo[el][iname].at(item), uvv ); return;
  } catch (std::exception &exc) {
    BTRACE( "is_descr::vgetv ( " << el << ", " << iname << ", " << item << ", std::vector<uint32_t> & uvv) : " << vsizes[el] << " " << exc.what() );
    uvv.clear();
  }
 }

void is_descr::vgetv( uint32_t el, std::string iname, uint32_t item, std::vector<int32_t> & lvv)
 {
  try {
    vgetv ( &anyInfo[el][iname].at(item), lvv ); return;
  } catch (std::exception &exc) {
    BTRACE( "is_descr::vgetv ( " << el << ", " << iname << ", " << item << ", std::vector<int32_t> & lvv) : " << vsizes[el] << " " << exc.what() );
    lvv.clear();
  }
 }

void is_descr::vgetv( uint32_t el, std::string iname, uint32_t item, std::vector<double> & dvv)
 {
  try {
    vgetv ( &anyInfo[el][iname].at(item), dvv ); return;
  } catch (std::exception &exc) {
    BTRACE( "is_descr::vgetv ( " << el << ", " << iname << ", " << item << ", std::vector<double> & dvv) : " << vsizes[el] << " " << exc.what() );
    dvv.clear();
  }
 }

uint32_t is_descr::uvtot( uint32_t el, std::string iname, uint32_t item)
 {
  try {
    return uvtot ( &anyInfo[el][iname].at(item) );
  } catch (std::exception &exc) {
    BTRACE( "is_descr::uvtot ( " << el << ", " << iname << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

int32_t is_descr::lvtot( uint32_t el, std::string iname, uint32_t item)
 {
  try {
    return lvtot ( &anyInfo[el][iname].at(item) );
  } catch (std::exception &exc) {
    BTRACE( "is_descr::lvtot ( " << el << ", " << iname << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

double is_descr::dsums( uint32_t el, uint32_t item)
 {
  try {
    return vsums[el].at(item);
  } catch (std::exception &exc) {
    BTRACE( "is_descr::dsums ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

double is_descr::dmaxs( uint32_t el, uint32_t item)
 {
  try {
    return vmaxs[el].at(item);
  } catch (std::exception &exc) {
    BTRACE( "is_descr::dmaxs ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

double is_descr::dmins( uint32_t el, uint32_t item)
 {
  try {
    return vmins[el].at(item);
  } catch (std::exception &exc) {
    BTRACE( "is_descr::dmins ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

double is_descr::ddeltas( uint32_t el, uint32_t item)
 {
  try {
    return vdeltas[el].at(item);
  } catch (std::exception &exc) {
    BTRACE( "is_descr::ddeltas ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint32_t is_descr::usums( uint32_t el, uint32_t item)
 {
  try {
    return urint(vsums[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::usums ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint32_t is_descr::umaxs( uint32_t el, uint32_t item)
 {
  try {
    return urint(vmaxs[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::umaxs ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint32_t is_descr::umins( uint32_t el, uint32_t item)
 {
  try {
    return urint(vmins[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::umins ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint32_t is_descr::udeltas( uint32_t el, uint32_t item)
 {
  try {
    return urint(vdeltas[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::udeltas ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

int32_t is_descr::lsums( uint32_t el, uint32_t item)
 {
  try {
    return irint(vsums[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::lsums ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

int32_t is_descr::lmaxs( uint32_t el, uint32_t item)
 {
  try {
    return irint(vmaxs[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::lmaxs ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

int32_t is_descr::lmins( uint32_t el, uint32_t item)
 {
  try {
    return irint(vmins[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::lmins ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

int32_t is_descr::ldeltas( uint32_t el, uint32_t item)
 {
  try {
    return irint(vdeltas[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::ldeltas ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint64_t is_descr::ULsums( uint32_t el, uint32_t item)
 {
  try {
    return ULrint(vsums[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::ULsums ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint64_t is_descr::ULmaxs( uint32_t el, uint32_t item)
 {
  try {
    return ULrint(vmaxs[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::ULmaxs ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint64_t is_descr::ULmins( uint32_t el, uint32_t item)
 {
  try {
    return ULrint(vmins[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::ULmins ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint64_t is_descr::ULdeltas( uint32_t el, uint32_t item)
 {
  try {
    return ULrint(vdeltas[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::ULdeltas ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint32_t is_descr::kmaxs( uint32_t el, uint32_t item)
 {
  try {
    return jmaxs[el].at(item);
  } catch (std::exception &exc) {
    BTRACE( "is_descr::kmaxs ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint32_t is_descr::kmins( uint32_t el, uint32_t item)
 {
  try {
    return jmins[el].at(item);
  } catch (std::exception &exc) {
    BTRACE( "is_descr::kmins ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

double is_descr::dmeans( uint32_t el, uint32_t item)
 {
  try {
    return vmeans[el].at(item);
  } catch (std::exception &exc) {
    BTRACE( "is_descr::dmeans ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint32_t is_descr::umeans( uint32_t el, uint32_t item)
 {
  try {
    return urint(vmeans[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::umeans ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

int32_t is_descr::lmeans( uint32_t el, uint32_t item)
 {
  try {
    return irint(vmeans[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::lmeans ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

uint64_t is_descr::ULmeans( uint32_t el, uint32_t item)
 {
  try {
   return ULrint(vmeans[el].at(item));
  } catch (std::exception &exc) {
    BTRACE( "is_descr::ULmeans ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
   return 0;
  }
 }

double is_descr::dmean2s( uint32_t el, uint32_t item)
 {
  try {
    return vmean2s[el].at(item);
  } catch (std::exception &exc) {
    BTRACE( "is_descr::dmean2s ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

double is_descr::dstds( uint32_t el, uint32_t item)
 {
  try {
    return vstds[el].at(item);
  } catch (std::exception &exc) {
    BTRACE( "is_descr::dstds ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

double is_descr::dmpull( uint32_t el, uint32_t item)
 {
  try {
    return vmpulls[el].at(item);
  } catch (std::exception &exc) {
    BTRACE( "is_descr::dmpull ( " << el << ", " << item << ") : " << vsizes[el] << " " << exc.what() );
    return 0;
  }
 }

void is_descr::vmaxv ( uint32_t el, uint32_t item, std::vector<uint32_t> & uvv)
 {
  try
   {
    uvv.clear();
    for (uint32_t i=0; i<vsizes[el]; i++)
     {
      std::vector<uint32_t> x;
      vgetv ( &(*anyInfoP[el].at(i)).at(item), x );
      size_t N = (x.size() <= uvv.size()) ? x.size() : uvv.size();
      for (size_t j=0; j<N; j++) if (x[j]>uvv[j]) uvv[j]=x[j];
      for (size_t j=uvv.size(); j<x.size(); j++) uvv.push_back(x[j]);
     }
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vmaxv ( " << el << ", " << item << ", std::vector<uint32_t> & uvv) : " << vsizes[el] << " " << exc.what() );
    uvv.clear();
   }
  return;
 }

void is_descr::vmaxv ( uint32_t el, uint32_t item, std::vector<int32_t> & lvv)
 {
  try
   {
    lvv.clear();
    for (uint32_t i=0; i<vsizes[el]; i++)
     {
      std::vector<int32_t> x;
      vgetv ( &(*anyInfoP[el].at(i)).at(item), x );
      size_t N = (x.size() <= lvv.size()) ? x.size() : lvv.size();
      for (size_t j=0; j<N; j++) if (x[j]>lvv[j]) lvv[j]=x[j];
      for (size_t j=lvv.size(); j<x.size(); j++) lvv.push_back(x[j]);
     }
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vmaxv ( " << el << ", " << item << ", std::vector<int32_t> & lvv) : " << vsizes[el] << " " << exc.what() );
    lvv.clear();
   }
  return;
 }

void is_descr::vmaxv ( uint32_t el, uint32_t item, std::vector<double> & dvv)
 {
  try
   {
    dvv.clear();
    for (uint32_t i=0; i<vsizes[el]; i++)
     {
      std::vector<double> x;
      vgetv ( &(*anyInfoP[el].at(i)).at(item), x );
      size_t N = (x.size() <= dvv.size()) ? x.size() : dvv.size();
      for (size_t j=0; j<N; j++) if (x[j]>dvv[j]) dvv[j]=x[j];
      for (size_t j=dvv.size(); j<x.size(); j++) dvv.push_back(x[j]);
     }
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vmaxv ( " << el << ", " << item << ", std::vector<double> & dvv) : " << vsizes[el] << " " << exc.what() );
    dvv.clear();
   }
  return;
 }

void is_descr::vminv ( uint32_t el, uint32_t item, std::vector<uint32_t> & uvv)
 {
  try
   {
    uvv.clear();
    for (uint32_t i=0; i<vsizes[el]; i++)
     {
      std::vector<uint32_t> x;
      vgetv ( &(*anyInfoP[el].at(i)).at(item), x );
      size_t N = (x.size() <= uvv.size()) ? x.size() : uvv.size();
      for (size_t j=0; j<N; j++) if (x[j]<uvv[j]) uvv[j]=x[j];
      for (size_t j=uvv.size(); j<x.size(); j++) uvv.push_back(x[j]);
     }
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vmaxv ( " << el << ", " << item << ", std::vector<uint32_t> & uvv) : " << vsizes[el] << " " << exc.what() );
    uvv.clear();
   }
  return;
 }

void is_descr::vminv ( uint32_t el, uint32_t item, std::vector<int32_t> & lvv)
 {
  try
   {
    lvv.clear();
    for (uint32_t i=0; i<vsizes[el]; i++)
     {
      std::vector<int32_t> x;
      vgetv ( &(*anyInfoP[el].at(i)).at(item), x );
      size_t N = (x.size() <= lvv.size()) ? x.size() : lvv.size();
      for (size_t j=0; j<N; j++) if (x[j]<lvv[j]) lvv[j]=x[j];
      for (size_t j=lvv.size(); j<x.size(); j++) lvv.push_back(x[j]);
     }
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vmaxv ( " << el << ", " << item << ", std::vector<int32_t> & lvv) : " << vsizes[el] << " " << exc.what() );
    lvv.clear();
   }
  return;
 }

void is_descr::vminv ( uint32_t el, uint32_t item, std::vector<double> & dvv)
 {
  try
   {
    dvv.clear();
    for (uint32_t i=0; i<vsizes[el]; i++)
     {
      std::vector<double> x;
      vgetv ( &(*anyInfoP[el].at(i)).at(item), x );
      size_t N = (x.size() <= dvv.size()) ? x.size() : dvv.size();
      for (size_t j=0; j<N; j++) if (x[j]<dvv[j]) dvv[j]=x[j];
      for (size_t j=dvv.size(); j<x.size(); j++) dvv.push_back(x[j]);
     }
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vmaxv ( " << el << ", " << item << ", std::vector<double> & dvv) : " << vsizes[el] << " " << exc.what() );
    dvv.clear();
   }
  return;
 }

void is_descr::vdeltav ( uint32_t el, uint32_t item, std::vector<uint32_t> & uvv)
 {
  try
   {
    std::vector<uint32_t> vmx, vmn;
    vmaxv ( el, item, vmx );
    vminv ( el, item, vmn );
    uvv.clear();
    for (uint32_t j=0; j<vmx.size(); j++) uvv.push_back(vmx[j]-vmn[j]);
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vdeltav ( " << el << ", " << item << ", std::vector<uint32_t> & uvv) : " << vsizes[el] << " " << exc.what() );
    uvv.clear();
   }
  return;
 }

void is_descr::vdeltav ( uint32_t el, uint32_t item, std::vector<int32_t> & lvv)
 {
  try
   {
    std::vector<int32_t> vmx, vmn;
    vmaxv ( el, item, vmx );
    vminv ( el, item, vmn );
    lvv.clear();
    for (uint32_t j=0; j<vmx.size(); j++) lvv.push_back(vmx[j]-vmn[j]);
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vdeltav ( " << el << ", " << item << ", std::vector<int32_t> & lvv) : " << vsizes[el] << " " << exc.what() );
    lvv.clear();
   }
  return;
 }

void is_descr::vdeltav ( uint32_t el, uint32_t item, std::vector<double> & dvv)
 {
  try
   {
    std::vector<double> vmx, vmn;
    vmaxv ( el, item, vmx );
    vminv ( el, item, vmn );
    dvv.clear();
    for (uint32_t j=0; j<vmx.size(); j++) dvv.push_back(vmx[j]-vmn[j]);
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vdeltav ( " << el << ", " << item << ", std::vector<double> & dvv) : " << vsizes[el] << " " << exc.what() );
    dvv.clear();
   }
  return;
 }

void is_descr::vkmaxv ( uint32_t el, uint32_t item, std::vector<uint32_t> & uvv)
 {
  try
   {
    std::vector<double> vmx;
    vmaxv ( el, item, vmx );

    size_t S = vmx.size();
    uvv.resize(S,0);

    for (uint32_t i=1; i<vsizes[el]; i++)
     {
      std::vector<double> x;
      vgetv ( &(*anyInfoP[el].at(i)).at(item), x );
      for (size_t j=0; j<x.size(); j++) if (x[j]==vmx[j]) uvv[j]=j;
     }
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vkmaxv ( " << el << ", " << item << ", std::vector<uint32_t> & uvv) : " << vsizes[el] << " " << exc.what() );
    uvv.clear();
   }
  return;
 }

void is_descr::vkminv ( uint32_t el, uint32_t item, std::vector<uint32_t> & uvv)
 {
  try
   {
    std::vector<double> vmn;
    vminv ( el, item, vmn );

    size_t S = vmn.size();
    uvv.resize(S,0);

    for (uint32_t i=1; i<vsizes[el]; i++)
     {
      std::vector<double> x;
      vgetv ( &(*anyInfoP[el].at(i)).at(item), x );
      for (size_t j=0; j<x.size(); j++) if (x[j]==vmn[j]) uvv[j]=j;
     }
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vkminv ( " << el << ", " << item << ", std::vector<uint32_t> & uvv) : " << vsizes[el] << " " << exc.what() );
    uvv.clear();
   }
  return;
 }

   void vmeanv( uint32_t el, uint32_t item, std::vector<uint32_t> & uvv);
   void vmeanv( uint32_t el, uint32_t item, std::vector<int32_t> & lvv);
   void vmeanv( uint32_t el, uint32_t item, std::vector<double> & dvv);

void is_descr::vmeanv ( uint32_t el, uint32_t item, std::vector<uint32_t> & uvv)
 {
  try
   {
    double n_el = double(vsizes[el]);
    if (n_el == 0) return;

    std::vector<double> vsum;
    vsumv ( el, item, vsum );

    size_t s = vsum.size();
    if (s == 0) return;

    uvv.resize(s,0);
    double scale = 1.0/n_el;
    for (size_t j=0; j<s; j++) uvv[j] = urint(vsum[j]*scale);
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vmeanv ( " << el << ", " << item << ", std::vector<uint32_t> & uvv) : " << vsizes[el] << " " << exc.what() );
    uvv.clear();
   }
  return;
 }

void is_descr::vmeanv ( uint32_t el, uint32_t item, std::vector<int32_t> & lvv)
 {
  try
   {
    double n_el = double(vsizes[el]);
    if (n_el == 0) return;

    std::vector<double> vsum;
    vsumv ( el, item, vsum );

    size_t s = vsum.size();
    if (s == 0) return;

    lvv.resize(s,0);
    double scale = 1.0/n_el;
    for (size_t j=0; j<s; j++) lvv[j] = irint(vsum[j]*scale);
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vmeanv ( " << el << ", " << item << ", std::vector<uint32_t> & lvv) : " << vsizes[el] << " " << exc.what() );
    lvv.clear();
   }
  return;
 }

void is_descr::vmeanv ( uint32_t el, uint32_t item, std::vector<double> & dvv)
 {
  try
   {
    double n_el = double(vsizes[el]);
    if (n_el == 0) return;

    std::vector<double> vsum;
    vsumv ( el, item, vsum );

    size_t s = vsum.size();
    if (s == 0) return;

    dvv.resize(s,0);
    double scale = 1.0/n_el;
    for (size_t j=0; j<s; j++) dvv[j] = vsum[j]*scale;
   }
  catch (std::exception &exc)
   {
    BTRACE( "is_descr::vmeanv ( " << el << ", " << item << ", std::vector<double> & dvv) : " << vsizes[el] << " " << exc.what() );
    dvv.clear();
   }
  return;
 }

