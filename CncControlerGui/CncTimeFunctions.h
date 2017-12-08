#ifndef CNC_TIME_FUMCTIONS
#define CNC_TIME_FUMCTIONS

#include <sys/time.h>

///////////////////////////////////////////////////////////////////////
// max isze of long long     : 		9223372036854775807 
// max mirco seconds of a day:		        86400000000
typedef long long CncTimestamp;
typedef long long CncTimespan;

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
		const CncTimestamp getAsTimestamp() const { return (CncTimestamp)(tv_sec * 1000 * 1000 + tv_usec); }
		
};


///////////////////////////////////////////////////////////////////////
class CncTimeFunctions {

	private:
		static void printError(const char *tag);
		static void init_gettimeofday();
		
	public:
		static void init();
		static int gettimeofday(struct timeval *tv, void *tz_unused);
		static void busyWaitMircoseconds(unsigned int micros);
		
		static CncTimestamp getMicrosecondTimestamp();
		static CncTimespan getTimeSpan(const CncTimeval& a, const CncTimeval& b);
		static CncTimespan getTimeSpan(const CncTimestamp& a, const CncTimestamp& b);
};

#endif