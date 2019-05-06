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
	setup.cnc = cnc;
	
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
			
			//std::cerr << "CncPathListRunner::destroyMoveSequence(): Sequence isn't empty. Size = " << currentSequence->getCount() << std::endl;
			//std::cerr << " Missing publishMoveSequence() before. All these entries are lost!" << std::endl;
		}
		
		delete currentSequence;
		currentSequence = NULL;
	}

	return ret;
}
//////////////////////////////////////////////////////////////////
bool CncPathListRunner::initNextMoveSequence() {
//////////////////////////////////////////////////////////////////
	if ( setup.optAnalyse == true ) {
		if ( destroyMoveSequence() == false ) {

			std::cerr << "CncPathListRunner::initNextMoveSequence(): destroyMoveSequence failed!" << std::endl;
			return false;
		}

		currentSequence = new CncMoveSequence(CMD_RENDER_AND_MOVE_SEQUENCE);
		THE_APP->getCncPreProcessor()->addMoveSequenceStart(*currentSequence);
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
		if ( currentSequence != NULL )
			currentSequence->setClientId(curr.clientId);
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
	
	if ( initNextMoveSequence() == false ) {
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
bool CncPathListRunner::onPhysicallyMoveAnalysed(unsigned long idx, const CncPathListEntry* curr, const CncPathListEntry* next) {
//////////////////////////////////////////////////////////////////





	//void initNextMoveSequence();
	/*
	if ( next == end() ) {
		publishMoveSequence();
	}
	*/
	//bool publishMoveSequence();
	wxASSERT(curr != NULL);
	wxASSERT(currentSequence != NULL);
	
	currentSequence->addMetricPosXYZF(	curr->entryDistance.getX(),
										curr->entryDistance.getY(),
										curr->entryDistance.getZ(),
										0.0);



	//std::cout << curr->entryDistance << std::endl;


	/*
	if ( next == NULL )
		;publishMoveSequence();
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
	
	auto deFrost = [&](bool ret) {
		THE_APP->getCncPreProcessor()->thaw();
		return ret;
	};
	
	// over all stored pathes
	THE_APP->getCncPreProcessor()->freeze();
	for ( auto it = plm.const_begin(); it != plm.const_end(); ++it) {
		const unsigned long distance = std::distance(plm.const_begin(), it);
		
		const CncPathListEntry curr  = *it;
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
		
		// postion change
		if ( curr.isPositionChange() == true ) {
			if ( setup.optAnalyse == false ) {
				if ( onPhysicallyMoveRaw(distance, curr) == false ) {
					return deFrost(false);
				}
			}
			else {
				const CncPathListEntry* cEntry = &(*it);
				const CncPathListEntry* nEntry = it + 1 != plm.const_end() ? &(*(it + 1)) : NULL;
				
				if ( onPhysicallyMoveAnalysed(distance, cEntry, nEntry) == false ) {
					return deFrost(false);
				}
			}
			
			continue;
		}
	}
	
	return deFrost(publishMoveSequence());
}
