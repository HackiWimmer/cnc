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

			explicit Move(const CncPathListEntry* e);
			explicit Move(const Move& m);

			bool	isXYZPitchEqual(const Move& m) const;
			
			bool	isZPitchDiffToStrong(const Move& m) const;
			bool	isXYPitchDiffToStrong(const Move& m) const;
			bool	isXYZPitchDiffToStrong(const Move& m) const;
			
			float	getXYPitchDiffenceAsRadians(const Move& m) const;
			float	getXYPitchDiffenceAsDegree(const Move& m) const;
			
			float	getZPitchDiffenceAsRadians(const Move& m) const;
			float	getZPitchDiffenceAsDegree(const Move& m) const;
			
			static	float maxXYPitchRadians;
			static	float maxZPitchRadians;
			
			static	float degree2Radians(float d)	{ return d * PI / 180.0; }
			static	float radians2Degree(float r)	{ return r * 180.0 / PI; }
		};

	public:
		struct Setup {
			CncControl* cnc				= NULL;
			FileParser*	fileParser		= NULL;
			
			bool optAnalyse				= true;
			bool optSkipEmptyMoves		= true;
			bool optCombineMoves		= true;
			
			// for testing only
			bool trace					= true;
		};
		
		CncPathListRunner::Setup& getSetup() { return setup; }
		
	private:
	
		CncMoveSequence* currentSequence;
		Setup setup;
		
		inline bool isCncInterrupted();
		inline bool checkDebugState();
		
		void traceSetup();

		bool onPhysicallyClientIdChange(const CncPathListEntry& curr);
		bool onPhysicallySpeedChange(const CncPathListEntry& curr, const CncPathListEntry* next);
		bool onPhysicallyMoveRaw(const CncPathListEntry& curr);
		bool onPhysicallyMoveAnalysed(CncPathList::const_iterator& it, const CncPathList::const_iterator& end);
		
		bool initializeNextMoveSequence(long nextClientId);
		bool initializeNextMoveSequence(double value_MM_MIN, char mode, long nextClientId);
		bool finalizeCurrMoveSequence(long nextClientId);
		bool addSequenceEntryFromValues(double dx, double dy, double dz);
		bool addSequenceEntryFromEntry(const CncPathListEntry* e);
		
		// don't call this functions directly. use initializeNextMoveSequence() 
		// or finalizeCurrMoveSequence() instead
		bool destroyMoveSequence();
		bool publishMoveSequence();
		
	public:
		
		explicit CncPathListRunner(const CncPathListRunner::Setup& setup);
		explicit CncPathListRunner(CncControl* cnc);
		~CncPathListRunner();
		
		void logMeasurementStart();
		void logMeasurementEnd();
		
		void onPhysicallySwitchToolState(bool state);
		bool onPhysicallyExecute(const CncPathListManager& plm);
		
		void autoSetup(bool trace);
};

#endif
