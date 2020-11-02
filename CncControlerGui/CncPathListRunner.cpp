#include <math.h>
#include "CncCommon.h"
#include "MainFrameProxy.h"
#include "CncContext.h"
#include "CncConfig.h"
#include "FileParser.h"
#include "CncControl.h"
#include "CncAutoFreezer.h"
#include "CncPreprocessor.h"
#include "CncMoveSequence.h"
#include "CncPathListRunner.h"

////////////////////////////////////////////////////////////////////
CncPathListRunner::Move::Move(const CncPathListEntry* e)
: dx		(e->entryDistance.getX())
, dy		(e->entryDistance.getY())
, dz		(e->entryDistance.getZ())
, mxy		(dx != 0.0 ? dy/dx : DBL_MAX)
, vxy		(sqrt(pow(dx, 2) + pow(dy, 2)))
, mz		(vxy != 0.0 ? dz / vxy : DBL_MAX)
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
bool CncPathListRunner::Move::isXYPitchDiffToStrong(const Move& mNext) const {
////////////////////////////////////////////////////////////////////
	const float max = 45 * PI / 180; // 15 degrees
	return std::abs(getXYPitchDiffenceAsRadians(mNext)) > max;
}
////////////////////////////////////////////////////////////////////
bool CncPathListRunner::Move::isZPitchDiffToStrong(const Move& mNext) const {
////////////////////////////////////////////////////////////////////
	const float max = 45 * PI / 180; // 15 degrees
	return std::abs(getZPitchDiffenceAsRadians(mNext)) > max;
}
////////////////////////////////////////////////////////////////////
bool CncPathListRunner::Move::isXYZPitchDiffToStrong(const Move& mNext) const {
////////////////////////////////////////////////////////////////////
	return isXYPitchDiffToStrong(mNext) || isZPitchDiffToStrong(mNext);
}
////////////////////////////////////////////////////////////////////
float CncPathListRunner::Move::getXYPitchDiffenceAsRadians(const Move& mNext) const {
////////////////////////////////////////////////////////////////////
	const float a1 = atan2(dx, dy);
	const float a2 = atan2(mNext.dx, mNext.dy);

	//std::cout << "(" << dx << ", " << dy << "), (" << mNext.dx << ", " << mNext.dy << "), " << (a1 - a2) *180/PI << ", " << a1 << ", " << a2 << std::endl;

	return (a1 - a2);
}
////////////////////////////////////////////////////////////////////
float CncPathListRunner::Move::getZPitchDiffenceAsRadians(const Move& mNext) const {
////////////////////////////////////////////////////////////////////
	const float a1 = atan2(dz, vxy);
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


//////////////////////////////////////////////////////////////////
CncPathListRunner::CncPathListRunner(const CncPathListRunner::Setup& s) 
: currentSequence(NULL)
, setup(s)
//////////////////////////////////////////////////////////////////
{
	wxASSERT( setup.cnc != NULL );
	initializeNextMoveSequence(CLIENT_ID.INVALID);
}
//////////////////////////////////////////////////////////////////
CncPathListRunner::CncPathListRunner(CncControl* cnc) 
: currentSequence(NULL)
, setup()
{
	setup.cnc 				= cnc;
	autoSetup(false);

	wxASSERT( setup.cnc != NULL );
	initializeNextMoveSequence(CLIENT_ID.INVALID);
}
//////////////////////////////////////////////////////////////////
CncPathListRunner::~CncPathListRunner() {
//////////////////////////////////////////////////////////////////
	destroyMoveSequence();
}
//////////////////////////////////////////////////////////////////
void CncPathListRunner::autoSetup(bool trace) {
//////////////////////////////////////////////////////////////////
	setup.optAnalyse		= THE_CONFIG->getPreProcessorAnalyseFlag();
	setup.optCombineMoves 	= THE_CONFIG->getPreProcessorCombineMovesFlag();
	setup.optSkipEmptyMoves = THE_CONFIG->getPreProcessoSkipEmptyFlag();
	setup.trace				= THE_CONFIG->getPreProcessorUseOperatingTrace();
	
	if ( trace && setup.trace == true )
		traceSetup();
}
//////////////////////////////////////////////////////////////////
void CncPathListRunner::traceSetup() {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;
	ss << "CNC Pointer         : " << setup.cnc					<< std::endl;
	ss << "Analyse Pathes      : " << setup.optAnalyse			<< std::endl;
	ss << "Combine Moves       : " << setup.optCombineMoves		<< std::endl;
	ss << "Skip empty Moves    : " << setup.optSkipEmptyMoves	<< std::endl;
	
	CncPreprocessor* cpp = APP_PROXY::getCncPreProcessor();
	wxASSERT( cpp != NULL );
	
	cpp->addOperatingTraceSeparator("Current Setup");
	cpp->addOperatingTrace(ss);
}
//////////////////////////////////////////////////////////////////
void CncPathListRunner::logMeasurementStart() {
//////////////////////////////////////////////////////////////////
	wxASSERT( setup.cnc != NULL );
	setup.cnc->startSerialMeasurement();
}
//////////////////////////////////////////////////////////////////
void CncPathListRunner::logMeasurementEnd() {
//////////////////////////////////////////////////////////////////
	wxASSERT( setup.cnc != NULL );
	setup.cnc->stopSerialMeasurement();
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::isCncInterrupted() {
//////////////////////////////////////////////////////////////////
	wxASSERT( setup.cnc != NULL );

	if ( setup.cnc->isInterrupted() == true )
		return true;

	return false;
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
	CncContext::PositionStorage::addMove(CncContext::PositionStorage::TRIGGER_PH_LST_RUN, dx, dy, dz);
	currentSequence->addMetricPosXYZ(dx, dy, dz);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::addSequenceEntryFromEntry(const CncPathListEntry* e) {
//////////////////////////////////////////////////////////////////
	if ( e == NULL )
		return false;
		
	CncContext::PositionStorage::addMove(CncContext::PositionStorage::TRIGGER_PH_LST_RUN, 
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
	wxASSERT( cpp != NULL );
		
	// if the corresponding list isn't connected this call does nothing and returns only
	cpp->addMoveSequence(*currentSequence);
	
	if ( setup.trace == true ) {
		std::stringstream ss; 
		currentSequence->outputOperator(ss, setup.cnc->getCurCtlPos());
		
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
	
	wxASSERT( setup.cnc != NULL );
	setup.cnc->setClientId(currentSequence->getLastClientId());
	const bool ret = setup.cnc->processMoveSequence(*currentSequence);

	if ( setup.trace == true )
		cpp->addOperatingTraceMovSeqSep(wxString::Format("Call of publishMoveSequence() returned with %d", (int)ret));
		
	return ret;
}
//////////////////////////////////////////////////////////////////
void CncPathListRunner::onPhysicallySwitchToolState(bool state) {
//////////////////////////////////////////////////////////////////
	state == true ? getSetup().cnc->switchToolOn() 
	              : getSetup().cnc->switchToolOff();
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallyClientIdChange(const CncPathListEntry& curr) {
//////////////////////////////////////////////////////////////////
	if ( curr.isClientIdChange() == false ) {
		std::cerr << "CncPathListRunner::onPhysicallyClientIdChange(): Invalid Type!" << std::endl;
		return false;
	}
	
	if ( isCncInterrupted() == true )
		return false;
		
	CncPreprocessor* cpp = APP_PROXY::getCncPreProcessor();
	wxASSERT( cpp != NULL );
	wxASSERT( setup.cnc != NULL );
	
	if ( setup.trace == true )
		cpp->addOperatingTraceSeparator(wxString::Format("ClientID Change (%ld)", curr.clientId));
		
	if ( currentSequence != NULL )	currentSequence->addClientId(curr.clientId);
	else							setup.cnc->setClientId(curr.clientId);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallySpeedChange(const CncPathListEntry& curr, const CncPathListEntry* next) {
//////////////////////////////////////////////////////////////////
	if ( curr.isSpeedChange() == false ) {
		std::cerr << CNC_LOG_FUNCT << ": Invalid Type!" << std::endl;
		return false;
	}
	
	if ( isCncInterrupted() == true )
		return false;
	
	if ( setup.trace == true )
		APP_PROXY::getCncPreProcessor()->addOperatingTraceSeparator(wxString::Format("Speed Change (%c - %5.1lf)", cnc::getCncSpeedTypeAsCharacter(curr.feedSpeedMode), curr.feedSpeed_MM_MIN));
		
	const long nextClientID = next ? next->clientId : CLIENT_ID.INVALID;
	if ( initializeNextMoveSequence(curr.feedSpeed_MM_MIN, cnc::getCncSpeedTypeAsCharacter(curr.feedSpeedMode), nextClientID) == false ) {
		std::cerr << CNC_LOG_FUNCT << ": initNextMoveSequence failed!" << std::endl;
		return false;
	}
	
	wxASSERT( setup.cnc != NULL );
	return setup.cnc->changeCurrentFeedSpeedXYZ_MM_MIN(curr.feedSpeed_MM_MIN, curr.feedSpeedMode);
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallyMoveRaw(const CncPathListEntry& curr) {
//////////////////////////////////////////////////////////////////
	if ( isCncInterrupted() == true )
		return false;
		
	wxASSERT( setup.cnc != NULL );
	
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

	CncContext::PositionStorage::addMove(CncContext::PositionStorage::TRIGGER_PH_LST_RUN, 
												curr.entryDistance.getX(), 
												curr.entryDistance.getY(), 
												curr.entryDistance.getZ());
												

	return setup.cnc->moveRelLinearMetricXYZ(	curr.entryDistance.getX(), 
												curr.entryDistance.getY(), 
												curr.entryDistance.getZ(), 
												curr.alreadyRendered
											);
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
		
		APP_PROXY::getCncPreProcessor()->addPathListEntry(*c);
	};
	
	while ( n != NULL ) {
		
		// check common break conditions
		if ( isCncInterrupted() == true  )	return false;
		if ( checkDebugState()  == false )	return false;
		
		nextClientID = n->clientId;
		
		// check type: Don't combine entries over a speed change
		if ( c->isSpeedChange() == true )  {
			finalizeCurrentSequence = true;
			break;
		}
		
		// check type: Register new client id
		if ( c->isClientIdChange() == true ) {
			
			if ( onPhysicallyClientIdChange(*c) == false )
				return false;
			
			skipToNextEntry();
			continue;
		}
		
		// check content: Skip this entry if it's an empty move
		// this also handle c->isNothingChanged()
		const bool empty = isEmptyMove(c->entryDistance.getX(), c->entryDistance.getY(), c->entryDistance.getZ());
		if ( setup.optSkipEmptyMoves == true && empty ) {
			
			skipToNextEntry();
			continue;
		}
		
		// init move structures
		const Move mCurr(c);
		const Move mNext(getNextPosEntry(itCurr, n));
		
		// check pitch: Stop here if the pitch to nect step is to strong 
		if ( mCurr.isXYPitchDiffToStrong(mNext) == true ) {
			cpp->addOperatingTrace(wxString::Format("--> XY: An entanglement of %.1f degree between X and Y is too strong to keep also the next entry together\n", mCurr.getXYPitchDiffenceAsDegree(mNext)));
			
			wxString x;
			cpp->addOperatingTrace(wxString::Format(" Curr : %s", c->traceEntryToString(x)));
			cpp->addOperatingTrace(wxString::Format(" Next : %s", getNextPosEntry(itCurr, n)->traceEntryToString(x)));
			
			finalizeCurrentSequence = true;
			break;
		}
		
		// check pitch: Stop here if the pitch to nect step is to strong 
		if ( mCurr.isZPitchDiffToStrong(mNext) == true ) {
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
		
		skipToNextEntry();
		
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
bool CncPathListRunner::onPhysicallyExecute(const CncPathListManager& plm) {
//////////////////////////////////////////////////////////////////
	if ( setup.cnc == NULL ) {
		std::cerr << "CncPathListRunner::execute(): Invalid cnc control!" << std::endl;
		return false;
	}
	
	if ( plm.getPathList().size() == 0 )
		return true;
	
	CncPreprocessor* cpp = APP_PROXY::getCncPreProcessor();
	wxASSERT( cpp != NULL );
	
	autoSetup(false);
	
	if ( setup.trace == true ) {
		std::stringstream ss; ss << plm << std::endl;
		cpp->addOperatingTracePthLstSep("Next CncPathListManager");
		cpp->addOperatingTrace(ss);
		
		traceSetup();
	}
	
	// freeze to speed up performace
	CncAutoFreezer caf(cpp);
	
	// Main loop over all path list manager entries
	auto beg = plm.const_begin();
	auto end = plm.const_end();
	for ( auto it = beg; it != end; ++it) {
		
		const CncPathListEntry& curr = *it;
		const CncPathListEntry* next = it + 1 != end ? &(*(it + 1)) : NULL;

		// common runtime check
		if ( isCncInterrupted() == true )	return false;
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
		if ( curr.isClientIdChange() == true ) {
			if ( onPhysicallyClientIdChange(curr) == false )
				return false;
		
			continue;
		}
		
		// ----------------------------------------------------------
		// speed change
		if ( curr.isSpeedChange() == true ) {
			if ( onPhysicallySpeedChange(curr, next) == false )
				return false;
			
			continue;
		}
		
		// ----------------------------------------------------------
		// position change
		if ( curr.isPositionChange() == true ) {
			
			if ( setup.optAnalyse == false ) {
				
				if ( onPhysicallyMoveRaw(curr) == false )
					return false;
			}
			else {
				
				// Note: this call may be increments it
				if ( onPhysicallyMoveAnalysed(it, plm.const_end()) == false ) {
					return false;
				}
			}
			
			continue;
		}
		
		// safty: This should not appear
		wxASSERT(NULL);
		
	} // for
	
	return finalizeCurrMoveSequence(CLIENT_ID.INVALID);
}
