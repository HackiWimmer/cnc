#include "MainFrameProxy.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncFileNameService.h"
#include "CncMotionMonitor.h"
#include "CncPathListInterface.h"

////////////////////////////////////////////////////////////////////
CncPathListMonitor::CncPathListMonitor()
: CncPathListRunner::Interface	(CncPathListRunner::Interface::IT_Monitor)
, current						()
, tsMeasurementStart			(CncTimeFunctions::getNanoTimestamp())
, tsMeasurementLast				(tsMeasurementStart)
////////////////////////////////////////////////////////////////////
{
}
////////////////////////////////////////////////////////////////////
CncPathListMonitor::~CncPathListMonitor() {
////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////
bool CncPathListMonitor::dispatchEventQueue() {
////////////////////////////////////////////////////////////////////
	// Event handling, enables the interrupt functionallity
	static       CncMilliTimestamp tsLastDispatch   = 0;

	if ( THE_CONTEXT->isAllowEventHandling() ) {
		const CncMilliTimespan timespanEvent = CncTimeFunctions::getTimeSpan(CncTimeFunctions::getMilliTimestamp(), tsLastDispatch);
		
		if ( timespanEvent >= THE_CONTEXT->getUpdateInterval() ) {
			APP_PROXY::getMotionMonitor()->update(true);
			APP_PROXY::dispatchAll();
			tsLastDispatch = CncTimeFunctions::getMilliTimestamp();
		}
	}
	
	return true;
}
////////////////////////////////////////////////////////////////////
CncLongPosition CncPathListMonitor::getPositionSteps() const {
////////////////////////////////////////////////////////////////////
	CncLongPosition ret;
	return THE_CONFIG->convertMetricToSteps(ret, current.monitorPos);
}
////////////////////////////////////////////////////////////////////
void CncPathListMonitor::publishGuidePath(const CncPathListManager& plm, double zOffset) {
////////////////////////////////////////////////////////////////////
	APP_PROXY::getMotionMonitor()->appendGuidPath(plm, zOffset);
	dispatchEventQueue();
}
////////////////////////////////////////////////////////////////////
void CncPathListMonitor::processClientIDChange(long cid) { 
////////////////////////////////////////////////////////////////////
	current.clientId = cid;
	dispatchEventQueue();
}
////////////////////////////////////////////////////////////////////
bool CncPathListMonitor::processFeedSpeedChange(double value_MM_MIN, CncSpeedMode m) { 
////////////////////////////////////////////////////////////////////
	current.speedValue_MM_MIN	= value_MM_MIN;
	current.speedMode			= m;
	
	return true;
}
////////////////////////////////////////////////////////////////////
bool CncPathListMonitor::processMoveSequence(CncMoveSequence& msq) {
////////////////////////////////////////////////////////////////////
	CncLongPosition ps;
	THE_CONFIG->convertMetricToSteps(ps, current.monitorPos);
	
	for ( auto it = msq.const_begin(); it != msq.const_end(); ++it ) {
		const CncMoveSequence::SequencePoint& sp = *it;
		
		ps.inc(sp.x, sp.y, sp.z);
		THE_CONFIG->convertStepsToMetric(current.monitorPos, ps);
		
		APP_PROXY::getMotionMonitor()->appendVertex(current.clientId, current.speedMode, ps);
		notifyNextPostion();
		dispatchEventQueue();
	}
	
	return true;
}
////////////////////////////////////////////////////////////////////
bool CncPathListMonitor::processPathListEntry(const CncPathListEntry& ple) {
////////////////////////////////////////////////////////////////////
	current.monitorPos = ple.entryTarget;
	APP_PROXY::getMotionMonitor()->appendVertex(current.clientId, current.speedMode, getPositionSteps());
	notifyNextPostion();
	dispatchEventQueue();
	
	return true;
}

////////////////////////////////////////////////////////////////////
CncPathListFileStore::CncPathListFileStore()
: CncPathListMonitor()
, headerStream		()
, bodyStream		()
, footerStream		()
, delimiter			(';')
////////////////////////////////////////////////////////////////////
{
	headerStream << wxString::Format(	"%s%c" 
										
										"%s%c"
										"%s%c"
										
										"%s%c"
										"%s%c"
										
										"%s%c"
										"%s%c"
										"%s%c"
										,
										
										"ClientId",		delimiter,
										
										"S-State",		delimiter,
										"S-Value",		delimiter,
										
										"F-Mode",		delimiter,
										"F-Value",		delimiter,
										
										"PosX",			delimiter,
										"PosY",			delimiter,
										"PosZ",			delimiter
									)
				 << std::endl
	;
	
	footerStream << std::endl;
}
////////////////////////////////////////////////////////////////////
CncPathListFileStore::~CncPathListFileStore() {
////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////
void CncPathListFileStore::notifyNextPostion() {
////////////////////////////////////////////////////////////////////
	wxString tmp;
	
	auto cnv1 = [&](CncSpeedMode sm)	{ return cnc::getCncSpeedTypeAsCharacter(sm); };
	auto cnv2 = [&](double v)			{ tmp.assign(wxString::Format("%lf", v)); tmp.Replace(".",","); return tmp; };
	
	bodyStream	<< wxString::Format	(	"%ld%c" 
										"%d%c" 
										"%s%c" 
										
										"%c%c" 
										"%s%c"
										
										"%s%c"
										"%s%c"
										"%s%c"
										,
										
										current.clientId,					delimiter,
										current.spindleState,				delimiter,
										cnv2(current.spindleSpeed),			delimiter,
										
										cnv1(current.speedMode),			delimiter,
										cnv2(current.speedValue_MM_MIN),	delimiter,
										
										cnv2(current.monitorPos.getX()),	delimiter,
										cnv2(current.monitorPos.getY()),	delimiter,
										cnv2(current.monitorPos.getZ()),	delimiter
									)
				<< std::endl;
	;
}
////////////////////////////////////////////////////////////////////
bool CncPathListFileStore::save(const wxString& fileName) {
////////////////////////////////////////////////////////////////////
	wxFileName fn(fileName);
	
	if ( fn.DirExists() == false ) {
		std::cerr << CNC_LOG_FUNCT_A("The corresponding directory didn't exists: '%s'\n", fileName);
		return false;
	}
	
	if ( fn.HasName() == false ) {
		std::cerr << CNC_LOG_FUNCT_A("The corresponding file name isn't valid: '%s'\n", fileName);
		return false;
	}
	
	std::ofstream ofs (fileName.c_str().AsChar(), std::ofstream::out);
	
	if ( ofs.good() ) {
			
			ofs	<< headerStream	.str()
				<< bodyStream	.str()
				<< footerStream	.str()
			;
		
		ofs.close();
		
		std::cout << "--------------------------------------------------------------------"	<< std::endl; 
		std::cout << "Preprocessor(File):  Result is be located at:\n  FILE: " << fileName	<< std::endl; 
		std::cout << "--------------------------------------------------------------------"	<< std::endl; 
		
		return true;
	}
	
	return false;
}
////////////////////////////////////////////////////////////////////
void CncPathListFileStore::processTrigger(const Trigger::BeginRun& tr) {
////////////////////////////////////////////////////////////////////
	bodyStream.str() = "";
}
////////////////////////////////////////////////////////////////////
void CncPathListFileStore::processTrigger(const Trigger::EndRun& tr) {
////////////////////////////////////////////////////////////////////
	save(CncFileNameService::getCncPathListFileStoreDefFileName());
}

