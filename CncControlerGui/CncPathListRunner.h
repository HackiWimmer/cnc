#ifndef CNC_PATH_LIST_RUNNER_H
#define CNC_PATH_LIST_RUNNER_H

#include "CncPathListManager.h"

class CncControl;
class CncMoveSequence;
class FileParser;

class CncPathListRunner {
	
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
