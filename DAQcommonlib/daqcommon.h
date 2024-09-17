
#include <ctime>
#include <sys/time.h>
#include <stdint.h>
#include <string>

const uint32_t ONEK = 1000;
const uint32_t ONEM = ONEK*ONEK;
const uint32_t ONEG = ONEK*ONEM;

void x_nsleep ( uint32_t ns );

void x_usleep (uint32_t us );

void dlwait( std::string msg );

class uTimer
 {
  public:
    uTimer()
     {
      m_StartTime = 0;
      m_EndTime = 0;
      m_OffsetTime = 0;
      m_bRunning = false;
     }
    void setOffset()
     {
      m_OffsetTime = time(NULL);
     }
    time_t getOffset()
     {
      return m_OffsetTime;
     }
    double utime()
     {
      struct timeval tod;
      gettimeofday ( &tod, NULL );
      time_t lastsec = tod.tv_sec-m_OffsetTime;
      return double(lastsec)*1E6+double(tod.tv_usec);
     }
    void start()
     {
      m_StartTime = this->utime();
      m_bRunning = true;
     }
    void stop()
     {
      m_EndTime = this->utime();
      m_bRunning = false;
     }
    double elapsedMicroseconds()
     {
      double endTime;
      if(m_bRunning)
        {
         endTime = this->utime();
        }
       else
        {
         endTime = m_EndTime;
        }
       return (endTime - m_StartTime);
      }
	    
    double elapsedSeconds()
     {
      double endTime;
      if(m_bRunning)
       {
        endTime = this->utime();
        }
       else
        {
         endTime = m_EndTime;
        }
       return (endTime - m_StartTime)/ONEM;
      }
	    
  private:
    double m_StartTime;
    double m_EndTime;
    time_t m_OffsetTime;
    bool m_bRunning;
 };

