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
	autoSetup();

	wxASSERT( setup.cnc != NULL );
	initNextMoveSequence();
}
//////////////////////////////////////////////////////////////////
CncPathListRunner::~CncPathListRunner() {
//////////////////////////////////////////////////////////////////
	destroyMoveSequence();
}
//////////////////////////////////////////////////////////////////
void CncPathListRunner::autoSetup() {
//////////////////////////////////////////////////////////////////
	setup.optAnalyse		= THE_CONFIG->getPreProcessorAnalyseFlag();
	setup.optCombineMoves 	= THE_CONFIG->getPreProcessorCombineMovesFlag();
	setup.optSkipEmptyMoves = THE_CONFIG->getPreProcessoSkipEmptyFlag();
	setup.trace				= THE_CONFIG->getPreProcessorUseOperatingTrace();
	
	CncPreprocessor* cpp = APP_PROXY::getCncPreProcessor();
	wxASSERT( cpp != NULL );
	
	if ( setup.trace == true ) {
		std::stringstream ss;
		ss << "CNC Pointer         : " << setup.cnc					<< std::endl;
		ss << "Analyse Pathes      : " << setup.optAnalyse			<< std::endl;
		ss << "Combine Moves       : " << setup.optCombineMoves		<< std::endl;
		ss << "Skip empty Moves    : " << setup.optSkipEmptyMoves	<< std::endl;
		
		cpp->addOperatingTraceSeparator("Current Setup");
		cpp->addOperatingTrace(ss);
	}
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
		
		CncPreprocessor* cpp = APP_PROXY::getCncPreProcessor();
		wxASSERT( cpp != NULL );
		
		if ( setup.trace == true ) {
			std::stringstream ss; 
			currentSequence->outputOperator(ss, setup.cnc->getCurCtlPos());
			
			cpp->addOperatingTraceMovSeqSep("Next CncMoveSequence");
			cpp->addOperatingTrace(ss);
		}
			
		// if the corresponding list isn't connected this call does nothing and returns only
		cpp->addMoveSequence(*currentSequence);
		
		if ( currentSequence->getCount() > 0 ) {

			wxASSERT( setup.cnc != NULL );
			setup.cnc->setClientId(currentSequence->getLastClientId());
			ret = setup.cnc->processMoveSequence(*currentSequence);

			if ( ret == true )
				currentSequence->clear();
		}
		else {
			std::clog << "CncPathListRunner::publishMoveSequence(): Empty Sequence" << std::endl;
		}
	}
		
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
bool CncPathListRunner::onPhysicallySpeedChange(const CncPathListEntry& curr) {
//////////////////////////////////////////////////////////////////
	if ( curr.isSpeedChange() == false ) {
		std::cerr << "onPhysicallySpeedChange::onPhysicallySpeedChange(): Invalid Type!" << std::endl;
		return false;
	}
	
	if ( isCncInterrupted() == true )
		return false;
	
	if ( setup.trace == true )
		APP_PROXY::getCncPreProcessor()->addOperatingTraceSeparator(wxString::Format("Speed Change (%c - %5.1lf)", cnc::getCncSpeedTypeAsCharacter(curr.feedSpeedMode), curr.feedSpeed_MM_MIN));

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

	return setup.cnc->moveRelLinearMetricXYZ(	curr.entryDistance.getX(), 
												curr.entryDistance.getY(), 
												curr.entryDistance.getZ(), 
												curr.alreadyRendered
											);
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallyMoveRawAsSequence(const CncPathListEntry& curr) {
//////////////////////////////////////////////////////////////////
	CncMoveSequence cms(CMD_RENDER_AND_MOVE_SEQUENCE);
	
	CncMoveSequence::SpeedInfo si;
	si.value = curr.feedSpeed_MM_MIN;
	si.mode  = cnc::getCncSpeedTypeAsCharacter(curr.feedSpeedMode);
	
	const long clientId = currentSequence != NULL ? currentSequence->getClientId(): CLIENT_ID.INVALID;
	cms.addClientId(clientId);
	cms.setCurrentSpeedInfo(si);
	
	cms.addMetricPosXYZ(	curr.entryDistance.getX(),
							curr.entryDistance.getY(),
							curr.entryDistance.getZ());
							
	CncPreprocessor* cpp = APP_PROXY::getCncPreProcessor();
	wxASSERT( cpp != NULL );
	
	cpp->addMoveSequence(cms);
		
	if ( setup.trace == true ) {
		std::stringstream ss; 
		cms.outputOperator(ss, setup.cnc->getCurCtlPos());
		
		cpp->addOperatingTraceMovSeqSep("Next CncMoveSequence");
		cpp->addOperatingTrace(ss);
	}
	
	wxASSERT ( setup.cnc != NULL );
	return setup.cnc->processMoveSequence(cms);
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

	// -----------------------------------------------------------
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
	// entries in focus ...
	const CncPathListEntry* curr  = getCurr(itCurr);
	const CncPathListEntry* next  = getNext(itCurr);
	
	wxASSERT(curr != NULL);

	// debugging only ... 
	const bool addCurrentEntryOnlyCurrentSequence = true;
	if ( addCurrentEntryOnlyCurrentSequence == true )
		return addSequenceEntryFromEntry(curr);

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
	
//
addSequenceEntryFromEntry(curr);
return true;

	// -----------------------------------------------------------
	// check if nothing more than curr available
	if ( next == NULL ) {
		addSequenceEntryFromEntry(curr);
		return true;
	}
	
	// initialize distances
	double cx = curr->entryDistance.getX();
	double cy = curr->entryDistance.getY();
	double cz = curr->entryDistance.getZ();
	
	// create navigation pointers
	const CncPathListEntry* c = curr;
	const CncPathListEntry* n = next;

	// loop: preview next entries
	auto skipToNext = [&]() { ++itCurr; c = n; n = getNext(itCurr); };
	
	if ( false ) {
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
			
		} // while
		
	} // if
	
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
	
	CncPreprocessor* cpp = APP_PROXY::getCncPreProcessor();
	wxASSERT( cpp != NULL );
	
	autoSetup();
	
	if ( setup.trace == true ) {
		std::stringstream ss; ss << plm << std::endl;
		cpp->addOperatingTracePthLstSep("Next CncPathListManager");
		cpp->addOperatingTrace(ss);
	}
	
	// freeze to speed up performace
	CncAutoFreezer caf(cpp);
	
	// over all path list manager entries
	for ( auto it = plm.const_begin(); it != plm.const_end(); ++it) {
		const CncPathListEntry& curr  = *it;

		// runtime check
		if ( isCncInterrupted() == true )
			return false;

		// if the corresponding gui list isn't connected this call 
		// does nothing and returns only
		APP_PROXY::getCncPreProcessor()->addPathListEntry(curr);
		
		// runtime check
		if ( checkDebugState() == false )
			return false;
		
		if ( curr.isNothingChanged() == true ) {
			continue;
		}
		
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
				
				if ( false ) {
					if ( onPhysicallyMoveRawAsSequence(curr) == false )
						return false;
				}
				else {
					if ( onPhysicallyMoveRaw(curr) == false )
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
		
		// safty: This should not appear
		wxASSERT(NULL);
		
	} // for
	
	return publishMoveSequence();
}
