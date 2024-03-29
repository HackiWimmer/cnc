#ifndef CNC_PATH_LIST_RUNNER_H
#define CNC_PATH_LIST_RUNNER_H

#include <iostream>
#include "CncVector.h"
#include "CncPathListManager.h"

class CncMoveSequence;
class FileParser;

///////////////////////////////////////////////////////////////////
class CncPathListRunner 
{
	private:
		
		// ---------------------------------------------------
		struct WorkflowEntry
		{
			WorkflowEntry() {}
			virtual ~WorkflowEntry() {}
			virtual bool process(CncPathListRunner* plr)	= 0;
			virtual void traceTo(std::ostream& o) const		= 0;
			
			virtual CncPathListManager* getPLM()					{ return NULL; }
			virtual bool isEndRunTrigger()			const			{ return false; }
		};
		
		// ---------------------------------------------------
		struct WorkflowSetupRunEntry : public WorkflowEntry
		{
			bool trace;
			explicit WorkflowSetupRunEntry(bool t) : trace (t) {}
			virtual bool process(CncPathListRunner* plr) override;
			virtual void traceTo(std::ostream& o) const override;
		};
		
		// ---------------------------------------------------
		struct WorkflowTriggerBeginRunEntry : public WorkflowEntry
		{
			Trigger::BeginRun tr;
			
			explicit WorkflowTriggerBeginRunEntry(const Trigger::BeginRun& t) : tr (t) {}
			virtual bool process(CncPathListRunner* plr) override;
			virtual void traceTo(std::ostream& o) const override;
		};
		
		// ---------------------------------------------------
		struct WorkflowTriggerEndRunEntry : public WorkflowEntry
		{
			Trigger::EndRun tr;
			
			explicit WorkflowTriggerEndRunEntry(const Trigger::EndRun& t) : tr (t) {}
			virtual bool process(CncPathListRunner* plr) override;
			virtual void traceTo(std::ostream& o) const override;
			virtual bool isEndRunTrigger() const { return true; }
		};
		// ---------------------------------------------------
		struct WorkflowTriggerNextPathEntry : public WorkflowEntry
		{
			Trigger::NextPath tr;
			
			explicit WorkflowTriggerNextPathEntry(const Trigger::NextPath& t) : tr (t) {}
			virtual bool process(CncPathListRunner* plr) override;
			virtual void traceTo(std::ostream& o) const override;
		};
		
		///////////////////////////////////////////////////////////////////
		struct WorkflowTriggerSpeedChangeEntry : public WorkflowEntry
		{
			Trigger::SpeedChange tr;
			
			explicit WorkflowTriggerSpeedChangeEntry(const Trigger::SpeedChange& t) : tr (t) {}
			virtual bool process(CncPathListRunner* plr) override;
			virtual void traceTo(std::ostream& o) const override;
		};
		// ---------------------------------------------------
		struct WorkflowTriggerGuidePathEntry : public WorkflowEntry
		{
			Trigger::GuidePath tr;
			
			explicit WorkflowTriggerGuidePathEntry(const Trigger::GuidePath& t) : tr (t) {}
			virtual bool process(CncPathListRunner* plr) override;
			virtual void traceTo(std::ostream& o) const override;
		};
		
		// ---------------------------------------------------
		struct WorkflowCncEntry : public WorkflowEntry
		{
			CncPathListManager plm;
			
			explicit WorkflowCncEntry(const CncPathListManager& p) : plm (p) {}
			
			virtual bool process(CncPathListRunner* plr) override;
			virtual void traceTo(std::ostream& o) const override;
			
			virtual CncPathListManager* getPLM() { return &plm; }
		};
		
		// ---------------------------------------------------
		struct WorkflowGuideEntry : public WorkflowEntry
		{
			CncPathListManager plm;
			
			explicit WorkflowGuideEntry(const CncPathListManager& p) : plm (p) {}
			virtual bool process(CncPathListRunner* plr) override;
			virtual void traceTo(std::ostream& o) const override;
		};
		
		// ---------------------------------------------------
		struct WorkflowCommandEntry : public WorkflowEntry
		{
			unsigned char*	buffer;
			int				bytes;
			
