#include <math.h>
#include "CncCommon.h"
#include "MainFrameProxy.h"
#include "CncContext.h"
#include "CncConfig.h"
#include "FileParser.h"
#include "CncMotionMonitor.h"
#include "CncAutoFreezer.h"
#include "CncPreprocessor.h"
#include "CncMoveSequence.h"
#include "CncPathListInterface.h"
#include "CncPathListRunner.h"

// debug only
namespace DebugPLR {
	bool trace		= false;
	bool tracePitch = false;
};

////////////////////////////////////////////////////////////////////
float CncPathListRunner::Move::maxXYPitchRadians	= CncPathListRunner::Move::degree2Radians(15);
float CncPathListRunner::Move::maxZPitchRadians		= CncPathListRunner::Move::degree2Radians(15);

////////////////////////////////////////////////////////////////////
CncPathListRunner::Move::Move(double dX, double dY, double dZ)
: dx		(dX)
, dy		(dY)
, dz		(dZ)
, mxy		(dx != 0.0 ? dy/dx : DBL_MAX)
, vxy		(sqrt(pow(dx, 2) + pow(dy, 2)))
, mz		(vxy != 0.0 ? dz / vxy : DBL_MAX)
////////////////////////////////////////////////////////////////////
{
}
////////////////////////////////////////////////////////////////////
CncPathListRunner::Move::Move(const CncDoubleDistance& d) 
: CncPathListRunner::Move::Move(d.getX(), d.getY(), d.getZ()) 
{
}
////////////////////////////////////////////////////////////////////
CncPathListRunner::Move::Move(const CncPathListEntry* e)
: CncPathListRunner::Move::Move(e->entryDistance) 
////////////////////////////////////////////////////////////////////
{
}
////////////////////////////////////////////////////////////////////
CncPathListRunner::Move::Move(const Move& m)
: dx		(m.dx)
, dy		(m.dy)
, dz		(m.dz)
, mxy		(m.mxy)
, vxy		(m.vxy)
, mz		(m.mz)
////////////////////////////////////////////////////////////////////
{
}
////////////////////////////////////////////////////////////////////
bool CncPathListRunner::Move::isXYZPitchEqual(const Move& mNext) const {
////////////////////////////////////////////////////////////////////
	const double epsilon = 0.001;
	return ( cnc::dblCompare(mxy, mNext.mxy, epsilon) == true && cnc::dblCompare(mz, mNext.mz, epsilon) == true );
}
////////////////////////////////////////////////////////////////////
bool CncPathListRunner::Move::isXYPitchDiffTooStrong(const Move& mNext) const {
////////////////////////////////////////////////////////////////////
	DebugPLR::tracePitch = DebugPLR::trace;
	
	const double absDiff = std::fabs(getXYPitchDiffenceAsRadians(mNext));
	const bool ret = ( absDiff > PI ? ( 2 * PI - absDiff ) : absDiff ) > maxXYPitchRadians;

	if ( DebugPLR::trace ) {
		std::cout	<< wxString::Format(" --> {%10.3lf, %d, [%d]}", ( absDiff > PI ? ( 2 * PI - absDiff ) : absDiff ) * 180/PI, (int)(absDiff > PI), (int)ret )
					<< std::endl;
	}
	
	DebugPLR::tracePitch = false;
	return ret;
}
////////////////////////////////////////////////////////////////////
bool CncPathListRunner::Move::isZPitchDiffTooStrong(const Move& mNext) const {
////////////////////////////////////////////////////////////////////
	return std::abs(getZPitchDiffenceAsRadians(mNext)) > maxZPitchRadians;
}
////////////////////////////////////////////////////////////////////
bool CncPathListRunner::Move::isXYZPitchDiffTooStrong(const Move& mNext) const {
////////////////////////////////////////////////////////////////////
	return isXYPitchDiffTooStrong(mNext) || isZPitchDiffTooStrong(mNext);
}
////////////////////////////////////////////////////////////////////
float CncPathListRunner::Move::getXYPitchDiffenceAsRadians(const Move& mNext) const {
////////////////////////////////////////////////////////////////////
	if ( cnc::dblCompareNull(dx) && cnc::dblCompareNull(dy) )
		return 0.0;
	
	if ( cnc::dblCompareNull(mNext.dx) && cnc::dblCompareNull(mNext.dy) )
		return 0.0;

	const float a1 = atan2(dx,       dy);
	const float a2 = atan2(mNext.dx, mNext.dy);

	if ( DebugPLR::tracePitch == true ) {
		std::cout	<< wxString::Format("(%10.3lf, %10.3lf) , ", dx, dy )
					<< wxString::Format("(%10.3lf, %10.3lf) , ", mNext.dx, mNext.dy )
					<< wxString::Format("(%10.3lf, %10.3lf --> %10.3lf) , ", a1  * 180/PI, a2  * 180/PI, (a1 - a2) * 180/PI )
					<< wxString::Format("[%10.3lf]", maxXYPitchRadians * 180/PI)
					;
	}
	
	return (a1 - a2);
}
////////////////////////////////////////////////////////////////////
float CncPathListRunner::Move::getZPitchDiffenceAsRadians(const Move& mNext) const {
////////////////////////////////////////////////////////////////////
	if ( cnc::dblCompareNull(dz) && cnc::dblCompareNull(vxy) )
		return 0.0;
	
	if ( cnc::dblCompareNull(mNext.dz) && cnc::dblCompareNull(mNext.vxy) )
		return 0.0;

	if ( cnc::dblCompareNull(vxy) )
		return dz > 0.0 ? +( PI / 2.0) : -( PI / 2.0 );
		
	const float a1 = atan2(dz,       vxy);
	const float a2 = atan2(mNext.dz, mNext.vxy);

	return (a1 - a2);
}
////////////////////////////////////////////////////////////////////
float CncPathListRunner::Move::getXYPitchDiffenceAsDegree(const Move& mNext) const {
////////////////////////////////////////////////////////////////////
	return getXYPitchDiffenceAsRadians(mNext) * 180 / PI;
}
////////////////////////////////////////////////////////////////////
float CncPathListRunner::Move::getZPitchDiffenceAsDegree(const Move& mNext) const {
////////////////////////////////////////////////////////////////////
	return getZPitchDiffenceAsRadians(mNext) * 180 / PI;
}

