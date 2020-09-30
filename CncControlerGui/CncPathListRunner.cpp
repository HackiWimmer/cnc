#include <math.h>
#include "CncCommon.h"
#include "MainFrameProxy.h"
#include "CncConfig.h"
#include "FileParser.h"
#include "CncControl.h"
#include "CncAutoFreezer.h"
#include "CncPreprocessor.h"
#include "CncMoveSequence.h"
#include "CncPathListRunner.h"

//////////////////////////////////////////////////////////////////
CncPathListRunner::CncPathListRunner(const CncPathListRunner::Setup& s) 
: currentSequence(NULL)
, setup(s)
//////////////////////////////////////////////////////////////////
{
	wxASSERT( setup.cnc != NULL );
	initNextMoveSequence();
}
//////////////////////////////////////////////////////////////////
CncPathListRunner::CncPathListRunner(CncControl* cnc) 
: currentSequence(NULL)
, setup()
{
	setup.cnc 				= cnc;
	setup.optAnalyse		= THE_CONFIG->getPreProcessorAnalyseFlag();
	setup.optCombineMoves 	= THE_CONFIG->getPreProcessorCombineMovesFlag();
	setup.optSkipEmptyMoves = THE_CONFIG->getPreProcessoSkipEmptyFlag();

	wxASSERT( setup.cnc != NULL );
	initNextMoveSequence();
}
//////////////////////////////////////////////////////////////////
CncPathListRunner::~CncPathListRunner() {
//////////////////////////////////////////////////////////////////
	destroyMoveSequence();
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
bool CncPathListRunner::destroyMoveSequence() {
//////////////////////////////////////////////////////////////////	
	bool ret = true;
	
	if ( currentSequence != NULL ) {
		if ( currentSequence->getCount() > 0 ) {
			ret = publishMoveSequence();
		}
		
		delete currentSequence;
		currentSequence = NULL;
	}

	return ret;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::initNextMoveSequence(double value_MM_MIN, char mode) {
//////////////////////////////////////////////////////////////////
	if ( setup.optAnalyse == true ) {
		// to hand over the client id to the next sequence
		const long clientId = currentSequence != NULL ? currentSequence->getClientId(): CLIENT_ID.INVALID;

		if ( destroyMoveSequence() == false ) {

			std::cerr << "CncPathListRunner::initNextMoveSequence(): destroyMoveSequence failed!" << std::endl;
			return false;
		}
		
		CncMoveSequence::SpeedInfo si;
		si.value = value_MM_MIN;
		si.mode  = mode;
		
		currentSequence = new CncMoveSequence(CMD_RENDER_AND_MOVE_SEQUENCE);
		currentSequence->addClientId(clientId);
		currentSequence->setCurrentSpeedInfo(si);
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::publishMoveSequence() {
//////////////////////////////////////////////////////////////////	
	bool ret = true;
	
	if ( setup.optAnalyse == true ) {
		
		if ( currentSequence == NULL ) {
			std::cerr << "CncPathListRunner::publishMoveSequence(): Invalid Sequence!" << std::endl;
			return false;
		}
		
		// if the corresponding list isn't connected this call does nothing and returns only
		wxASSERT( APP_PROXY::getCncPreProcessor() != NULL );
		APP_PROXY::getCncPreProcessor()->addMoveSequence(*currentSequence);
		
		if ( currentSequence->getCount() > 0 ) {

			wxASSERT( setup.cnc != NULL );
			ret = setup.cnc->processMoveSequence(*currentSequence);

			if ( ret == true )
				currentSequence->clear();
		}
	}
		
	return ret;
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

	wxASSERT( setup.cnc != NULL );

	if ( currentSequence != NULL )	currentSequence->addClientId(curr.clientId);
	else							setup.cnc->setClientId(curr.clientId);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallySpeedChange(const CncPathListEntry& curr) {
//////////////////////////////////////////////////////////////////
	if ( curr.isSpeedChange() == false ) {
		std::cerr << "onPhysicallySpeedChange::onPhysicallySpeedChange(): Invalid Type!" << std::endl;
		return false;
	}
	
	if ( isCncInterrupted() == true )
		return false;
	
	if ( initNextMoveSequence(curr.feedSpeed_MM_MIN, cnc::getCncSpeedTypeAsCharacter(curr.feedSpeedMode)) == false ) {
		std::cerr << "onPhysicallySpeedChange::onPhysicallySpeedChange(): initNextMoveSequence failed!" << std::endl;
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

	// -----------------------------------------------------------
	auto getCurr = [&](const CncPathList::const_iterator& it) {
		return &(*it);
	};

	// -----------------------------------------------------------
	auto getNext = [&](const CncPathList::const_iterator& it) {
		return it + 1 != itEnd ? &(*(it + 1)) : NULL;
	};

	auto addSequenceEntryFromValues = [&](double dx, double dy, double dz) {
		currentSequence->addMetricPosXYZ(dx, dy, dz);
		
		return true;
	};
	
	// -----------------------------------------------------------
	auto addSequenceEntryFromEntry = [&](const CncPathListEntry* e) {
		if ( e == NULL )
			return false;

		currentSequence->addMetricPosXYZ(	e->entryDistance.getX(),
											e->entryDistance.getY(),
											e->entryDistance.getZ());
		return true;
	};

	// -----------------------------------------------------------
	auto isEmptyMove = [&](double dx, double dy, double dz) {
		return (cnc::dblCompareNull(dx) == true &&	cnc::dblCompareNull(dy) == true &&	cnc::dblCompareNull(dz) == true);
	};

	// -----------------------------------------------------------
	// entry ....
	const CncPathListEntry* curr  = getCurr(itCurr);
	const CncPathListEntry* next  = getNext(itCurr);

	wxASSERT(curr != NULL);

	// -----------------------------------------------------------
	// check if current is a empty move
	if ( setup.optSkipEmptyMoves == true )  {
		if (    cnc::dblCompareNull(curr->entryDistance.getX()) == true
			 &&	cnc::dblCompareNull(curr->entryDistance.getY()) == true
			 &&	cnc::dblCompareNull(curr->entryDistance.getZ()) == true
		   ) {

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

	// initialized distances
	double cx = curr->entryDistance.getX();
	double cy = curr->entryDistance.getY();
	double cz = curr->entryDistance.getZ();
	
	// create navigation pointers
	const CncPathListEntry* c = curr;
	const CncPathListEntry* n = next;

	// loop: preview next entries
	auto skipToNext = [&]() { ++itCurr; c = n; n = getNext(itCurr); };
	while ( n != NULL ) {
		
		if ( isCncInterrupted() == true )
			return false;

		if ( checkDebugState() == false )
			return false;

		// don't combine entries over a speed change
		if ( c->isSpeedChange()  == true )
			break;

		if ( c->isClientIdChange() == true ) {
			wxASSERT ( currentSequence != NULL );
			currentSequence->addClientId(n->clientId);
		}

		// skip the next entry if it is an empty move
		const bool empty = isEmptyMove(c->entryDistance.getX(), c->entryDistance.getY(), c->entryDistance.getZ());
		if ( setup.optSkipEmptyMoves == true && empty ) {
			skipToNext();
			continue;
		}
		
		const Move mCurr(c);
		const Move mNext(n);
		
		// .....
		if ( mCurr.isPitchToStrong(mNext) == true ) {
			
			//std::cout << "isPitchToStrong: " << mCurr.getPitchDiffenceAsDegree(mNext) << std::endl;
			//MessageBoxA(0,"","",0);
		}
		
		// stop here if nothing should be combined
		if ( setup.optCombineMoves == false ) 
			break;
			
		// stop here if the next pitch isn't equal with the reference
		if ( mCurr.isPitchEqual(mNext) == false ) 
			break;
			
		// concatenate next to the current summary
		cx += mNext.dx;
		cy += mNext.dy;
		cz += mNext.dz;
		
		skipToNext();
	}

	// finally check this again
	if ( setup.optSkipEmptyMoves == true && isEmptyMove(cx, cy, cz) )
		return true;

	addSequenceEntryFromValues(cx, cy, cz);
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallyExecute(const CncPathListManager& plm) {
//////////////////////////////////////////////////////////////////
	if ( setup.cnc == NULL ) {
		std::cerr << "CncPathListRunner::execute(): Invalid cnc control!" << std::endl;
		return false;
	}
	
	// over all stored pathes
	CncPreprocessor* cpp = APP_PROXY::getCncPreProcessor();
	wxASSERT( cpp != NULL );
	
	CncAutoFreezer caf(cpp);
	
	for ( auto it = plm.const_begin(); it != plm.const_end(); ++it) {
		const CncPathListEntry& curr  = *it;

		if ( isCncInterrupted() == true )
			return false;

		// if the corresponding list isn't connected this call does nothing and returns only
		APP_PROXY::getCncPreProcessor()->addPathListEntry(curr);
		
		if ( checkDebugState() == false )
			return false;
		
		// client id change
		if ( curr.isClientIdChange() == true ) {
			if ( onPhysicallyClientIdChange(curr) == false )
				return false;
		
			continue;
		}
		
		// speed change
		if ( curr.isSpeedChange() == true ) {
			if ( onPhysicallySpeedChange(curr) == false )
				return false;
			
			continue;
		}
		
		// position change
		if ( curr.isPositionChange() == true ) {
			if ( setup.optAnalyse == false ) {
				if ( onPhysicallyMoveRaw(curr) == false ) {
					return false;
				}
			}
			else {
				if ( onPhysicallyMoveAnalysed(it, plm.const_end()) == false ) {
					return false;
				}
			}
			
			continue;
		}
	}
	
	return publishMoveSequence();
}