			WorkflowCommandEntry(const unsigned char* b, int len)
			: WorkflowEntry	()
			, buffer		(NULL)
			, bytes			(0)
			{
				// creates a deep copy!
				if ( b != NULL && len > 0 )
				{
					bytes = len;
					buffer = new unsigned char[bytes];
					memcpy(buffer, b, bytes);
				}
			}
			
			~WorkflowCommandEntry()
			{
				if ( buffer != NULL )
					delete [] buffer;
			}
			
			virtual bool process(CncPathListRunner* plr) override;
			virtual void traceTo(std::ostream& o) const override;
		};
		

		typedef std::vector<WorkflowEntry*> WorkFlowList;
		
		// ---------------------------------------------------
		struct Move
		{
			double dx;
			double dy;
			double dz;
			double mxy;
			double vxy;
			double mz;

			Move(double dx, double dy, double dz);
			explicit Move(const CncDoubleDistance& d);
			explicit Move(const CncPathListEntry* e);
			explicit Move(const Move& m);

			bool	isXYZPitchEqual(const Move& m) const;
			
			bool	isZPitchDiffTooStrong(const Move& m) const;
			bool	isXYPitchDiffTooStrong(const Move& m) const;
			bool	isXYZPitchDiffTooStrong(const Move& m) const;
			
			float	getXYPitchDiffenceAsRadians(const Move& m) const;
			float	getXYPitchDiffenceAsDegree(const Move& m) const;
			
			float	getZPitchDiffenceAsRadians(const Move& m) const;
			float	getZPitchDiffenceAsDegree(const Move& m) const;
			
			static	float maxXYPitchRadians;
			static	float maxZPitchRadians;
			
			static	float degree2Radians(float d)	{ return d * PI / 180.0; }
			static	float radians2Degree(float r)	{ return r * 180.0 / PI; }
			
			static	bool test();
		};
		
	public:
		
		// ---------------------------------------------------
		struct Setup
		{
			FileParser*	fileParser		= NULL;
			
			bool optAnalyse				= true;
			bool optSkipEmptyMoves		= true;
			bool optCombineMoves		= true;
			
			// for testing only
			bool trace					= true;
		};
		
		CncPathListRunner::Setup& getSetup() { return setup; }
		
		// ---------------------------------------------------
		class Interface
		{
			public: 
				
				enum Type { IT_FileStore, IT_Monitor, IT_CncControl };
				
			protected:
				
				Type type;
				
			public:
				
				Interface(Type t)
				: type (t)
				{}
				
				virtual ~Interface()
				{}
				
				virtual CncLongPosition		getCurrentPositionSteps()  const							= 0;
				virtual CncDoublePosition	getCurrentPositionMetric() const							= 0;
				virtual void				logCurrentPositionMetric(double px, double py, double pz)	= 0;
				virtual void				logCurrentPositionMetric(const CncDoublePosition& pos)		= 0;

				virtual bool spoolInstructions()														= 0;
				virtual void resetInstructions()														= 0;
		
				virtual void logMeasurementStart()														= 0;
				virtual void logMeasurementEnd()														= 0;
				virtual bool isInterrupted()															= 0;
				
				virtual bool processGuidePath(const CncPathListManager& plm)							= 0;
				virtual bool processClientIDChange(long cid)											= 0;
				virtual bool processFeedSpeedChange(double value_MM_MIN, CncSpeedMode m)				= 0;
				virtual bool processToolChange(int id)													= 0;
				virtual bool processSpindleStateSwitch(bool on, bool force=false)						= 0;
				virtual bool processSpindleSpeedChange(double value_U_MIN)								= 0;
				virtual bool processMoveSequence(CncMoveSequence& msq)									= 0;
				virtual bool processInitialEntry(const CncDoublePosition& p)							= 0;
				virtual bool processPathListEntry(const CncDoublePosition& p)							= 0;
				virtual bool processCommandEntry(const unsigned char* buffer, int bytes)				= 0;
				
				virtual void processTrigger(const Trigger::BeginRun& tr)								= 0;
				virtual void processTrigger(const Trigger::EndRun& tr)									= 0;
				virtual void processTrigger(const Trigger::NextPath& tr)								= 0;
				virtual void processTrigger(const Trigger::SpeedChange& tr)								= 0;
				virtual void processTrigger(const Trigger::GuidePath& tr)								= 0;
		};
		
