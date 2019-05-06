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
		
		bool onPhysicallyClientIdChange(long idx, const CncPathListEntry& curr);
		bool onPhysicallySpeedChange(unsigned long idx, const CncPathListEntry& curr);
		bool onPhysicallyMoveRaw(unsigned long idx, const CncPathListEntry& curr);
		bool onPhysicallyMoveAnalysed(unsigned long idx, const CncPathListEntry* curr, const CncPathListEntry* next);
		
		bool destroyMoveSequence();
		bool initNextMoveSequence();
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
