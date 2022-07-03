#ifndef CNC_PATH_LIST_INTERFACE_CNC
#define CNC_PATH_LIST_INTERFACE_CNC

#include "CncCommon.h"
#include "CncArduino.h"
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
		
		struct CncCommandInstruction : public CncInstruction
		{
			unsigned char*	buffer;
			int				bytes;
			
			CncCommandInstruction(const unsigned char* b, int len)
			: CncInstruction	()
			, buffer			(NULL)
			, bytes				(0)
			{
				// creates a deep copy!
				if ( b != NULL && len > 0 )
				{
					bytes = len;
					buffer = new unsigned char[bytes];
					memcpy(buffer, b, bytes);
				}
			}
			
			~CncCommandInstruction()
			{
				if ( buffer != NULL )
					delete [] buffer;
			}
			
			virtual bool process(CncPathListInterfaceCnc* i);
			virtual void traceTo(std::ostream& o) const
			{
				o	<< "  Command:"
					<< " type = " 
					<< std::left  << std::setw(30) << std::setfill(' ') << ( bytes > 0 ?  ArduinoCMDs::getCMDLabel(buffer[0]) : "???" ) 
					<< " size = "
					<< std::right << std::setw( 3) << std::setfill('0') << bytes
					<< std::endl
				; 
			}
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
			bool state;
			bool force;
			
			explicit CncSpindleStateInstruction(bool s, bool f) : state (s), force(f) {}
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
		bool executeSpindleStateSwitch(CncSpindlePowerState state, bool force=false);
		bool executeSpindleSpeedChange(double value_U_MIN);
		bool executeMoveImage(const CncMoveSequenceImage& img);
		bool executeMoveSequence(CncMoveSequence& seq);
		bool executePathListEntry(const CncPathListEntry& ple);
		bool executeCommand(const unsigned char* buffer, int bytes);
		
		void executeTrigger(const Trigger::BeginRun& tr);
		void executeTrigger(const Trigger::EndRun& tr);
		void executeTrigger(const Trigger::NextPath& tr);
		void executeTrigger(const Trigger::SpeedChange& tr);
		void executeTrigger(const Trigger::GuidePath& tr);
	
		bool spoolInstructions(CncInstructionList* ci);
		
	public:
		
		CncPathListInterfaceCnc(CncControl* cc);
		virtual ~CncPathListInterfaceCnc();
		
		virtual bool spoolInstructions();
		virtual void resetInstructions();
		
		virtual CncLongPosition		getCurrentPositionSteps()									const	override;
		virtual CncDoublePosition	getCurrentPositionMetric()									const	override;
		virtual void				setCurrentPositionMetric(double px, double py, double pz)			override;
		virtual void				setCurrentPositionMetric(const CncDoublePosition& pos)				override;
		
		virtual void logMeasurementStart();
		virtual void logMeasurementEnd();
		virtual bool isInterrupted();
		
	#define CNC_USE_INSTRUCTIONS
	#ifdef CNC_USE_INSTRUCTIONS
	
		virtual bool processGuidePath(const CncPathListManager& plm)					override { cncInstructions.push_back(new CncGuidPathInstruction(plm));				return true; }
		virtual bool processClientIDChange(long cid)									override { cncInstructions.push_back(new CncClientIDInstruction(cid));				return true; }
		virtual bool processFeedSpeedChange(double value_MM_MIN, CncSpeedMode m)		override { cncInstructions.push_back(new CncFeedSpeedInstruction(value_MM_MIN, m));	return true; }
		virtual bool processToolChange(double diameter)									override { cncInstructions.push_back(new CncToolChangeInstruction(diameter));		return true; }
		virtual bool processSpindleStateSwitch(CncSpindlePowerState state, bool force)	override { cncInstructions.push_back(new CncSpindleStateInstruction(state, force));	return true; }
		virtual bool processSpindleSpeedChange(double value_U_MIN)						override { cncInstructions.push_back(new CncSpindleSpeedInstruction(value_U_MIN));	return true; }
		virtual bool processMoveSequence(CncMoveSequence& msq)							override { cncInstructions.push_back(new CncMovSeqInstruction(msq));				return true; }
		virtual bool processPathListEntry(const CncPathListEntry& ple)					override { cncInstructions.push_back(new CncPathListInstruction(ple));				return true; }
		virtual bool processCommandEntry(const unsigned char* buffer, int bytes)		override { cncInstructions.push_back(new CncCommandInstruction(buffer, bytes));		return true; }
	
	#else
	
		virtual bool processGuidePath(const CncPathListManager& plm, double zOffset)	override { return executeGuidePath(plm, zOffset); }
		virtual bool processClientIDChange(long cid)									override { return executeClientIDChange( cid); }
		virtual bool processFeedSpeedChange(double value_MM_MIN, CncSpeedMode m)		override { return executeFeedSpeedChange(value_MM_MIN, m); }
		virtual bool processToolChange(double diameter)									override { return executeToolChange(diameter); }
		virtual bool processSpindleStateSwitch(CncSpindlePowerState state, bool force)	override { return executeSpindleStateSwitch(state, force); }
		virtual bool processSpindleSpeedChange(double value_U_MIN)						override { return executeSpindleSpeedChange(value_U_MIN); }
		virtual bool processMoveSequence(CncMoveSequence& msq)							override { return executeMoveSequence(msq); }
		virtual bool processPathListEntry(const CncPathListEntry& ple)					override { return executePathListEntry(ple); }
		virtual bool processCommandEntry(const unsigned char* buffer, int bytes)		override { return executeCommandEntry(buffer, bytes); }
	
	#endif
	
		virtual void processTrigger(const Trigger::BeginRun& tr)						override;
		virtual void processTrigger(const Trigger::EndRun& tr)							override;
		virtual void processTrigger(const Trigger::NextPath& tr)						override { cncInstructions.push_back(new InstructionTriggerNextPath     (tr)); }
		virtual void processTrigger(const Trigger::SpeedChange& tr)						override { cncInstructions.push_back(new InstructionTriggerSpeedChange  (tr)); }
		virtual void processTrigger(const Trigger::GuidePath& tr)						override { cncInstructions.push_back(new InstructionTriggerGuidePath    (tr)); }
};

#endif