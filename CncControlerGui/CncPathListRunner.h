#ifndef CNC_PATH_LIST_RUNNER_H
#define CNC_PATH_LIST_RUNNER_H

#include "CncControl.h"
#include "CncPathListManager.h"

class CncMoveSequence;
class FileParser;
class CncPathListRunner;

///////////////////////////////////////////////////////////////////
struct WorflowEntry
{
	WorflowEntry() {}
	virtual ~WorflowEntry() {}
	virtual bool process(CncPathListRunner* plr) = 0;
};
///////////////////////////////////////////////////////////////////
struct WorflowSetupRunEntry : public WorflowEntry
{
	bool trace;
	explicit WorflowSetupRunEntry(bool t) : trace (t) {}
	virtual bool process(CncPathListRunner* plr);
};
///////////////////////////////////////////////////////////////////
struct WorflowTriggerBeginRunEntry : public WorflowEntry
{
	Trigger::BeginRun tr;
	
	explicit WorflowTriggerBeginRunEntry(const Trigger::BeginRun& t) : tr (t) {}
	virtual bool process(CncPathListRunner* plr);
};
///////////////////////////////////////////////////////////////////
struct WorflowTriggerEndRunEntry : public WorflowEntry
{
	Trigger::EndRun tr;
	
	explicit WorflowTriggerEndRunEntry(const Trigger::EndRun& t) : tr (t) {}
	virtual bool process(CncPathListRunner* plr);
};
///////////////////////////////////////////////////////////////////
struct WorflowTriggerNextPathEntry : public WorflowEntry
{
	Trigger::NextPath tr;
	
	explicit WorflowTriggerNextPathEntry(const Trigger::NextPath& t) : tr (t) {}
	virtual bool process(CncPathListRunner* plr);
};
///////////////////////////////////////////////////////////////////
struct WorflowTriggerSpeedChangeEntry : public WorflowEntry
{
	Trigger::SpeedChange tr;
	
	explicit WorflowTriggerSpeedChangeEntry(const Trigger::SpeedChange& t) : tr (t) {}
	virtual bool process(CncPathListRunner* plr);
};
///////////////////////////////////////////////////////////////////
struct WorflowTriggerGuidePtahEntry : public WorflowEntry
{
	Trigger::GuidePath tr;
	
	explicit WorflowTriggerGuidePtahEntry(const Trigger::GuidePath& t) : tr (t) {}
	virtual bool process(CncPathListRunner* plr);
};
///////////////////////////////////////////////////////////////////
struct WorflowCncEntry : public WorflowEntry
{
	CncPathListManager plm;
	
	explicit WorflowCncEntry(const CncPathListManager& p) : plm (p) {}
	virtual bool process(CncPathListRunner* plr);
};
///////////////////////////////////////////////////////////////////
struct WorflowGuideEntry : public WorflowEntry
{
	CncPathListManager plm;
	double zOffset = 0.0;
	
	explicit WorflowGuideEntry(const CncPathListManager& p, double z) : plm (p), zOffset(z) {}
	virtual bool process(CncPathListRunner* plr);
};

typedef std::vector<WorflowEntry*> WorkFlowList;

///////////////////////////////////////////////////////////////////
class CncPathListRunner {
	
	private:
		
		struct Move {
			
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
		struct Setup {
			
			FileParser*	fileParser		= NULL;
			
			bool optAnalyse				= true;
			bool optSkipEmptyMoves		= true;
			bool optCombineMoves		= true;
			
			// for testing only
			bool trace					= true;
		};
		
		CncPathListRunner::Setup& getSetup() { return setup; }
		
		// ---------------------------------------------------
		class Interface {
			
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
				
				virtual CncLongPosition   getPositionSteps()  const								= 0;
				virtual CncDoublePosition getPositionMetric() const								= 0;
				
				virtual void logMeasurementStart()												= 0;
				virtual void logMeasurementEnd()												= 0;
				virtual bool isInterrupted()													= 0;
				
				virtual void publishGuidePath(const CncPathListManager& plm, double zOffset)	= 0;
				
				virtual void processClientIDChange(long cid)									= 0;
				virtual bool processFeedSpeedChange(double value_MM_MIN, CncSpeedMode m)		= 0;
				virtual bool processToolChange(double diameter)									= 0;
				virtual bool processSpindleStateSwitch(bool on)									= 0;
				virtual bool processSpindleSpeedChange(double value_U_MIN)						= 0;
				virtual bool processMoveSequence(CncMoveSequence& msq)							= 0;
				virtual bool processPathListEntry(const CncPathListEntry& ple)					= 0;
				
				virtual void processTrigger(const Trigger::BeginRun& tr)						= 0;
				virtual void processTrigger(const Trigger::EndRun& tr)							= 0;
				virtual void processTrigger(const Trigger::NextPath& tr)						= 0;
				virtual void processTrigger(const Trigger::SpeedChange& tr)						= 0;
				virtual void processTrigger(const Trigger::GuidePath& tr)						= 0;
		};
		
		virtual void changePathListRunnerInterfaceImpl(const wxString& portName);
		
	private:
		
		bool installInterface(CncPathListRunner::Interface* iface);

		// ---------------------------------------------------
		class CncCtrl : public Interface {
			
			private:
				
				CncControl* cnc;
			
			public:
				
				CncCtrl(CncControl* cc)
				: Interface(IT_CncControl)
				, cnc(cc)
				{ wxASSERT ( cnc != NULL ); }
				
