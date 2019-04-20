#ifndef CNC_SPEED_SIMULATOR
#define CNC_SPEED_SIMULATOR

#include <iostream>
#include <vector>

#include "OSD/CncTimeFunctions.h"
#include "CncCommon.h"
#include "CncSpeedController.h"

//////////////////////////////////////////////////////////////////////////////////
class CncSpeedSimulator;
class DelayAccumulator {

	private:
		static const int64_t DefaultThresholdMicros = 20 * 1000; // 20 ms

		// ----------------------------------------------------------------------
		struct Statistics {
			unsigned long targetEqual 		= 0;
			unsigned long targetSmaller		= 0;
			unsigned long targetLarger		= 0;
			unsigned long sleeped			= 0;

			int64_t minSleepInterval 		= 0;
			int64_t maxSleepInterval 		= 0;
			int64_t avgSleepInterval 		= 0;

			void reset() { *this = Statistics(); }

			void trace(std::ostream& o) {
				o << "Statistic: " << targetSmaller << ", " << targetEqual << ", " << targetLarger << " | "
				  << "Sleeped: # " << sleeped << "(" << minSleepInterval << "us, " << avgSleepInterval << "us, "  << maxSleepInterval<< "us)"
				  << std::endl;
			}

		};

		// ----------------------------------------------------------------------
		struct SleepFlowEntry {

			CncNanoTimestamp tsNow 				= 0;
			CncNanoTimestamp tsRef				= 0;
			CncNanoTimestamp tsLast				= 0;
			CncNanoTimestamp tsTarget			= 0;
			CncMicroTimespan elapsedMicros		= 0;

			int64_t stepCountX					= 0;
			int64_t stepCountY					= 0;
			int64_t stepCountZ					= 0;
			int64_t incrementMicros				= 0;
			int64_t accumulatedMicros			= 0;
			int64_t sleepIntervalMicros			= 0;

			void reset() {
				*this = SleepFlowEntry();
			}

			void next() {
				SleepFlowEntry sf;
				sf.accumulatedMicros 	= accumulatedMicros;
				sf.tsRef 				= tsRef;
				sf.tsLast 				= tsLast;

				*this = sf;
			}

			static void traceHeadline(std::ostream& o);
			void trace(std::ostream& o);

		};

		CncSpeedSimulator* 			simulator;
		Statistics					statistics;
		SleepFlowEntry				currentSleepEntry;
		std::vector<SleepFlowEntry> sleepStatistics;

		inline int64_t performA(bool force = false);
		inline int64_t performB(bool force = false);
		inline int64_t performC(bool force = false);

		inline int64_t sleepMicros(int64_t micros);

	public:

		DelayAccumulator(CncSpeedSimulator* s);
		~DelayAccumulator();

		void trace(std::ostream& o);

		void initialize();
		bool incMicros(int64_t micros);
		int64_t perform(bool force = false);

};

//////////////////////////////////////////////////////////////////////////////////
class CncSpeedSimulator : public CncSpeedController {
	
	public:
		
		//////////////////////////////////////////////////////////////////////////
		CncSpeedSimulator();
		virtual ~CncSpeedSimulator();
		
		void simulateOneStepX();
		void simulateOneStepY();
		void simulateOneStepZ();
		
		void performCurrentOffset(bool force = false);

		virtual void initMove(int32_t dx, int32_t dy, int32_t dz);
		virtual void completeMove();

	private:
		
		DelayAccumulator delayAccumulator;
};

#endif
