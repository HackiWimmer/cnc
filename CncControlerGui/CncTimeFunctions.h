#ifndef CNC_TIME_FUMCTIONS
#define CNC_TIME_FUMCTIONS

#include <chrono>
#include <sys/time.h>

///////////////////////////////////////////////////////////////////////
// max size of long long     : 		9223372036854775807 
// max mirco seconds of a day:		        86400000000
// max nano seconds of a day:		     86400000000000
typedef long long   CncNanoTimestamp;
typedef long double CncMicroTimestamp;
typedef double      CncMilliTimestamp;
typedef double      CncSecondTimestamp;
typedef long long   CncNanoTimespan;


struct CncTimeval : public timeval {
	
	public:
		
		////////////////////////////////////////////////////////////////
		friend bool operator== (const CncTimeval &a, const CncTimeval &b) {
			return ( a.tv_sec == b.tv_sec && a.tv_usec == b.tv_usec );
		}
		
		////////////////////////////////////////////////////////////////
		friend bool operator!= (const CncTimeval &a, const CncTimeval &b) {
			return ( !(a == b) );
		}
		
		////////////////////////////////////////////////////////////////
		friend CncTimeval operator- (const CncTimeval &a, const CncTimeval &b) {
			CncTimeval c;
			c.tv_sec  = a.tv_sec  - b.tv_sec;
			c.tv_usec = a.tv_usec - b.tv_usec;
			return c;
		}
		
		////////////////////////////////////////////////////////////////
		friend CncTimeval operator+ (const CncTimeval &a, const CncTimeval &b) {
			CncTimeval c;
			c.tv_sec  = a.tv_sec  + b.tv_sec;
			c.tv_usec = a.tv_usec + b.tv_usec;
			return c;
		}
		
		////////////////////////////////////////////////////////////////
		friend bool operator> (const CncTimeval &a, const CncTimeval &b) {
			return a.getAsTimestamp() > b.getAsTimestamp();
		}
		
		////////////////////////////////////////////////////////////////
		friend bool operator< (const CncTimeval &a, const CncTimeval &b) {
			return a.getAsTimestamp() < b.getAsTimestamp();
		}
		
		////////////////////////////////////////////////////////////////
		const CncNanoTimestamp getAsTimestamp() const { return (CncNanoTimestamp)(tv_sec * 1000 * 1000 + tv_usec); }
		
};

///////////////////////////////////////////////////////////////////////
class CncTimeFunctions {
	
	private:
		static void printError(const char *tag);
		static void init_gettimeofday();
		
	public:
		
		typedef std::nano                                         period;
		typedef std::chrono::duration<CncNanoTimestamp, period>   duration;
		typedef std::chrono::time_point<CncTimeFunctions>         time_point;

		static void init();
		
		static int64_t getOPCFrequency();
		static unsigned int geMaxtQPCResolutionNS();
				
		static int gettimeofday(struct timeval *tv, void *tz_unused);
		static void sleepMircoseconds(int64_t micros);
		static void activeWaitMircoseconds(int64_t micros, bool active=true);
		static void busyWaitMircoseconds(unsigned int micros);
		
		static CncNanoTimestamp   getNanoTimestamp();
		static CncMicroTimestamp  getMicroTimestamp() 	{ return getNanoTimestamp() / (double)std::kilo::den; }
		static CncMilliTimestamp  getMilliTimestamp() 	{ return getNanoTimestamp() / (double)std::mega::den; }
		static CncSecondTimestamp getSecondTimestamp() 	{ return getNanoTimestamp() / (double)std::giga::den; }
		
		static CncNanoTimespan getTimeSpan(const CncTimeval& a, const CncTimeval& b);
		static CncNanoTimespan getTimeSpan(const CncNanoTimestamp& a, const CncNanoTimestamp& b);
		
};

#endif