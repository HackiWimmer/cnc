#include "CncControl.h"
#include "CncPathListInterfaceCnc.h"

////////////////////////////////////////////////////////////////////

bool CncPathListInterfaceCnc::CncMovSeqInstruction				::process(CncPathListInterfaceCnc* i)	{ return i->executeMoveImage(img); }
bool CncPathListInterfaceCnc::CncPathListInstruction			::process(CncPathListInterfaceCnc* i)	{ return i->executePathListEntry(ple); }
bool CncPathListInterfaceCnc::CncGuidPathInstruction			::process(CncPathListInterfaceCnc* i)	{ return i->executeGuidePath(plm, zOffset); }
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
void CncPathListInterfaceCnc::CncMovSeqInstruction				::traceTo(std::ostream& o) const		{ ; } // todo
void CncPathListInterfaceCnc::CncPathListInstruction			::traceTo(std::ostream& o) const		{ ; }
void CncPathListInterfaceCnc::CncGuidPathInstruction			::traceTo(std::ostream& o) const		{ ; }
void CncPathListInterfaceCnc::CncClientIDInstruction			::traceTo(std::ostream& o) const		{ ; }
void CncPathListInterfaceCnc::CncFeedSpeedInstruction			::traceTo(std::ostream& o) const		{ ; }
void CncPathListInterfaceCnc::CncToolChangeInstruction			::traceTo(std::ostream& o) const		{ ; }
void CncPathListInterfaceCnc::CncSpindleStateInstruction		::traceTo(std::ostream& o) const		{ ; }
void CncPathListInterfaceCnc::CncSpindleSpeedInstruction		::traceTo(std::ostream& o) const		{ ; }
void CncPathListInterfaceCnc::InstructionTriggerBeginRun		::traceTo(std::ostream& o) const		{ o << tr 			<< std::endl; }
void CncPathListInterfaceCnc::InstructionTriggerEndRun			::traceTo(std::ostream& o) const		{ o << tr 			<< std::endl; }
void CncPathListInterfaceCnc::InstructionTriggerNextPath		::traceTo(std::ostream& o) const		{ o << tr 			<< std::endl; }
void CncPathListInterfaceCnc::InstructionTriggerSpeedChange		::traceTo(std::ostream& o) const		{ o << tr 			<< std::endl; }
void CncPathListInterfaceCnc::InstructionTriggerGuidePath		::traceTo(std::ostream& o) const		{ o << tr 			<< std::endl; }

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
CncPathListInterfaceCnc::CncPathListInterfaceCnc(CncControl* cc)
: Interface			(IT_CncControl)
, cnc				(cc)
, cncInstructions	()
////////////////////////////////////////////////////////////////////
{ 
	wxASSERT ( cnc != NULL ); 
}
////////////////////////////////////////////////////////////////////
CncPathListInterfaceCnc::~CncPathListInterfaceCnc() {
////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////
bool CncPathListInterfaceCnc::spoolInstructions() {
////////////////////////////////////////////////////////////////////
	CNC_CEX2_A("Start processing cnc instructions(entries=%zu)", cncInstructions.size())
	
	for ( auto instruction : cncInstructions )
	{
		if ( instruction->process(this) == false )
		{
			CNC_CERR_FUNCT_A("Error while processing: .... ")
			return false;
		}
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
CncLongPosition CncPathListInterfaceCnc::getPositionSteps()  const { 
////////////////////////////////////////////////////////////////////
	return cnc->getCurCtlPos(); 
}
////////////////////////////////////////////////////////////////////
CncDoublePosition CncPathListInterfaceCnc::getPositionMetric() const { 
////////////////////////////////////////////////////////////////////
	return cnc->getCurCtlPosMetric(); 
}
////////////////////////////////////////////////////////////////////
void CncPathListInterfaceCnc::logMeasurementStart() { 
////////////////////////////////////////////////////////////////////
	cnc->startSerialMeasurement(); 
}
////////////////////////////////////////////////////////////////////
void CncPathListInterfaceCnc::logMeasurementEnd() { 
////////////////////////////////////////////////////////////////////
	cnc->stopSerialMeasurement();  
}
////////////////////////////////////////////////////////////////////
bool CncPathListInterfaceCnc::isInterrupted() { 
////////////////////////////////////////////////////////////////////
	return cnc->isInterrupted();  
}
////////////////////////////////////////////////////////////////////
bool CncPathListInterfaceCnc::executeGuidePath(const CncPathListManager& plm, double zOffset) { 
////////////////////////////////////////////////////////////////////
	cnc->addGuidePath(plm, zOffset);
	return true;
}
////////////////////////////////////////////////////////////////////
bool CncPathListInterfaceCnc::executeClientIDChange(long cid) { 
////////////////////////////////////////////////////////////////////
	cnc->setClientId(cid);
	return true;
}
////////////////////////////////////////////////////////////////////
bool CncPathListInterfaceCnc::executeFeedSpeedChange(double value_MM_MIN, CncSpeedMode m) { 
////////////////////////////////////////////////////////////////////
	return cnc->changeCurrentFeedSpeedXYZ_MM_MIN(value_MM_MIN, m); 
}
////////////////////////////////////////////////////////////////////
bool CncPathListInterfaceCnc::executeToolChange(double diameter) { 
////////////////////////////////////////////////////////////////////
	return true; 
}
////////////////////////////////////////////////////////////////////
bool CncPathListInterfaceCnc::executeSpindleStateSwitch(bool on, bool force) { 
////////////////////////////////////////////////////////////////////
	return cnc->switchSpindleState(on, force); 
}
////////////////////////////////////////////////////////////////////
bool CncPathListInterfaceCnc::executeSpindleSpeedChange(double value_U_MIN) { 
////////////////////////////////////////////////////////////////////
	return cnc->changeCurrentSpindleSpeed_U_MIN(value_U_MIN); 
}
////////////////////////////////////////////////////////////////////
bool CncPathListInterfaceCnc::executeMoveSequence(CncMoveSequence& seq) { 
////////////////////////////////////////////////////////////////////
	return cnc->processMoveSequence(seq); 
}
////////////////////////////////////////////////////////////////////
bool CncPathListInterfaceCnc::executeMoveImage(const CncMoveSequenceImage& img) { 
////////////////////////////////////////////////////////////////////
	return cnc->processMoveImage(img); 
}
////////////////////////////////////////////////////////////////////
bool CncPathListInterfaceCnc::executePathListEntry(const CncPathListEntry& ple) { 
////////////////////////////////////////////////////////////////////
	return cnc->moveAbsLinearMetricXYZ(	ple.entryTarget.getX(),
										ple.entryTarget.getY(),
										ple.entryTarget.getZ(),
										ple.alreadyRendered); 
}
////////////////////////////////////////////////////////////////////
void CncPathListInterfaceCnc::executeTrigger(const Trigger::BeginRun& tr) { 
////////////////////////////////////////////////////////////////////
	cnc->processTrigger(tr); 
}
////////////////////////////////////////////////////////////////////
void CncPathListInterfaceCnc::executeTrigger(const Trigger::EndRun& tr) { 
////////////////////////////////////////////////////////////////////
	cnc->processTrigger(tr); 
}
////////////////////////////////////////////////////////////////////
void CncPathListInterfaceCnc::executeTrigger(const Trigger::NextPath& tr) { 
////////////////////////////////////////////////////////////////////
	cnc->processTrigger(tr); 
}
////////////////////////////////////////////////////////////////////
void CncPathListInterfaceCnc::executeTrigger(const Trigger::SpeedChange& tr) { 
////////////////////////////////////////////////////////////////////
	cnc->processTrigger(tr); 
}
////////////////////////////////////////////////////////////////////
void CncPathListInterfaceCnc::executeTrigger(const Trigger::GuidePath& tr) { 
////////////////////////////////////////////////////////////////////
	cnc->processTrigger(tr); 
}

