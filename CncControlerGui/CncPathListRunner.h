#ifndef CNC_PATH_LIST_RUNNER_H
#define CNC_PATH_LIST_RUNNER_H

#include "CncPathListManager.h"

class CncControl;
class CncMoveSequence;
class FileParser;

class CncPathListRunner {
	
	private:

		struct Move {
			double dx;
			double dy;
			double dz;
			double mxy;
			double vxy;
			double mz;

			explicit Move(const CncPathListEntry* e)
			: dx(e->entryDistance.getX())
			, dy(e->entryDistance.getY())
			, dz(e->entryDistance.getZ())
			, mxy(dx != 0.0 ? dy/dx : DBL_MAX)
			, vxy(sqrt(pow(dx, 2) + pow(dy, 2)))
			, mz(vxy != 0.0 ? dz / vxy : DBL_MAX)
			{}

			explicit Move(const Move& m)
			: dx(m.dx)
			, dy(m.dy)
			, dz(m.dz)
			, mxy(m.mxy)
			, vxy(m.vxy)
			, mz(m.mz)
			{}

			bool isPitchEqual(const Move& m) const {
				const double epsilon = 0.001;
				return ( cnc::dblCompare(mxy, m.mxy, epsilon) == true && cnc::dblCompare(mz, m.mz, epsilon) == true );
			}

			bool isPitchToStrong(const Move& m) const {
				const float max = 15 * PI / 180; // 15 degrees
				const float a1 = atan2(dx, dy);
				const float a2 = atan2(m.dx, m.dy);

				return abs(a1 - a2) > max;
			}

			float getPitchDiffenceAsRadians(const Move& m) const {
				const float a1 = atan2(dx, dy);
				const float a2 = atan2(m.dx, m.dy);

				return abs(a1 - a2);
			}

			float getPitchDiffenceAsDegree(const Move& m) const {
				const float a1 = atan2(dx, dy);
				const float a2 = atan2(m.dx, m.dy);

				return abs(a1 - a2) * 180 / PI;
			}
		};

	public:
		struct Setup {
			CncControl* cnc				= NULL;
			FileParser*	fileParser		= NULL;
			
			bool optAnalyse				= true;
			bool optSkipEmptyMoves		= true;
			bool optCombineMoves		= true;
			
		};
		
		CncPathListRunner::Setup& getSetup() { return setup; }
		
	private:
	
		CncMoveSequence* currentSequence;
		Setup setup;
		
		inline bool isCncInterrupted();
		inline bool checkDebugState();

		bool onPhysicallyClientIdChange(const CncPathListEntry& curr);
		bool onPhysicallySpeedChange(const CncPathListEntry& curr);
		bool onPhysicallyMoveRaw(const CncPathListEntry& curr);
		bool onPhysicallyMoveAnalysed(CncPathList::const_iterator& it, const CncPathList::const_iterator& end);
		
		bool destroyMoveSequence();
		bool initNextMoveSequence(double value_MM_MIN = 0.0, char mode = '-');
		bool publishMoveSequence();
		
	public:
		
		explicit CncPathListRunner(const CncPathListRunner::Setup& setup);
		explicit CncPathListRunner(CncControl* cnc);
		~CncPathListRunner();
		
		void logMeasurementStart();
		void logMeasurementEnd();
		
		void onPhysicallySwitchToolState(bool state);
		bool onPhysicallyExecute(const CncPathListManager& plm);
};

#endif
