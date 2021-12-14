#include "CncControl.h"
#include "CncControl.h"
#include "MainFrame.h"
#include "CncPathListInterfaceCnc.h"

////////////////////////////////////////////////////////////////////

bool CncPathListInterfaceCnc::CncMovSeqInstruction				::process(CncPathListInterfaceCnc* i)	{ return i->executeMoveImage(img); }
bool CncPathListInterfaceCnc::CncPathListInstruction			::process(CncPathListInterfaceCnc* i)	{ return i->executePathListEntry(ple); }
bool CncPathListInterfaceCnc::CncGuidPathInstruction			::process(CncPathListInterfaceCnc* i)	{ return i->executeGuidePath(plm); }
bool CncPathListInterfaceCnc::CncClientIDInstruction			::process(CncPathListInterfaceCnc* i)	{ return i->executeClientIDChange(cid); }
bool CncPathListInterfaceCnc::CncFeedSpeedInstruction			::process(CncPathListInterfaceCnc* i)	{ return i->executeFeedSpeedChange(value_MM_MIN, mode); }
bool CncPathListInterfaceCnc::CncToolChangeInstruction			::process(CncPathListInterfaceCnc* i)	{ return i->executeToolChange(diameter); }
bool CncPathListInterfaceCnc::CncSpindleStateInstruction		::process(CncPathListInterfaceCnc* i)	{ return i->executeSpindleStateSwitch(on, force); }
bool CncPathListInterfaceCnc::CncSpindleSpeedInstruction		::process(CncPathListInterfaceCnc* i)	{ return i->executeSpindleSpeedChange(value_U_MIN); }
bool CncPathListInterfaceCnc::InstructionTriggerBeginRun		::process(CncPathListInterfaceCnc* i)	{ i->executeTrigger(tr); return true; }
bool CncPathListInterfaceCnc::InstructionTriggerEndRun			::process(CncPathListInterfaceCnc* i)	{ i->executeTrigger(tr); return true; }
bool CncPathListInterfaceCnc::InstructionTriggerNextPath		::process(CncPathListInterfaceCnc* i)	{ i->executeTrigger(tr); return true; }
bool CncPathListInterfaceCnc::InstructionTriggerSpeedChange		::process(CncPathListInterfaceCnc* i)	{ i->executeTrigger(tr); return true; }
bool CncPathListInterfaceCnc::InstructionTriggerGuidePath		::process(CncPathListInterfaceCnc* i)	{ i->executeTrigger(tr); return true; }

