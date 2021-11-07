#ifndef FILE_PARSER_RUN_INFO
#define FILE_PARSER_RUN_INFO

#include "CncCommon.h"

/////////////////////////////////////////////////////////////////////
class CncProcessingInfo {
	
	public:
		///////////////////////////////////////////////////////////////
		enum RunPhase {RP_Unknown=0, RP_Preparation=1, RP_Preprocesser=2, RP_Spool=3, RP_LAST=4 };
		
	private:
		RunPhase runPhase;
		RunPhase finalizeAfter;
		int lastLineNumber;
		bool waitingForUserEvents;
		bool state;
		bool stop;
		bool next;
		bool pause;
		
		bool debugMode[RP_LAST];
		
	public:
		CncProcessingInfo();
		~CncProcessingInfo();
		
		void reset();
		
		void initPrepationPhase();
		void stopProcessingAfter(RunPhase fa);
		bool processMore();
		void setDebugMode(RunPhase p, bool mode);
		void debugAllPhases();
		void releaseAllPhases();
		void setCurrentRunPhase(RunPhase p);
		
		RunPhase getCurrentRunPhase()		const	{ return runPhase; }
		bool isProcessing()					const	{ return runPhase != RP_Unknown; }
		bool isPause()						const	{ return getPauseFlag(); }
		bool isWaitingForUserEvents()		const	{ return waitingForUserEvents; }
		bool getCurrentDebugState()			const	{ return state; }
		bool getCurrentDebugMode()			const	{ return debugMode[runPhase]; }
		bool getStopFlag()					const	{ return stop; }
		bool getStepByStepFlag()			const	{ return next; }
		bool getPauseFlag()					const	{ return pause; }
		
		bool togglePause()							{ setPauseFlag(!getPauseFlag()); return getPauseFlag(); }
		
		void setCurrentDebugState(bool s)			{ state = s; }
		void setStopFlag(bool s)					{ stop = s; }
		void setStepByStepFlag(bool n)				{ next = n; }
		void setPauseFlag(bool p)					{ pause = p; }
		void setWaitingForUserEvents(bool s)		{ waitingForUserEvents = s; }
		
		int getLastLineNumber()				const	{ return lastLineNumber; }
		bool isLastLineNumberDefined()		const	{ return lastLineNumber != UNDEFINED_LINE_NUMBER; }
		void resetLastLineNumber()					{ lastLineNumber = UNDEFINED_LINE_NUMBER; }
		void setLastLineNumber(int ln)				{ lastLineNumber = ln; }
		
		const char* getCurrentDebugPhaseAsString() const;
		const char* getDebugModeAsString() const;
};

#endif