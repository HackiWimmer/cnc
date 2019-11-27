#include <math.h>
#include "CncCommon.h"
#include "MainFrame.h"
#include "CncConfig.h"
#include "FileParser.h"
#include "CncControl.h"
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
	setup.optAnalyse		= GBL_CONFIG->getPreProcessorAnalyseFlag();
	setup.optCombineMoves 	= GBL_CONFIG->getPreProcessorCombineMovesFlag();
	setup.optSkipEmptyMoves = GBL_CONFIG->getPreProcessoSkipEmptyFlag();

	wxASSERT( setup.cnc != NULL );
	initNextMoveSequence();
}
//////////////////////////////////////////////////////////////////
CncPathListRunner::~CncPathListRunner() {
//////////////////////////////////////////////////////////////////
	destroyMoveSequence();
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

		currentSequence = new CncMoveSequence(CMD_RENDER_AND_MOVE_SEQUENCE);
		currentSequence->setClientId(clientId);

		THE_APP->getCncPreProcessor()->addMoveSequenceStart(*currentSequence, value_MM_MIN, mode);
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

		THE_APP->getCncPreProcessor()->addMoveSequence(*currentSequence);

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
bool CncPathListRunner::onPhysicallyClientIdChange(long idx, const CncPathListEntry& curr) {
//////////////////////////////////////////////////////////////////
	if ( curr.isClientIdChange() == false ) {
		std::cerr << "CncPathListRunner::onPhysicallyClientIdChange(): Invalid Type!" << std::endl;
		return false;
	}
	
	if ( setup.optAnalyse == true ) {
		if ( currentSequence != NULL ) {
			//std::cout << "CncPathListRunner::onPhysicallyClientIdChange: " << curr.clientId << std::endl;
			currentSequence->setClientId(curr.clientId);
		}
	}

	wxASSERT( setup.cnc != NULL );
	
	if ( setup.cnc->isInterrupted() == true )
		return false;
		
	setup.cnc->setClientId(curr.clientId);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallySpeedChange(unsigned long idx, const CncPathListEntry& curr) {
//////////////////////////////////////////////////////////////////
	if ( curr.isSpeedChange() == false ) {
		std::cerr << "onPhysicallySpeedChange::onPhysicallySpeedChange(): Invalid Type!" << std::endl;
		return false;
	}
	
	wxASSERT( setup.cnc != NULL );
	
	if ( setup.cnc->isInterrupted() == true )
		return false;
	
	if ( initNextMoveSequence(curr.feedSpeed_MM_MIN, cnc::getCncSpeedTypeAsCharacter(curr.feedSpeedMode)) == false ) {
		std::cerr << "onPhysicallySpeedChange::onPhysicallySpeedChange(): initNextMoveSequence failed!" << std::endl;
		return false;
	}
	
	return setup.cnc->changeCurrentFeedSpeedXYZ_MM_MIN(curr.feedSpeed_MM_MIN, curr.feedSpeedMode);
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallyMoveRaw(unsigned long idx, const CncPathListEntry& curr) {
//////////////////////////////////////////////////////////////////
	wxASSERT( setup.cnc != NULL );
	
	if ( setup.cnc->isInterrupted() == true )
		return false;
		
	return setup.cnc->moveRelLinearMetricXYZ(	curr.entryDistance.getX(), 
												curr.entryDistance.getY(), 
												curr.entryDistance.getZ(), 
												curr.alreadyRendered
											);
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallyMoveAnalysed(unsigned long idx, CncPathList::const_iterator& itCurr, const CncPathList::const_iterator& itEnd) {
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

	auto addSequenceEntryFromValues = [&](double dx, double dy, double dz, double f) {
		currentSequence->addMetricPosXYZF(	dx, dy, dz,	f);
		
		return true;
	};
	
	// -----------------------------------------------------------
	auto addSequenceEntryFromEntry = [&](const CncPathListEntry* e) {
		if ( e == NULL )
			return false;

		currentSequence->addMetricPosXYZF(	e->entryDistance.getX(),
											e->entryDistance.getY(),
											e->entryDistance.getZ(),
											0.0); // 0.0 ????
		return true;
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

	// -----------------------------------------------------------
	struct Move {
		double dx;
		double dy;
		double dz;
		double mxy;
		double vxy;
		double mz;

		explicit Move(const CncPathListEntry* e)
		: dx(e->entryDistance.getX())
		, dy(e->entryDistance.getY())
		, dz(e->entryDistance.getZ())
		, mxy(dx != 0.0 ? dy/dx : DBL_MAX)
		, vxy(sqrt(pow(dx, 2) + pow(dy, 2)))
		, mz(vxy != 0.0 ? dz / vxy : DBL_MAX)
		{}

		explicit Move(const Move& m)
		: dx(m.dx)
		, dy(m.dy)
		, dz(m.dz)
		, mxy(m.mxy)
		, vxy(m.vxy)
		, mz(m.mz)
		{}

		bool isPitchEqual(const Move& m) const {
			const double epsilon = 0.001;
			return ( cnc::dblCompare(mxy, m.mxy, epsilon) == true && cnc::dblCompare(mz, m.mz, epsilon) == true );
		}
		
		bool isPitchToStrong(const Move& m) const {
			const float max = 15 * PI / 180; // 15 degrees
			const float a1 = atan2(dx, dy);
			const float a2 = atan2(m.dx, m.dy);
			
			return abs(a1 - a2) > max;
		}
		
		float getPitchDiffenceAsRadians(const Move& m) const {
			const float a1 = atan2(dx, dy);
			const float a2 = atan2(m.dx, m.dy);
			
			return abs(a1 - a2);
		}
		
		float getPitchDiffenceAsDegree(const Move& m) const {
			const float a1 = atan2(dx, dy);
			const float a2 = atan2(m.dx, m.dy);
			
			return abs(a1 - a2) * 180 / PI;
		}
	};

	double cx = curr->entryDistance.getX();
	double cy = curr->entryDistance.getY();
	double cz = curr->entryDistance.getZ();
	
	const CncPathListEntry* c = curr;
	const CncPathListEntry* n = next;

	// loop: preview next entries
	while ( n != NULL ) {
		
		// don't combine entries over a speed change
		if ( n->isSpeedChange()  == true )
			break;

		// skip the next entry if it is an emty move
		if ( setup.optSkipEmptyMoves == true )  {
			if (    cnc::dblCompareNull(n->entryDistance.getX()) == true
				&&	cnc::dblCompareNull(n->entryDistance.getY()) == true
				&&	cnc::dblCompareNull(n->entryDistance.getZ()) == true
			) {
				// skip
				n = getNext(++itCurr);
				continue;
			}
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
			
		// stop here if the next pit isn't equal with the reference
		if ( mCurr.isPitchEqual(mNext) == false ) 
			break;
			
		// concatenate next to the current summary
		cx += mNext.dx;
		cy += mNext.dy;
		cz += mNext.dz;
		
		// skip to next ...
		c = n;
		n = getNext(++itCurr);
	}

	addSequenceEntryFromValues(cx, cy, cz, 0.0); // 0.0 ????

	return true;

/*

	if ( next != NULL ) {

		const double dxC = curr->entryDistance.getX();
		const double dyC = curr->entryDistance.getY();
		const double dxN = next->entryDistance.getX();
		const double dyN = next->entryDistance.getY();

		if ( dxC != 0 && dxN != 0 ) {
			const float m1 = dyC / dxC;
			const float m2 = dyN / dxN;

			if ( m1 * m2 != -1 ) {
				const float alpha = atan( ( m1 - m2 ) / (1 + m1 * m2) ) * 180 / PI;

				std::cout << "alpha: " << alpha << std::endl;
			}
			else {
				// 90
				std::cout << "m1 * m2 != -1" << std::endl;
			}
		}
		else {
			std::cout << "dxC == " << dxC << " || dxN == " << dxN << std::endl;
		}
	}
	else {
		//std::cout << "next == NULL" << std::endl;
	}



*/

	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::onPhysicallyExecute(const CncPathListManager& plm) {
//////////////////////////////////////////////////////////////////
	if ( setup.cnc == NULL ) {
		std::cerr << "CncPathListRunner::execute(): Invalid cnc control!" << std::endl;
		return false;
	}
	
	// ----------------------------------------------------------
	auto deFrost = [&](bool ret) {
		THE_APP->getCncPreProcessor()->thaw();
		return ret;
	};
	
	// over all stored pathes
	THE_APP->getCncPreProcessor()->freeze();
	
	for ( auto it = plm.const_begin(); it != plm.const_end(); ++it) {
		const unsigned long distance = std::distance(plm.const_begin(), it);
		
		const CncPathListEntry& curr  = *it;
		THE_APP->getCncPreProcessor()->addPathListEntry(curr);
		
		if ( setup.fileParser != NULL )
			setup.fileParser->evaluateDebugState();
		
		// client id change
		if ( curr.isClientIdChange() == true ) {
			if ( onPhysicallyClientIdChange(distance, curr) == false )
				return deFrost(false);
		
			continue;
		}
		
		// speed change
		if ( curr.isSpeedChange() == true ) {
			if ( onPhysicallySpeedChange(distance, curr) == false )
				return deFrost(false);
			
			continue;
		}
		
		// position change
		if ( curr.isPositionChange() == true ) {
			if ( setup.optAnalyse == false ) {
				if ( onPhysicallyMoveRaw(distance, curr) == false ) {
					return deFrost(false);
				}
			}
			else {
				if ( onPhysicallyMoveAnalysed(distance, it, plm.const_end()) == false ) {
					return deFrost(false);
				}
			}
			
			continue;
		}
	}
	
	return deFrost(publishMoveSequence());
}
