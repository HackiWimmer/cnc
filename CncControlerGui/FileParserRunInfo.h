#ifndef FILE_PARSER_RUN_INFO
#define FILE_PARSER_RUN_INFO

#include "CncCommon.h"

/////////////////////////////////////////////////////////////////////
class FileParserRunInfo {
	
	public:
		///////////////////////////////////////////////////////////////
		enum RunPhase {RP_Unknown=0, RP_Preprocesser=1, RP_UserAgent=2,  RP_Spool=3, RP_LAST=4};
		
	private:
		RunPhase runPhase;
		RunPhase finalizeAfter;
		int lastLineNumber;
		bool state;
		bool stop;
		bool next;
		bool pause;
		
		bool debugMode[RP_LAST];
		
	public:
		///////////////////////////////////////////////////////////////
		FileParserRunInfo() 
		: runPhase(RP_Unknown)
		, finalizeAfter(RP_LAST)
		, lastLineNumber(UNDEFINED_LINE_NUMBER)
		, state(false)
		, stop(false)
		, next(false)
		, pause(false)
		{
			for ( unsigned int i=0; i<RP_LAST; i++)
				debugMode[i] = false;
		}
		///////////////////////////////////////////////////////////////
		~FileParserRunInfo() {
		}
		
		///////////////////////////////////////////////////////////////
		void stopProcessingAfter(RunPhase fa) {
			finalizeAfter = fa;
		}
		
		bool processMore() {
			if ( runPhase >= finalizeAfter ) {
				std::clog << "Run would be finalized after step: " << getCurrentDebugPhaseAsString() << std::endl;
				return false;
			}
				
			return true;
		}
		
		///////////////////////////////////////////////////////////////
		bool getCurrentDebugMode() { return debugMode[runPhase]; }
		
		void setDebugMode(RunPhase p, bool mode) {
			if ( p == RP_Unknown)
				return;
				
			if ( p == RP_LAST)
				return;
				
			debugMode[p] = mode;
		}
		///////////////////////////////////////////////////////////////
		void debugAllPhases() {
			for ( unsigned int i=1; i<RP_LAST; i++)
				debugMode[i] = true;
		}
		///////////////////////////////////////////////////////////////
		void releaseAllPhases() {
			for ( unsigned int i=1; i<RP_LAST; i++)
				debugMode[i] = false;
		}
		///////////////////////////////////////////////////////////////
		void setCurrentRunPhase(RunPhase p) { 
			if ( p == RP_LAST ) runPhase = RP_Unknown;
			else					runPhase = p; 
			
			setCurrentDebugState(debugMode[runPhase]);
		}
		
		///////////////////////////////////////////////////////////////
		RunPhase getCurrentRunPhase() { return runPhase; }
		
		///////////////////////////////////////////////////////////////
		bool isProcessing() { return runPhase != RP_Unknown;};
		
		///////////////////////////////////////////////////////////////
		void setCurrentDebugState(bool s) { state = s; }
		bool getCurrentDebugState() { return state; }
		
		///////////////////////////////////////////////////////////////
		void setStopFlag(bool s) { stop = s; }
		bool getStopFlag() { return stop; }
		
		///////////////////////////////////////////////////////////////
		void setNextFlag(bool n) { next = n; }
		bool getNextFlag() { return next; }
		
		///////////////////////////////////////////////////////////////
		void setPauseFlag(bool p) { pause = p; }
		bool getPauseFlag() { return pause; }
		///////////////////////////////////////////////////////////////
		int getLastLineNumber() { return lastLineNumber; }
		void setLastLineNumber(int ln) { lastLineNumber = ln; }
		
		///////////////////////////////////////////////////////////////
		const char* getCurrentDebugPhaseAsString() {
			switch ( runPhase) {
				case RP_Preprocesser: 	return "Preprocessor";
				case RP_UserAgent:		return "User Agent";
				case RP_Spool:			return "Spooling";
				default:					return "";
			}
		}
		
		////////////////////////////////////////////////////////////////
		const char* getDebugModeAsString() {
			if ( getCurrentDebugMode() == true ) 	return "Debug Mode";
			else									return "Release Mode";
		}
};

#endif