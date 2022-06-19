#include "CncControl.h"
#include "CncControl.h"
#include "MainFrame.h"
#include "CncTemplateContext.h"
#include "CncPreprocessor.h"
#include "CncPathListInterfaceCnc.h"

////////////////////////////////////////////////////////////////////

typedef CncPathListInterfaceCnc CPLI;

bool CPLI::CncMovSeqInstruction				::process(CPLI* i)					{ return i->executeMoveImage(img); }
bool CPLI::CncPathListInstruction			::process(CPLI* i)					{ return i->executePathListEntry(ple); }
bool CPLI::CncGuidPathInstruction			::process(CPLI* i)					{ return i->executeGuidePath(plm); }
bool CPLI::CncClientIDInstruction			::process(CPLI* i)					{ return i->executeClientIDChange(cid); }
bool CPLI::CncFeedSpeedInstruction			::process(CPLI* i)					{ return i->executeFeedSpeedChange(value_MM_MIN, mode); }
bool CPLI::CncToolChangeInstruction			::process(CPLI* i)					{ return i->executeToolChange(diameter); }
bool CPLI::CncSpindleStateInstruction		::process(CPLI* i)					{ return i->executeSpindleStateSwitch(on, force); }
bool CPLI::CncSpindleSpeedInstruction		::process(CPLI* i)					{ return i->executeSpindleSpeedChange(value_U_MIN); }
bool CPLI::InstructionTriggerBeginRun		::process(CPLI* i)					{ i->executeTrigger(tr); return true; }
bool CPLI::InstructionTriggerEndRun			::process(CPLI* i)					{ i->executeTrigger(tr); return true; }
bool CPLI::InstructionTriggerNextPath		::process(CPLI* i)					{ i->executeTrigger(tr); return true; }
bool CPLI::InstructionTriggerSpeedChange	::process(CPLI* i)					{ i->executeTrigger(tr); return true; }
bool CPLI::InstructionTriggerGuidePath		::process(CPLI* i)					{ i->executeTrigger(tr); return true; }

void CPLI::CncClientIDInstruction			::traceTo(std::ostream& o) const	{ o << "Next Id: "			<< cid 							<< std::endl; }
void CPLI::CncMovSeqInstruction				::traceTo(std::ostream& o) const	{ o << " MoveSequence: ";	img.traceTo(o); o				<< std::endl; } 
void CPLI::CncPathListInstruction			::traceTo(std::ostream& o) const	{ o << " PathEntry: ";		ple.traceEntry(o); o			<< std::endl; }
void CPLI::CncGuidPathInstruction			::traceTo(std::ostream& o) const	{ o << " Guide Path:" 		<< plm							<< std::endl; }
void CPLI::CncFeedSpeedInstruction			::traceTo(std::ostream& o) const	{ o << " Speed: F("			<< value_MM_MIN << ")"			<< std::endl; }
void CPLI::CncToolChangeInstruction			::traceTo(std::ostream& o) const	{ o << " Tool Change("		<< diameter << ")"				<< std::endl; }
void CPLI::CncSpindleStateInstruction		::traceTo(std::ostream& o) const	{ o << " Spindle Motor: ("	<< (on ? "ON" : "OFF") << ")"	<< std::endl; }
void CPLI::CncSpindleSpeedInstruction		::traceTo(std::ostream& o) const	{ o << " Spindle Speed: S("	<< value_U_MIN << ")"			<< std::endl; }
void CPLI::InstructionTriggerBeginRun		::traceTo(std::ostream& o) const	{ o << tr 													<< std::endl; }
void CPLI::InstructionTriggerEndRun			::traceTo(std::ostream& o) const	{ o << tr 													<< std::endl; }
void CPLI::InstructionTriggerNextPath		::traceTo(std::ostream& o) const	{ o << tr 													<< std::endl; }
void CPLI::InstructionTriggerSpeedChange	::traceTo(std::ostream& o) const	{ o << tr 													<< std::endl; }
void CPLI::InstructionTriggerGuidePath		::traceTo(std::ostream& o) const	{ o << tr 													<< std::endl; }

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
bool CncPathListInterfaceCnc::spoolInstructions(CncInstructionList* ci) {
////////////////////////////////////////////////////////////////////
	if ( ci == NULL )
		return false;
		
	CNC_CEX2_A("Start processing cnc instructions (entries=%zu)", ci->size())
	FORCE_LOGGER_UPDATE
	
	CncPreprocessor* pp = THE_APP->getCncPreProcessor();
	
	for ( auto instruction : *ci )
	{
		if ( pp && pp->isCncInstTraceActive() )
		{
			std::stringstream ss;
			instruction->traceTo(ss);
			pp->addCncInstructionTrace(ss.str().c_str());
		}
		
		if ( instruction->process(this) == false )
		{
			std::stringstream ss; instruction->traceTo(ss);
			CNC_CERR_FUNCT_A("Error while processing: %s", ss.str().c_str())
			return false;
		}
		
		if ( THE_APP->evaluateAndPerformProcessingState() == false )
			return false;
	}
	
	return true;
}
////////////////////////////////////////////////////////////////////
bool CncPathListInterfaceCnc::spoolInstructions() {
////////////////////////////////////////////////////////////////////
	return spoolInstructions(&cncInstructions);
}
////////////////////////////////////////////////////////////////////
void CncPathListInterfaceCnc::processTrigger(const Trigger::EndRun& tr) { 
////////////////////////////////////////////////////////////////////
	cncInstructions.push_back(new InstructionTriggerEndRun(tr)); 
	
	THE_TPL_CTX->registerCncInterface(this);
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