				virtual ~CncCtrl()
				{}
				
				virtual CncLongPosition   getPositionSteps()  const								{ return cnc->getCurCtlPos();       }
				virtual CncDoublePosition getPositionMetric() const								{ return cnc->getCurCtlPosMetric(); }
				
				virtual void logMeasurementStart()												{ cnc->startSerialMeasurement(); }
				virtual void logMeasurementEnd()												{ cnc->stopSerialMeasurement();  }
				virtual bool isInterrupted()													{ return cnc->isInterrupted();   }
				
				virtual void publishGuidePath(const CncPathListManager& plm, double zOffset)	{ cnc->addGuidePath(plm, zOffset); }
				
				virtual void processClientIDChange(long cid)									{ cnc->setClientId(cid); }
				virtual bool processFeedSpeedChange(double value_MM_MIN, CncSpeedMode m)		{ return cnc->changeCurrentFeedSpeedXYZ_MM_MIN(value_MM_MIN, m); }
				virtual bool processToolChange(double diameter)									{ return true; }
				virtual bool processSpindleStateSwitch(bool on)									{ return cnc->switchSpindleState(on); }
				virtual bool processSpindleSpeedChange(double value_U_MIN)						{ return cnc->changeCurrentSpindleSpeed_U_MIN(value_U_MIN); }
				virtual bool processMoveSequence(CncMoveSequence& msq)							{ return cnc->processMoveSequence(msq); }
				virtual bool processPathListEntry(const CncPathListEntry& ple)					{ return cnc->moveAbsLinearMetricXYZ(	ple.entryTarget.getX(),
																																		ple.entryTarget.getY(),
																																		ple.entryTarget.getZ(),
																																		ple.alreadyRendered); }
																																		
				virtual void processTrigger(const Trigger::BeginRun& tr)						{ cnc->processTrigger(tr); }
				virtual void processTrigger(const Trigger::EndRun& tr)							{ cnc->processTrigger(tr); }
				virtual void processTrigger(const Trigger::NextPath& tr)						{ cnc->processTrigger(tr); }
				virtual void processTrigger(const Trigger::SpeedChange& tr)						{ cnc->processTrigger(tr); }
				virtual void processTrigger(const Trigger::GuidePath& tr)						{ cnc->processTrigger(tr); }
		};
		
		// ---------------------------------------------------
		WorkFlowList		workflowList;
		CncMoveSequence*	currentSequence;
		Interface*			currentInterface;
		Setup				setup;
		
		bool isInterrupted();
		bool checkAndPerfromProcessingState();
		bool checkDebugState();
		
		void traceSetup();
		
		bool onPhysicallyClientIdChange(const CncPathListEntry& curr);
		bool onPhysicallyFeedSpeedChange(const CncPathListEntry& curr, const CncPathListEntry* next);
		bool onPhysicallySpindleChange(const CncPathListEntry& curr);
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
		
		bool checkContent(const CncPathListEntry& curr);
		




		void autoSetup(bool trace);
		bool publishGuidePath(const CncPathListManager& plm, double zOffset=0.0);
		bool publishCncPath(const CncPathListManager&plm);
		
		void executeTrigger(const Trigger::BeginRun& tr)	{ currentInterface->processTrigger(tr); }
		void executeTrigger(const Trigger::EndRun& tr)		{ currentInterface->processTrigger(tr); }
		void executeTrigger(const Trigger::NextPath& tr)	{ currentInterface->processTrigger(tr); }
		void executeTrigger(const Trigger::SpeedChange& tr)	{ currentInterface->processTrigger(tr); }
		void executeTrigger(const Trigger::GuidePath& tr)	{ currentInterface->processTrigger(tr); }
		
		friend WorflowSetupRunEntry;
		friend WorflowTriggerBeginRunEntry;
		friend WorflowTriggerEndRunEntry;
		friend WorflowTriggerNextPathEntry;
		friend WorflowTriggerSpeedChangeEntry;
		friend WorflowTriggerGuidePtahEntry;
		friend WorflowCncEntry;
		friend WorflowGuideEntry;
		
	public:
		
		explicit CncPathListRunner(CncControl* cnc);
		~CncPathListRunner();
		
		void logMeasurementStart();
		void logMeasurementEnd();
		
		bool spoolWorkflow();
		void resetWorkflow();
		
		bool processGuidePath(const CncPathListManager& plm, double zOffset=0.0)
		{ workflowList.push_back(new WorflowGuideEntry(plm, zOffset)); return true; }
		
		bool processCncPath(const CncPathListManager& plm)
		{ workflowList.push_back(new WorflowCncEntry(plm)); return true; }
		
		void processTrigger(const Trigger::BeginRun& tr)	{ workflowList.push_back(new WorflowTriggerBeginRunEntry   (tr)); }
		void processTrigger(const Trigger::EndRun& tr)		{ workflowList.push_back(new WorflowTriggerEndRunEntry     (tr)); }
		void processTrigger(const Trigger::NextPath& tr)	{ workflowList.push_back(new WorflowTriggerNextPathEntry   (tr)); }
		void processTrigger(const Trigger::SpeedChange& tr)	{ workflowList.push_back(new WorflowTriggerSpeedChangeEntry(tr)); }
		void processTrigger(const Trigger::GuidePath& tr)	{ workflowList.push_back(new WorflowTriggerGuidePtahEntry  (tr)); }
		
		static bool test() { return Move::test(); }
};


#endif
