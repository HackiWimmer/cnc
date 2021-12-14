#ifndef CNC_PATH_LIST_INTERFACE_CNC
#define CNC_PATH_LIST_INTERFACE_CNC

#include "CncCommon.h"
#include "CncPosition.h"
#include "CncMoveSequence.h"
#include "CncPathListRunner.h"

// -------------------------------------------------------------------------------------
class CncControl;
class CncPathListInterfaceCnc : public CncPathListRunner::Interface {
	
	private:
		
		// ---------------------------------------------------
		struct CncInstruction
		{
			CncInstruction() {}
			virtual ~CncInstruction() {}

			virtual bool process(CncPathListInterfaceCnc* i)	= 0;
			virtual void traceTo(std::ostream& o) const			= 0;
		};
		
		// ---------------------------------------------------
		struct CncMovSeqInstruction : public CncInstruction
		{
			CncMoveSequenceImage img;
			
			explicit CncMovSeqInstruction(CncMoveSequence& s) : img (s) {}
			virtual bool process(CncPathListInterfaceCnc* i);
			virtual void traceTo(std::ostream& o) const;
		};
	
		struct CncPathListInstruction : public CncInstruction
		{
			CncPathListEntry ple;
			
			explicit CncPathListInstruction(const CncPathListEntry& e) : ple (e) {}
			virtual bool process(CncPathListInterfaceCnc* i);
			virtual void traceTo(std::ostream& o) const;
		};
		
		struct CncGuidPathInstruction : public CncInstruction
		{
			CncPathListManager	plm;
			
			explicit CncGuidPathInstruction(const CncPathListManager& m) : plm (m) {}
			virtual bool process(CncPathListInterfaceCnc* i);
			virtual void traceTo(std::ostream& o) const;
		};
		
		struct CncClientIDInstruction : public CncInstruction
		{
			long cid;
			
			explicit CncClientIDInstruction(long id) : cid (id) {}
			virtual bool process(CncPathListInterfaceCnc* i);
			virtual void traceTo(std::ostream& o) const;
		};
		
		// ---------------------------------------------------
		struct CncFeedSpeedInstruction : public CncInstruction
		{
			double 			value_MM_MIN;
			CncSpeedMode	mode;
			
			explicit CncFeedSpeedInstruction(double v, CncSpeedMode m) : value_MM_MIN (v), mode(m) {}
			virtual bool process(CncPathListInterfaceCnc* i);
			virtual void traceTo(std::ostream& o) const;
		};
		
		// ---------------------------------------------------
		struct CncToolChangeInstruction : public CncInstruction
		{
			double diameter;
			
			explicit CncToolChangeInstruction(double d) : diameter (d) {}
			virtual bool process(CncPathListInterfaceCnc* i);
			virtual void traceTo(std::ostream& o) const;
		};
		
		// ---------------------------------------------------
		struct CncSpindleStateInstruction : public CncInstruction
		{
			bool on;
			bool force;
			
			explicit CncSpindleStateInstruction(bool o, bool f) : on (o), force(f) {}
			virtual bool process(CncPathListInterfaceCnc* i);
			virtual void traceTo(std::ostream& o) const;
		};
		
		// ---------------------------------------------------
		struct CncSpindleSpeedInstruction : public CncInstruction
		{
			double value_U_MIN;
			
			explicit CncSpindleSpeedInstruction(double v) : value_U_MIN (v) {}
			virtual bool process(CncPathListInterfaceCnc* i);
			virtual void traceTo(std::ostream& o) const;
		};
		
		// ---------------------------------------------------
		struct InstructionTriggerBeginRun : public CncInstruction
		{
			Trigger::BeginRun tr;
			
			explicit InstructionTriggerBeginRun(const Trigger::BeginRun& t) : tr (t) {}
			virtual bool process(CncPathListInterfaceCnc* i);
			virtual void traceTo(std::ostream& o) const;
		};
		
		// ---------------------------------------------------
		struct InstructionTriggerEndRun : public CncInstruction
		{
			Trigger::EndRun tr;
			
			explicit InstructionTriggerEndRun(const Trigger::EndRun& t) : tr (t) {}
			virtual bool process(CncPathListInterfaceCnc* i);
			virtual void traceTo(std::ostream& o) const;
		};
		// ---------------------------------------------------
		struct InstructionTriggerNextPath : public CncInstruction
		{
			Trigger::NextPath tr;
			
			explicit InstructionTriggerNextPath(const Trigger::NextPath& t) : tr (t) {}
			virtual bool process(CncPathListInterfaceCnc* i);
			virtual void traceTo(std::ostream& o) const;
		};
		
		///////////////////////////////////////////////////////////////////
		struct InstructionTriggerSpeedChange : public CncInstruction
		{
			Trigger::SpeedChange tr;
			
			explicit InstructionTriggerSpeedChange(const Trigger::SpeedChange& t) : tr (t) {}
			virtual bool process(CncPathListInterfaceCnc* i);
			virtual void traceTo(std::ostream& o) const;
		};
		
		// ---------------------------------------------------
		struct InstructionTriggerGuidePath : public CncInstruction
		{
			Trigger::GuidePath tr;
			
