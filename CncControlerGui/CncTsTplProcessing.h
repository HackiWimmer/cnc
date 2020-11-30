#ifndef CNC_TS_TPL_PROCESSING_H
#define CNC_TS_TPL_PROCESSING_H

#include <wx/string.h>
#include "OSD/CncTimeFunctions.h"
#include "CncPosition.h"
#include "CncAverage.h"

struct TsTplProcessing {
	
	private:
		
		CncNanoTimestamp tsTotalStart	= CncTimeFunctions::getNanoTimestamp();
		CncNanoTimestamp tsTotalEnd	 	= CncTimeFunctions::getNanoTimestamp();

		CncNanoTimestamp tsPreStart	 	= CncTimeFunctions::getNanoTimestamp();
		CncNanoTimestamp tsPreEnd		= CncTimeFunctions::getNanoTimestamp();

		CncNanoTimestamp tsPostStart	= CncTimeFunctions::getNanoTimestamp();
		CncNanoTimestamp tsPostEnd	 	= CncTimeFunctions::getNanoTimestamp();

		CncNanoTimestamp tsSerialStart 	= CncTimeFunctions::getNanoTimestamp();
		CncNanoTimestamp tsSerialEnd	= CncTimeFunctions::getNanoTimestamp();

	public:
		
		struct MeasuredDurations {
			
			CncAverage<long> md1;
			CncAverage<long> md2;
			CncAverage<long> md3;
			CncAverage<long> md4;
			CncAverage<long> md5;
			
		} measuredDurations;
		
		// -------------------------------------------------------
		void logTotalTimeStart() 	{ tsTotalStart	= CncTimeFunctions::getNanoTimestamp(); }
		void logTotalTimeEnd() 		{ tsTotalEnd	= CncTimeFunctions::getNanoTimestamp(); }

		void logPreTimeStart() 		{ tsPreStart	= CncTimeFunctions::getNanoTimestamp(); }
		void logPreTimeEnd() 		{ tsPreEnd		= CncTimeFunctions::getNanoTimestamp(); }

		void logPostTimeStart()		{ tsPostStart	= CncTimeFunctions::getNanoTimestamp(); }
		void logPostTimeEnd() 		{ tsPostEnd		= CncTimeFunctions::getNanoTimestamp(); }

		void logSerialTimeStart() 	{ tsSerialStart	= CncTimeFunctions::getNanoTimestamp(); }
		void logSerialTimeEnd() 	{ tsSerialEnd	= CncTimeFunctions::getNanoTimestamp(); }

		// -------------------------------------------------------
		const wxString& getTimeConsumedFormated(wxString& ret, const CncNanoTimespan duartionNanos) const {
			int n = (int) ( duartionNanos % std::nano::den );
			int s = (int) ( duartionNanos / std::nano::den) % 60 ;
			int m = (int) ((duartionNanos / (std::nano::den * 60)) % 60);
			int h = (int) ((duartionNanos / (std::nano::den * 60 * 60)) % 24);

			ret.assign(wxString::Format("%02d:%02d:%02d.%06d", h, m, s, n / 1000));
			return ret;
		}

		// -------------------------------------------------------
		const wxString& getTimeConsumedFormated(wxString& ret, CncNanoTimestamp tsStart, CncNanoTimestamp tsEnd) const {
			const CncNanoTimespan duartionNanos = CncTimeFunctions::getTimeSpan(tsEnd, tsStart);
			return getTimeConsumedFormated(ret, duartionNanos);
		}

		long getTotalDurationMillis() 	const { return getTotalDurationMicros()  / 1000; }
		long getPreDurationMillis()   	const { return getPreDurationMicros()    / 1000; }
		long getSerialDurationMillis()	const { return getSerialDurationMicros() / 1000; }
		long getPostDurationMillis()  	const { return getPostDurationMicros()   / 1000; }

		long getTotalDurationMicros() 	const { return getTotalDurationNanos()   / 1000; }
		long getPreDurationMicros()   	const { return getPreDurationNanos()     / 1000; }
		long getSerialDurationMicros()	const { return getSerialDurationNanos()  / 1000; }
		long getPostDurationMicros()  	const { return getPostDurationNanos()    / 1000; }

		CncNanoTimestamp getTotalDurationNanos() 	const { return CncTimeFunctions::getTimeSpan(tsTotalEnd,  tsTotalStart);  }
		CncNanoTimestamp getPreDurationNanos()   	const { return CncTimeFunctions::getTimeSpan(tsPreEnd,    tsPreStart);    }
		CncNanoTimestamp getSerialDurationNanos()	const { return CncTimeFunctions::getTimeSpan(tsSerialEnd, tsSerialStart); }
		CncNanoTimestamp getPostDurationNanos()  	const { return CncTimeFunctions::getTimeSpan(tsPostEnd,   tsPostStart);   }

		const wxString& getTotalTimeConsumedFormated(wxString& ret)  const { return getTimeConsumedFormated(ret, tsTotalStart,  tsTotalEnd);  }
		const wxString& getPreTimeConsumedFormated(wxString& ret)    const { return getTimeConsumedFormated(ret, tsPreStart,    tsPreEnd);    }
		const wxString& getSerialTimeConsumedFormated(wxString& ret) const { return getTimeConsumedFormated(ret, tsSerialStart, tsSerialEnd); }
		const wxString& getPostTimeConsumedFormated(wxString& ret)   const { return getTimeConsumedFormated(ret, tsPostStart,   tsPostEnd);   }

		// -------------------------------------------------------
		friend std::ostream &operator<< (std::ostream& ostr, const TsTplProcessing& ts) {
			
			// ---------------------------------------------------
			auto formatDuartion = [](wxString& ret, const CncAverage<long>& a) { 
				const long f = 1000;
				
				ret.assign(wxString::Format("[min=% 5ld, avg=% 5ld, max=% 5ld]", 
											a.getMin() / f,
											a.getAvg() / f, 
											a.getMax() / f
						  ));
				
				//ostr << a.getMin() / f << " < " << a.getAvg() / f << " > " << a.getMax() / f << "[us] s = " << a.getSum() / f / 100 << "[ms] c = " << a.count();
				
				return ret;
			};
			
			const CncNanoTimestamp rest = ts.getTotalDurationNanos()
										- ts.getPreDurationNanos()
										- ts.getSerialDurationNanos()
										- ts.getPostDurationNanos();
			
			wxString value;
			ostr 	<< "Time consumed:"														<< std::endl
					<< " * Total  : " << ts.getTotalTimeConsumedFormated	(value)			<< std::endl
					<< " * Pre    : " << ts.getPreTimeConsumedFormated		(value) 		<< std::endl
					<< " * Serial : " << ts.getSerialTimeConsumedFormated	(value)			<< std::endl
					<< " * Post   : " << ts.getPostTimeConsumedFormated		(value)			<< std::endl
					<< " * Rest   : " << ts.getTimeConsumedFormated			(value, rest)	<< std::endl
					
					<< "Durations:"															<< std::endl
					<< " * m1     : " << formatDuartion(value, ts.measuredDurations.md1)	<< std::endl
					<< " * m2     : " << formatDuartion(value, ts.measuredDurations.md2)	<< std::endl
					<< " * m3     : " << formatDuartion(value, ts.measuredDurations.md3)	<< std::endl
					<< " * m4     : " << formatDuartion(value, ts.measuredDurations.md4)	<< std::endl
					<< " * m5     : " << formatDuartion(value, ts.measuredDurations.md5)	<< std::endl
					
					;
			return ostr;
		}

};

#endif