////////////////////////////////////////////////////////////////////
bool CncPathListRunner::Move::test() {
////////////////////////////////////////////////////////////////////
	
	// ------------------------------------------------------------
	auto pitchDiff = [](const Move& m1, const Move& m2) {
		
		std::cout	<< wxString::Format("XY Pitch: (%+.2lf, %+.2lf, %+.2lf)(%+.2lf, %+.2lf, %+.2lf)", m1.dx, m1.dy, m1.dz, m2.dx, m2.dy, m2.dz)
					<< " = " 
					<< wxString::Format("[%+7.2lf] [%+7.2lf]", atan2(m1.dx, m1.dy) * 180 / PI , atan2(m2.dx, m2.dy) * 180 / PI  )
					<< " => " 
					<< wxString::Format("%+7.2lf", m1.getXYPitchDiffenceAsDegree(m2))
					<< "; " 
					<< "Pitch too strong: " << ( m1.isXYPitchDiffTooStrong(m2) ? "Yes" : " No" )
					<< std::endl
					;
		
		return true;
	};
	
	const Move m1[] = {
		{+0.0, +0.0, +0.0},
		{+0.5, +0.0, +0.0},
		{+0.5, +0.5, +0.0},
		{+0.0, +0.5, +0.0},
		{-0.0, +0.5, +0.0},
		{-0.5, +0.0, +0.0},
		{-0.5, -0.5, +0.0},
		{+0.0, -0.5, +0.0},
		{+0.5, -0.5, +0.0},
		{+0.2, -0.5, +0.0},
	};

	const Move m2[] = {
		{+0.0, +0.0, +0.0},
		{+0.5, +0.0, +0.0},
		{+0.5, +0.5, +0.0},
		{+0.0, +0.5, +0.0},
		{-0.0, +0.5, +0.0},
		{-0.5, +0.0, +0.0},
		{-0.5, -0.5, +0.0},
		{+0.0, -0.5, +0.0},
		{+0.5, -0.5, +0.0},
		{+0.2, -0.5, +0.0},
	};
	
	maxXYPitchRadians = CncPathListRunner::Move::degree2Radians(46);
	maxZPitchRadians  = CncPathListRunner::Move::degree2Radians(46);
	
	std::clog << "maxXYPitchRadians: " << maxXYPitchRadians * 180 / PI << std::endl;
	std::clog << "maxZPitchRadians : " << maxZPitchRadians  * 180 / PI << std::endl;
	
	unsigned int s1 = sizeof(m1) / sizeof(*m1);
	unsigned int s2 = sizeof(m2) / sizeof(*m2);
	
	for (unsigned int i = 0; i < s1; i++ ) {
		for (unsigned int j = 0; j < s2; j++ ) {
			
			if (pitchDiff(m1[i], m2[j]) == false ) 
				return false;
		}
		
		std::cout << std::endl;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////
CncPathListRunner::CncPathListRunner(CncControl* cnc) 
: currentSequence		(NULL)
, currentInterface		(new CncCtrl(cnc))
, setup					()
//////////////////////////////////////////////////////////////////
{
	autoSetup(false);
	initializeNextMoveSequence(CLIENT_ID.INVALID);
}
//////////////////////////////////////////////////////////////////
CncPathListRunner::~CncPathListRunner() {
//////////////////////////////////////////////////////////////////
	destroyMoveSequence();
	wxDELETE(currentInterface);
}
//////////////////////////////////////////////////////////////////
void CncPathListRunner::changePathListRunnerInterfaceImpl(const wxString& portName) {
//////////////////////////////////////////////////////////////////
	//std::cout << CNC_LOG_FUNCT_A(wxString::Format("%s\n", portName));
	
	if		( portName.IsSameAs(_portPreProcMonitor) )	installInterface(new CncPathListMonitor());
	else if	( portName.IsSameAs(_portPreProcFile) )		installInterface(new CncPathListFileStore());
	else												installInterface(new CncCtrl(APP_PROXY::getCncControl()));
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::installInterface(CncPathListRunner::Interface* iface) {
//////////////////////////////////////////////////////////////////
	if ( iface == NULL )
		return false;
		
	wxDELETE(currentInterface);
	currentInterface = iface;
	
	return true;
}
//////////////////////////////////////////////////////////////////
void CncPathListRunner::autoSetup(bool trace) {
//////////////////////////////////////////////////////////////////
	setup.optAnalyse		= THE_CONFIG->getPreProcessorAnalyseFlag();
	setup.optCombineMoves 	= THE_CONFIG->getPreProcessorCombineMovesFlag();
	setup.optSkipEmptyMoves = THE_CONFIG->getPreProcessoSkipEmptyFlag();
	setup.trace				= THE_CONFIG->getPreProcessorUseOperatingTrace();
	
	typedef CncPathListRunner::Move Move;
	Move::maxXYPitchRadians	= Move::degree2Radians(THE_CONFIG->getMaxXYPitchToKeep());
	Move::maxZPitchRadians	= Move::degree2Radians(THE_CONFIG->getMaxZPitchToKeep());
	
	if ( trace && setup.trace == true )
		traceSetup();
}
//////////////////////////////////////////////////////////////////
void CncPathListRunner::traceSetup() {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << "Analyse Pathes      : " << setup.optAnalyse			<< std::endl;
	ss << "Combine Moves       : " << setup.optCombineMoves		<< std::endl;
	ss << "Skip empty Moves    : " << setup.optSkipEmptyMoves	<< std::endl;
	ss << "Max XY Pitch        : " << Move::maxXYPitchRadians	<< std::endl;
	ss << "Max  Z Pitch        : " << Move::maxZPitchRadians	<< std::endl;
	
	CncPreprocessor* cpp = APP_PROXY::getCncPreProcessor();
	wxASSERT( cpp != NULL );
	
	cpp->addOperatingTraceSeparator("Current Setup");
	cpp->addOperatingTrace(ss);
}
//////////////////////////////////////////////////////////////////
void CncPathListRunner::logMeasurementStart() {
//////////////////////////////////////////////////////////////////
	currentInterface->logMeasurementStart();
}
//////////////////////////////////////////////////////////////////
void CncPathListRunner::logMeasurementEnd() {
//////////////////////////////////////////////////////////////////
	currentInterface->logMeasurementStart();
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::isInterrupted() {
//////////////////////////////////////////////////////////////////
	return currentInterface->isInterrupted();
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::checkDebugState() {
//////////////////////////////////////////////////////////////////
	if ( setup.fileParser != NULL )
		return setup.fileParser->evaluateDebugState();

	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::finalizeCurrMoveSequence(long nextClientId) {
//////////////////////////////////////////////////////////////////
	return initializeNextMoveSequence(nextClientId);
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::initializeNextMoveSequence(long clientId) {
//////////////////////////////////////////////////////////////////
	CncMoveSequence::SpeedInfo defSi;
	return initializeNextMoveSequence(defSi.value, defSi.mode, clientId);
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::initializeNextMoveSequence(double value_MM_MIN, char mode, long clientId) {
//////////////////////////////////////////////////////////////////
	if ( setup.optAnalyse == true ) {
		
		CncMoveSequence::SpeedInfo si(value_MM_MIN, mode);
		
		// to hand over the speed info on demand
		if ( cnc::dblCompareNull(si.value) == true && currentSequence != NULL )
			si = currentSequence->getCurrentSpeedInfo();
		
		// this will also publish the current sequence
		if ( destroyMoveSequence() == false ) {
			std::cerr << CNC_LOG_FUNCT << ": destroyMoveSequence failed!" << std::endl;
			return false;
		}
		
		currentSequence = new CncMoveSequence(CMD_RENDER_AND_MOVE_SEQUENCE);
		currentSequence->addClientId(clientId);
		currentSequence->setCurrentSpeedInfo(si);
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::addSequenceEntryFromValues(double dx, double dy, double dz) {
//////////////////////////////////////////////////////////////////
	PositionStorage::addMove(PositionStorage::TRIGGER_PH_LST_RUN, dx, dy, dz);
	currentSequence->addMetricPosXYZ(dx, dy, dz);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::addSequenceEntryFromEntry(const CncPathListEntry* e) {
//////////////////////////////////////////////////////////////////
	if ( e == NULL )
		return false;
		
	PositionStorage::addMove(PositionStorage::TRIGGER_PH_LST_RUN, 
										e->entryDistance.getX(), 
										e->entryDistance.getY(), 
										e->entryDistance.getZ());
										
	currentSequence->addMetricPosXYZ(	e->entryDistance.getX(),
										e->entryDistance.getY(),
										e->entryDistance.getZ());
	return true;
}
//////////////////////////////////////////////////////////////////
// don't call this functions directly. use initializeNextMoveSequence() 
// or finalizeCurrMoveSequence() instead
bool CncPathListRunner::destroyMoveSequence() {
//////////////////////////////////////////////////////////////////	
	bool ret = true;
	
	if ( currentSequence != NULL && currentSequence->getCount() > 0 ) {
		ret = publishMoveSequence();
		wxDELETE ( currentSequence );
	}

	return ret;
}
//////////////////////////////////////////////////////////////////
// don't call this functions directly. use initializeNextMoveSequence() 
// or finalizeCurrMoveSequence() instead
bool CncPathListRunner::publishMoveSequence() {
//////////////////////////////////////////////////////////////////
	if ( setup.optAnalyse == false )
		return true;

	if ( currentSequence == NULL ) {
		std::cerr << "CncPathListRunner::publishMoveSequence(): Invalid Sequence!" << std::endl;
		return false;
	}
	
	CncPreprocessor* cpp = APP_PROXY::getCncPreProcessor();
		
	// if the corresponding list isn't connected this call does nothing and returns only
	cpp->addMoveSequence(*currentSequence);
	
	if ( setup.trace == true ) {
		std::stringstream ss; 
		currentSequence->outputOperator(ss, currentInterface->getPositionSteps());
		
		cpp->addOperatingTraceMovSeqSep("Try to publish next CncMoveSequence");
		cpp->addOperatingTrace(ss);
	}
		
	if ( currentSequence->getCount() == 0 ) {
		if ( setup.trace == true ) {
			const char* msg = "Call of publishMoveSequence(): Empty CncMoveSequence, nothing was publsihed.";
			cpp->addOperatingTraceMovSeqSep(msg);
		}
		
		return true;
	}
	
	currentInterface->processClientIDChange(currentSequence->getLastClientId());
	const bool ret = currentInterface->processMoveSequence(*currentSequence);

	if ( setup.trace == true )
		cpp->addOperatingTraceMovSeqSep(wxString::Format("Call of publishMoveSequence() returned with %d", (int)ret));
		
	return ret;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallyClientIdChange(const CncPathListEntry& curr) {
//////////////////////////////////////////////////////////////////
	if ( curr.hasClientIdChange() == false ) {
		std::cerr << "CncPathListRunner::onPhysicallyClientIdChange(): Invalid Type!" << std::endl;
		return false;
	}
	
	if ( isInterrupted() == true )
		return false;
		
	CncPreprocessor* cpp = APP_PROXY::getCncPreProcessor();
	
	if ( setup.trace == true )
		cpp->addOperatingTraceSeparator(wxString::Format("ClientID Change (%ld)", curr.clientId));
		
	if ( currentSequence != NULL )	currentSequence->addClientId(curr.clientId);
	else							currentInterface->processClientIDChange(curr.clientId);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallyFeedSpeedChange(const CncPathListEntry& curr, const CncPathListEntry* next) {
//////////////////////////////////////////////////////////////////
	if ( curr.hasSpeedChange() == false ) {
		std::cerr << CNC_LOG_FUNCT << ": Invalid Type!" << std::endl;
		return false;
	}
	
	if ( isInterrupted() == true )
		return false;
	
	if ( setup.trace == true ) {
		const wxString msg(wxString::Format("Feed Speed Change (%c - %5.1lf)", 
											cnc::getCncSpeedTypeAsCharacter(curr.feedSpeedMode), curr.feedSpeed_MM_MIN));
											
		APP_PROXY::getCncPreProcessor()->addOperatingTraceSeparator(msg);
	}
		
	const long nextClientID = next ? next->clientId : CLIENT_ID.INVALID;
	
	if ( initializeNextMoveSequence(curr.feedSpeed_MM_MIN, cnc::getCncSpeedTypeAsCharacter(curr.feedSpeedMode), nextClientID) == false ) {
		std::cerr << CNC_LOG_FUNCT << ": initNextMoveSequence failed!" << std::endl;
		return false;
	}
	
	return currentInterface->processFeedSpeedChange(curr.feedSpeed_MM_MIN, curr.feedSpeedMode);
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallySpindleChange(const CncPathListEntry& curr) {
//////////////////////////////////////////////////////////////////
	if ( curr.hasToolChange() == false ) {
		std::cerr << CNC_LOG_FUNCT << ": Invalid Type!" << std::endl;
		return false;
	}
	
	if ( isInterrupted() == true )
		return false;
	
	if ( setup.trace == true ) {
		const wxString msg(wxString::Format("Spindle State, Speed Change (%s, %.1lf)", 
											curr.spindleState ? "ON" : "OFF", curr.spindleSpeed_U_MIN));
											
		APP_PROXY::getCncPreProcessor()->addOperatingTraceSeparator(msg);
	}
	
	if ( currentInterface->processSpindleStateSwitch(curr.spindleState) == false ) {
		std::cerr << CNC_LOG_FUNCT << ": processSpindleStateSwitch() failed!" << std::endl;
		return false;
	}
	
	return currentInterface->processSpindleSpeedChange(curr.spindleSpeed_U_MIN);
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallyMoveRaw(const CncPathListEntry& curr) {
//////////////////////////////////////////////////////////////////
	if ( isInterrupted() == true )
		return false;
		
	if ( setup.trace == true ) {
		std::stringstream ss; 
		ss 	<< "Distance         : " << cnc::dblFormat(curr.entryDistance)	<< std::endl
			<< "Already Rendered : " << curr.alreadyRendered				<< std::endl
		;
		
		CncPreprocessor* cpp = APP_PROXY::getCncPreProcessor();
		wxASSERT( cpp != NULL );
		
		cpp->addOperatingTraceMovSeqSep("Next PhysicallyMoveRaw");
		cpp->addOperatingTrace(ss);
	}

	// Important: Move absolute to avoid a error propagation
	PositionStorage::addPos(PositionStorage::TRIGGER_PH_LST_RUN, 
												curr.entryTarget.getX(), 
												curr.entryTarget.getY(), 
												curr.entryTarget.getZ());

	return currentInterface->processPathListEntry(curr);
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallyMoveAnalysed(CncPathList::const_iterator& itCurr, const CncPathList::const_iterator& itEnd) {
//////////////////////////////////////////////////////////////////
	wxASSERT(currentSequence != NULL);

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	auto getCurrEntry    = [&](const CncPathList::const_iterator& it) { return &(*it); };
	auto getNextEntry    = [&](const CncPathList::const_iterator& it) { return it + 1 != itEnd ? &(*(it + 1)) : NULL; };
	
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	auto getNextPosEntry = [&](const CncPathList::const_iterator& it, const CncPathListEntry* defaultRet=NULL) { 
		
		uint32_t index = 1;
		while ( it + index != itEnd ) {
			
			if ( (it + index)->isPositionChange() )
				return &(*(it + index));
				
			index++;
		}
		
		return defaultRet; 
	};
	
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	auto isEmptyMove = [&](double dx, double dy, double dz) {
		return (cnc::dblCompareNull(dx) == true &&	cnc::dblCompareNull(dy) == true &&	cnc::dblCompareNull(dz) == true);
	};
	
	// -----------------------------------------------------------
	CncPreprocessor* cpp = APP_PROXY::getCncPreProcessor();
	wxASSERT( cpp != NULL );

	// -----------------------------------------------------------
	// entries in focus ...
	const CncPathListEntry* curr  = getCurrEntry(itCurr);
	const CncPathListEntry* next  = getNextEntry(itCurr);
	
	wxASSERT(curr != NULL);
	
	// -----------------------------------------------------------
	// debugging only ... 
	const bool addCurrentEntryOnlyToCurrentSequence = false;
	if ( addCurrentEntryOnlyToCurrentSequence == true )
		return addSequenceEntryFromEntry(curr);
	
	// -----------------------------------------------------------
	// check if current is a empty move
	if ( setup.optSkipEmptyMoves == true ) {
		if ( isEmptyMove(curr->entryDistance.getX(), curr->entryDistance.getY(), curr->entryDistance.getZ()) ) {
			// skip
			return true;
		}
	}
	
	// -----------------------------------------------------------
	// check if nothing more than curr available
	if ( next == NULL ) {
		addSequenceEntryFromEntry(curr);
		return true;
	}
	
	// -----------------------------------------------------------
	// Form here on try to optimize
	
	// initialize distances
	double cx = curr->entryDistance.getX();
	double cy = curr->entryDistance.getY();
	double cz = curr->entryDistance.getZ();
	
	long nextClientID 				= CLIENT_ID.INVALID;
	bool finalizeCurrentSequence	= false;
	
	// create navigation pointers
	const CncPathListEntry* c = curr;
	const CncPathListEntry* n = next;
	
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	auto skipToNextEntry = [&]() { 
		
		++itCurr; 
		c = n; 
		n = getNextEntry(itCurr); 
		
		if ( c != NULL )	APP_PROXY::getCncPreProcessor()->addPathListEntry(*c);
		else				std::cerr << CNC_LOG_FUNCT << ": skipToNextEntry() failed!" << std::endl;
			
		return (c != NULL && n != NULL);
	};
	
	while ( c != NULL && n != NULL ) {
		
		// check common break conditions
		if ( isInterrupted()   == true  )	return false;
		if ( checkDebugState() == false )	return false;
		
		nextClientID = n->clientId;
		
		// check type: Don't combine entries over a speed change
		if ( c->hasSpeedChange() == true )  {
			finalizeCurrentSequence = true;
			
			if ( c->isSpeedChange() ) {
				break;
			}
			else {
			#warning
			
			}
		}
		
		// check type: Register new client id
		if ( c->hasClientIdChange() == true ) {
			
			if ( onPhysicallyClientIdChange(*c) == false )
				return false;
			
			if ( c->isClientIdChange() ) {
				if ( skipToNextEntry() == false )
					break;
					
				continue;
			}
		}
		
		// check type: process spindle update
		if ( c->hasToolChange() == true ) {
			if ( onPhysicallySpindleChange(*c) == false )
				return false;
				
			if ( c->isToolChange() == true ) {
				if ( skipToNextEntry() == false )
					break;
				
				continue;
			}
		}
		
		// check content: Skip this entry if it's an empty move
		// this also handle c->isNothingChanged()
		const bool empty = isEmptyMove(c->entryDistance.getX(), c->entryDistance.getY(), c->entryDistance.getZ());
		if ( setup.optSkipEmptyMoves == true && empty ) {
			
			if ( skipToNextEntry() == false )
				break;
				
			continue;
		}
		
		// initialize move structures
		const Move mCurr(c);
		const Move mNext(getNextPosEntry(itCurr, n));
		
		// check pitch: Stop here if the pitch to nect step is to strong 
		if ( mCurr.isXYPitchDiffTooStrong(mNext) == true ) {
			cpp->addOperatingTrace(wxString::Format("--> XY: An entanglement of %.1f degree between X and Y is too strong to keep also the next entry together\n", mCurr.getXYPitchDiffenceAsDegree(mNext)));
			
			wxString x;
			cpp->addOperatingTrace(wxString::Format(" Curr : %s", c->traceEntryToString(x)));
			cpp->addOperatingTrace(wxString::Format(" Next : %s", getNextPosEntry(itCurr, n)->traceEntryToString(x)));
			
			finalizeCurrentSequence = true;
			break;
		}
		
		// check pitch: Stop here if the pitch to nect step is to strong 
		if ( mCurr.isZPitchDiffTooStrong(mNext) == true ) {
			cpp->addOperatingTrace(wxString::Format("--> Z: An entanglement of %.1f degree between Z and the XY Pane is too strong to keep also the next entry together\n", mCurr.getZPitchDiffenceAsDegree(mNext)));
			
			wxString x;
			cpp->addOperatingTrace(wxString::Format(" Curr : %s", c->traceEntryToString(x)));
			cpp->addOperatingTrace(wxString::Format(" Next : %s", getNextPosEntry(itCurr, n)->traceEntryToString(x)));
			
			finalizeCurrentSequence = true;
			break;
		}
		
		// stop here if nothing should be combined
		if ( setup.optCombineMoves == false ) 
			break;
		
		
		
		// stop here if the next pitch isn't equal with the reference
		// why ?????
		
		
		//, mxy		(dx != 0.0 ? dy/dx : DBL_MAX)
		//, vxy		(sqrt(pow(dx, 2) + pow(dy, 2)))
		//, mz		(vxy != 0.0 ? dz / vxy : DBL_MAX)
		// 	return ( cnc::dblCompare(mxy, mNext.mxy, epsilon) == true && cnc::dblCompare(mz, mNext.mz, epsilon) == true );

		if ( mCurr.isXYZPitchEqual(mNext) == false ) 
			break;
		
		
		// concatenate next to the current summary
		cx += mNext.dx;
		cy += mNext.dy;
		cz += mNext.dz;
		
		if ( skipToNextEntry() == false )
			break;
		
	} // while a next entry is available
	
	// finally check this again
	if ( setup.optSkipEmptyMoves == true && isEmptyMove(cx, cy, cz) )
		return true;

	// add the current distance
	addSequenceEntryFromValues(cx, cy, cz);
	
	if ( finalizeCurrentSequence == true )
		return finalizeCurrMoveSequence(nextClientID);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::publishGuidePath(const CncPathListManager& plm, double zOffset) {
//////////////////////////////////////////////////////////////////
	if ( plm.getPathList().size() == 0 )
		return true;
	
	if ( plm.getPathType() != CncPathListManager::PathType::PT_GUIDE_PATH )
		return false;
		
	currentInterface->publishGuidePath(plm, zOffset);
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallyExecute(const CncPathListManager& plm) {
//////////////////////////////////////////////////////////////////
	if ( plm.getPathList().size() == 0 )
		return true;
	
	// forward guide pathes
	if ( plm.getPathType() == CncPathListManager::PathType::PT_GUIDE_PATH )
		return publishGuidePath(plm, 0.0);
	
	CncPreprocessor* cpp = APP_PROXY::getCncPreProcessor();
	
	autoSetup(false);
	
	if ( setup.trace == true ) {
		std::stringstream ss; ss << plm << std::endl;
		cpp->addOperatingTracePthLstSep("Next CncPathListManager\n");
		cpp->addOperatingTrace(ss);
		
		traceSetup();
	}
	
	// freeze only if shown to speed up performance - significantly!
	if ( cpp->IsShownOnScreen() )
		CncAutoFreezer caf(cpp);
	
	// Main loop over all path list manager cnc entries
	auto beg = plm.cbegin();
	auto end = plm.cend();
	for ( auto it = beg; it != end; ++it) {
		
		const CncPathListEntry& curr = *it;
		const CncPathListEntry* next = it + 1 != end ? &(*(it + 1)) : NULL;

		// common runtime check
		if ( isInterrupted()   == true )	return false;
		if ( checkDebugState() == false )	return false;
		
		// if the corresponding gui list isn't connected this call 
		// does nothing and returns only
		APP_PROXY::getCncPreProcessor()->addPathListEntry(curr);

		// ----------------------------------------------------------
		if ( curr.isNothingChanged() == true ) {
			continue;
		}
		
		// ----------------------------------------------------------
		// client id change
		if ( curr.hasClientIdChange() == true ) {
			if ( onPhysicallyClientIdChange(curr) == false )
				return false;
		
			if ( curr.isClientIdChange() )
				continue;
		}
		
		// ----------------------------------------------------------
		// feed speed change
		if ( curr.hasSpeedChange() == true ) {
			if ( onPhysicallyFeedSpeedChange(curr, next) == false )
				return false;
			
			if ( curr.isSpeedChange() == true )
				continue;
		}
		
		// ----------------------------------------------------------
		// spindle speed change
		if ( curr.hasToolChange() == true ) {
			if ( onPhysicallySpindleChange(curr) == false ) {
					std::cerr << CNC_LOG_FUNCT_A("\n");
				
				return false;
			}
			
			if ( curr.isToolChange() == true )
				continue;
		}
		
		// ----------------------------------------------------------
		// position change
		if ( curr.hasPositionChange() == true ) {
			
			if ( setup.optAnalyse == false ) {
				
				if ( onPhysicallyMoveRaw(curr) == false )
					return false;
			}
			else {
				
				// Note: this call may be increments it
				if ( onPhysicallyMoveAnalysed(it, plm.cend()) == false ) {
					return false;
				}
			}
			
			if ( curr.isPositionChange() == true )
				continue;
		}
		
	} // for
	
	const bool ret = finalizeCurrMoveSequence(CLIENT_ID.INVALID);
	return ret;
}