			explicit InstructionTriggerGuidePath(const Trigger::GuidePath& t) : tr (t) {}
			virtual bool process(CncPathListInterfaceCnc* i);
			virtual void traceTo(std::ostream& o) const;
		};
		
		typedef std::vector<CncInstruction*> CncInstructionList;
		
		CncControl*			cnc;
		CncDoublePosition	currentAddPosition;
		CncInstructionList	cncInstructions;
		
		// ---------------------------------------------------
		// instruction interface
		bool executeGuidePath(const CncPathListManager& plm);
		bool executeClientIDChange(long cid);
		bool executeFeedSpeedChange(double value_MM_MIN, CncSpeedMode m);
		bool executeToolChange(double diameter);
		bool executeSpindleStateSwitch(bool on, bool force=false);
		bool executeSpindleSpeedChange(double value_U_MIN);
		bool executeMoveImage(const CncMoveSequenceImage& img);
		bool executeMoveSequence(CncMoveSequence& seq);
		bool executePathListEntry(const CncPathListEntry& ple);
		
		void executeTrigger(const Trigger::BeginRun& tr);
		void executeTrigger(const Trigger::EndRun& tr);
		void executeTrigger(const Trigger::NextPath& tr);
		void executeTrigger(const Trigger::SpeedChange& tr);
		void executeTrigger(const Trigger::GuidePath& tr);
	
	public:
		
		CncPathListInterfaceCnc(CncControl* cc);
		virtual ~CncPathListInterfaceCnc();
		
		virtual bool spoolInstructions();
		virtual void resetInstructions();
		
		virtual CncLongPosition		getCurrentPositionSteps()  const;
		virtual CncDoublePosition	getCurrentPositionMetric() const;
		virtual void				setCurrentPositionMetric(double px, double py, double pz);
		virtual void				setCurrentPositionMetric(const CncDoublePosition& pos);
		
		virtual void logMeasurementStart();
		virtual void logMeasurementEnd();
		virtual bool isInterrupted();
		
	#define XXXXX
	#ifdef XXXXX
	
		virtual bool processGuidePath(const CncPathListManager& plm)					{ cncInstructions.push_back(new CncGuidPathInstruction(plm));				return true; }
		virtual bool processClientIDChange(long cid)									{ cncInstructions.push_back(new CncClientIDInstruction(cid));				return true; }
		virtual bool processFeedSpeedChange(double value_MM_MIN, CncSpeedMode m)		{ cncInstructions.push_back(new CncFeedSpeedInstruction(value_MM_MIN, m));	return true; }
		virtual bool processToolChange(double diameter)									{ cncInstructions.push_back(new CncToolChangeInstruction(diameter));		return true; }
		virtual bool processSpindleStateSwitch(bool on, bool force)						{ cncInstructions.push_back(new CncSpindleStateInstruction(on, force));		return true; }
		virtual bool processSpindleSpeedChange(double value_U_MIN)						{ cncInstructions.push_back(new CncSpindleSpeedInstruction(value_U_MIN));	return true; }
		virtual bool processMoveSequence(CncMoveSequence& msq)							{ cncInstructions.push_back(new CncMovSeqInstruction(msq));					return true; }
		virtual bool processPathListEntry(const CncPathListEntry& ple)					{ cncInstructions.push_back(new CncPathListInstruction(ple));				return true; }
	
	#else
	
		virtual bool processGuidePath(const CncPathListManager& plm, double zOffset)	{ return executeGuidePath(plm, zOffset); }
		virtual bool processClientIDChange(long cid)									{ return executeClientIDChange( cid); }
		virtual bool processFeedSpeedChange(double value_MM_MIN, CncSpeedMode m)		{ return executeFeedSpeedChange(value_MM_MIN, m); }
		virtual bool processToolChange(double diameter)									{ return executeToolChange(diameter); }
		virtual bool processSpindleStateSwitch(bool on, bool force)						{ return executeSpindleStateSwitch(on, force); }
		virtual bool processSpindleSpeedChange(double value_U_MIN)						{ return executeSpindleSpeedChange(value_U_MIN); }
		virtual bool processMoveSequence(CncMoveSequence& msq)							{ return executeMoveSequence(msq); }
		virtual bool processPathListEntry(const CncPathListEntry& ple)					{ return executePathListEntry(ple); }
	
	#endif
		virtual void processTrigger(const Trigger::BeginRun& tr)						{ cncInstructions.push_back(new InstructionTriggerBeginRun     (tr)); }
		virtual void processTrigger(const Trigger::EndRun& tr)							{ cncInstructions.push_back(new InstructionTriggerEndRun       (tr)); }
		virtual void processTrigger(const Trigger::NextPath& tr)						{ cncInstructions.push_back(new InstructionTriggerNextPath     (tr)); }
		virtual void processTrigger(const Trigger::SpeedChange& tr)						{ cncInstructions.push_back(new InstructionTriggerSpeedChange  (tr)); }
		virtual void processTrigger(const Trigger::GuidePath& tr)						{ cncInstructions.push_back(new InstructionTriggerGuidePath    (tr)); }
};

#endif