#warning todo
void CncPathListInterfaceCnc::CncMovSeqInstruction				::traceTo(std::ostream& o) const		{ o << "MovSeq"							<< std::endl; } // todo
void CncPathListInterfaceCnc::CncPathListInstruction			::traceTo(std::ostream& o) const		{ o << "PathList"						<< std::endl; } // todo
void CncPathListInterfaceCnc::CncGuidPathInstruction			::traceTo(std::ostream& o) const		{ o << "Guide"							<< std::endl; } // todo
void CncPathListInterfaceCnc::CncClientIDInstruction			::traceTo(std::ostream& o) const		{ o << "ClientID(" << cid << ")"		<< std::endl; }
void CncPathListInterfaceCnc::CncFeedSpeedInstruction			::traceTo(std::ostream& o) const		{ o << "F(" << value_MM_MIN << ")"		<< std::endl; }
void CncPathListInterfaceCnc::CncToolChangeInstruction			::traceTo(std::ostream& o) const		{ o << "ToolChange(" << diameter << ")"	<< std::endl; }
void CncPathListInterfaceCnc::CncSpindleStateInstruction		::traceTo(std::ostream& o) const		{ o << "Spindle(" << on << ")"			<< std::endl; }
void CncPathListInterfaceCnc::CncSpindleSpeedInstruction		::traceTo(std::ostream& o) const		{ o << "S(" << value_U_MIN << ")"		<< std::endl; }
void CncPathListInterfaceCnc::InstructionTriggerBeginRun		::traceTo(std::ostream& o) const		{ o << tr 								<< std::endl; }
void CncPathListInterfaceCnc::InstructionTriggerEndRun			::traceTo(std::ostream& o) const		{ o << tr 								<< std::endl; }
void CncPathListInterfaceCnc::InstructionTriggerNextPath		::traceTo(std::ostream& o) const		{ o << tr 								<< std::endl; }
void CncPathListInterfaceCnc::InstructionTriggerSpeedChange		::traceTo(std::ostream& o) const		{ o << tr 								<< std::endl; }
void CncPathListInterfaceCnc::InstructionTriggerGuidePath		::traceTo(std::ostream& o) const		{ o << tr 								<< std::endl; }

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
CncPathListInterfaceCnc::CncPathListInterfaceCnc(CncControl* cc)
: Interface				(IT_CncControl)
, cnc					(cc)
, currentAddPosition	(0.0, 0.0, 0.0)
, cncInstructions		()
////////////////////////////////////////////////////////////////////
{ 
	wxASSERT ( cnc != NULL ); 
	cncInstructions.reserve(1024);
}
////////////////////////////////////////////////////////////////////
CncPathListInterfaceCnc::~CncPathListInterfaceCnc() {
////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////

void CncPathListInterfaceCnc::logMeasurementStart()												{ cnc->startSerialMeasurement(); }
void CncPathListInterfaceCnc::logMeasurementEnd()												{ cnc->stopSerialMeasurement(); }
bool CncPathListInterfaceCnc::isInterrupted()													{ return cnc->isInterrupted(); }
bool CncPathListInterfaceCnc::executeClientIDChange(long cid)									{ cnc->setClientId(cid); return true; }
bool CncPathListInterfaceCnc::executeToolChange(double diameter)								{ return true; }
bool CncPathListInterfaceCnc::executeSpindleStateSwitch(bool on, bool force)					{ return cnc->switchSpindleState(on, force); }
bool CncPathListInterfaceCnc::executeSpindleSpeedChange(double value_U_MIN)						{ return cnc->changeCurrentSpindleSpeed_U_MIN(value_U_MIN); }
bool CncPathListInterfaceCnc::executeFeedSpeedChange(double value_MM_MIN, CncSpeedMode m)		{ return cnc->changeCurrentFeedSpeedXYZ_MM_MIN(value_MM_MIN, m); }
bool CncPathListInterfaceCnc::executeMoveSequence(CncMoveSequence& seq)							{ return cnc->processMoveSequence(seq); }
bool CncPathListInterfaceCnc::executeMoveImage(const CncMoveSequenceImage& img)					{ return cnc->processMoveImage(img); }

void CncPathListInterfaceCnc::executeTrigger(const Trigger::BeginRun& tr)						{ cnc->processTrigger(tr); }
void CncPathListInterfaceCnc::executeTrigger(const Trigger::EndRun& tr)							{ cnc->processTrigger(tr); }
void CncPathListInterfaceCnc::executeTrigger(const Trigger::NextPath& tr)						{ cnc->processTrigger(tr); }
void CncPathListInterfaceCnc::executeTrigger(const Trigger::SpeedChange& tr)					{ cnc->processTrigger(tr); }
void CncPathListInterfaceCnc::executeTrigger(const Trigger::GuidePath& tr)						{ cnc->processTrigger(tr); }

bool CncPathListInterfaceCnc::executeGuidePath(const CncPathListManager& plm)					{ cnc->addGuidePath(plm); return true; }

void CncPathListInterfaceCnc::setCurrentPositionMetric(double px, double py, double pz)			{ currentAddPosition.setXYZ(px, py, pz); }
void CncPathListInterfaceCnc::setCurrentPositionMetric(const CncDoublePosition& pos)			{ currentAddPosition.set(pos); }
CncDoublePosition CncPathListInterfaceCnc::getCurrentPositionMetric() const						{ return currentAddPosition; }
CncLongPosition CncPathListInterfaceCnc::getCurrentPositionSteps() const						{ CncLongPosition ret; return THE_CONFIG->convertMetricToSteps(ret, currentAddPosition); }

////////////////////////////////////////////////////////////////////
bool CncPathListInterfaceCnc::spoolInstructions() {
////////////////////////////////////////////////////////////////////
	CNC_CEX2_A("Start processing cnc instructions (entries=%zu)", cncInstructions.size())
	
	for ( auto instruction : cncInstructions )
	{
		if ( instruction->process(this) == false )
		{
			std::stringstream ss; instruction->traceTo(ss);
			CNC_CERR_FUNCT_A("Error while processing: %s", ss.str().c_str())
			return false;
		}
		
		THE_APP->evaluateAndPerformProcessingState();
	}
	
	return true;
}
////////////////////////////////////////////////////////////////////
void CncPathListInterfaceCnc::resetInstructions() {
////////////////////////////////////////////////////////////////////
	for ( auto instruction : cncInstructions )
		delete instruction;
		
	cncInstructions.clear();
}
////////////////////////////////////////////////////////////////////
bool CncPathListInterfaceCnc::executePathListEntry(const CncPathListEntry& ple) { 
////////////////////////////////////////////////////////////////////
	return cnc->moveAbsLinearMetricXYZ(	ple.entryTarget.getX(),
										ple.entryTarget.getY(),
										ple.entryTarget.getZ(),
										ple.alreadyRendered
	); 
}
