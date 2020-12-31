#include "MainFrameProxy.h"
#include "CncConfig.h"
#include "CncFileNameService.h"
#include "CncMotionMonitor.h"
#include "CncPathListIntrface.h"

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
CncLongPosition CncPathListMonitor::getPositionSteps() const {
////////////////////////////////////////////////////////////////////
	CncLongPosition ret;
	return THE_CONFIG->convertMetricToSteps(ret, current.monitorPos);
}
////////////////////////////////////////////////////////////////////
void CncPathListMonitor::publishGuidePath(const CncPathListManager& plm, double zOffset) {
////////////////////////////////////////////////////////////////////
	APP_PROXY::getMotionMonitor()->appendGuidPath(plm, zOffset);
	APP_PROXY::getMotionMonitor()->update(true);
}
////////////////////////////////////////////////////////////////////
void CncPathListMonitor::processClientIDChange(long cid) { 
////////////////////////////////////////////////////////////////////
	current.clientId = cid;
	
	APP_PROXY::getMotionMonitor()->update(true);
}
////////////////////////////////////////////////////////////////////
bool CncPathListMonitor::processSpeedChange(double value_MM_MIN, CncSpeedMode m) { 
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
	}
	
	return true;
}
////////////////////////////////////////////////////////////////////
bool CncPathListMonitor::processPathListEntry(const CncPathListEntry& ple) {
////////////////////////////////////////////////////////////////////
	current.monitorPos = ple.entryTarget;
	APP_PROXY::getMotionMonitor()->appendVertex(current.clientId, current.speedMode, getPositionSteps());
	notifyNextPostion();
	
	return true;
}

////////////////////////////////////////////////////////////////////
CncPathListFileStore::CncPathListFileStore()
: CncPathListMonitor()
, headerStream		()
, bodyStream		()
, footerStream		()
////////////////////////////////////////////////////////////////////
{
	headerStream << "" << std::endl;
	footerStream << std::endl;
}
////////////////////////////////////////////////////////////////////
CncPathListFileStore::~CncPathListFileStore() {
////////////////////////////////////////////////////////////////////

}
////////////////////////////////////////////////////////////////////
void CncPathListFileStore::notifyNextPostion() {
////////////////////////////////////////////////////////////////////
	
	auto cnv = [&](CncSpeedMode sm) { return cnc::getCncSpeedTypeAsCharacter(sm); };
	
	const char delimiter = ';';
	
	bodyStream	<< wxString::Format	(	"%ld%c" 
										"%d%c" 
										
										"%c%c" 
										"%lf%c"
										
										"%lf%c"
										"%lf%c"
										"%lf%c"
										,
										
										current.clientId,					delimiter,
										current.toolState,					delimiter,
										
										cnv(current.speedMode),				delimiter,
										current.speedValue_MM_MIN,			delimiter,
										
										current.monitorPos.getX(),			delimiter,
										current.monitorPos.getY(),			delimiter,
										current.monitorPos.getZ(),			delimiter
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
	CNC_LOG_FUNCT_A("CncFileNameService::getCncPathListFileStoreDefFileName()\n");
	
	save(CncFileNameService::getCncPathListFileStoreDefFileName());
}