		virtual void changePathListRunnerInterfaceImpl(const wxString& portName);
		
	private:
		
		bool installInterface(CncPathListRunner::Interface* iface);
		
		// ---------------------------------------------------
		WorkFlowList		workflowList;
		CncDoubleMatrix4x4	transformationMatrix;
		CncMoveSequence*	currentSequence;
		Interface*			currentInterface;
		Setup				setup;
		bool				initialMovementPerformed;

		
		bool isInterrupted();
		bool checkAndPerfromProcessingState();
		bool checkDebugState();
		
		void traceSetup();
		
		bool onPerfromClientIdChange(const CncPathListEntry& curr);
		bool onPerfromFeedSpeedChange(const CncPathListEntry& curr, const CncPathListEntry* next);
		bool onPerfromToolChange(const CncPathListEntry& curr);
		bool onPerfromSpindleChange(const CncPathListEntry& curr);
		bool onPerfromInitialMove(const CncPathListEntry& curr);
		bool onPerfromRawMove(const CncPathListEntry& curr);
		bool onPerfromAnalysedMove(CncPathList::const_iterator& it, const CncPathList::const_iterator& end);
		
		bool initializeNextMoveSequence(long nextClientId);
		bool initializeNextMoveSequence(double value_MM_MIN, char mode, long nextClientId);
		bool finalizeCurrMoveSequence(long nextClientId);
		bool addSequenceEntryFromAbsValues(double px, double py, double pz);
		bool addSequenceEntryFromEntry(const CncPathListEntry* e);
		
		// don't call this functions directly. use initializeNextMoveSequence() 
		// or finalizeCurrMoveSequence() instead
		bool destroyMoveSequence();
		bool publishMoveSequence();
		
		bool checkContent(const CncPathListEntry& curr);
		
		// ---------------------------------------------------
		// workflow interface
		void autoSetup(bool trace);
		bool publishGuidePath(const CncPathListManager& plm);
		bool publishCncPath(const CncPathListManager&plm);
		bool publishCommand(const unsigned char* buffer, int bytes);
		
		void executeTrigger(const Trigger::BeginRun& tr)	{ currentInterface->processTrigger(tr); }
		void executeTrigger(const Trigger::EndRun& tr)		{ currentInterface->processTrigger(tr); }
		void executeTrigger(const Trigger::NextPath& tr)	{ currentInterface->processTrigger(tr); }
		void executeTrigger(const Trigger::SpeedChange& tr)	{ currentInterface->processTrigger(tr); }
		void executeTrigger(const Trigger::GuidePath& tr)	{ currentInterface->processTrigger(tr); }
		
	protected:
		
		void setTranslation(const CncDoubleDistance& offset);
		
	public:
		
		explicit CncPathListRunner(CncControl* cnc);
		~CncPathListRunner();
		
		void logMeasurementStart();
		void logMeasurementEnd();
		
		bool spoolWorkflow();
		void resetWorkflow();
		void traceWorkflow(std::ostream& o);
		
		bool processGuidePath(const CncPathListManager& plm, double zOffset=0.0);
		bool processCncPath(const CncPathListManager& plm);
		
		bool processCommand(const unsigned char* buffer, int len);
		
		void processTrigger(const Trigger::BeginRun& tr)	{ resetWorkflow(); 
															  workflowList.push_back(new WorkflowTriggerBeginRunEntry   (tr)); }
		void processTrigger(const Trigger::EndRun& tr)		{ workflowList.push_back(new WorkflowTriggerEndRunEntry     (tr)); }
		void processTrigger(const Trigger::NextPath& tr)	{ workflowList.push_back(new WorkflowTriggerNextPathEntry   (tr)); }
		void processTrigger(const Trigger::SpeedChange& tr)	{ workflowList.push_back(new WorkflowTriggerSpeedChangeEntry(tr)); }
		void processTrigger(const Trigger::GuidePath& tr)	{ workflowList.push_back(new WorkflowTriggerGuidePathEntry  (tr)); }
		
		static bool test() { return Move::test(); }
};


#endif
