#include <iostream>
#include "CncConfig.h"
#include "GLGuidePath.h"

///////////////////////////////////////////////////////////////////
GLGuidePath::GLGuidePath(const CncPathListManager& plm, double zOffset)
: std::vector<CncDoublePosition>	()
, guideColour						(wxNullColour)
, guideStyle						(wxPENSTYLE_INVALID)
, clientIds							()
///////////////////////////////////////////////////////////////////
{
	if ( plm.getPathType() == CncPathListManager::PathType::PT_GUIDE_PATH ) {
		
		// determine parameteres
		switch ( plm.getGuideType() ) {
			case CncPathListManager::GuideType::HELP_PATH:
			{
				guideColour	= wxColour(  0, 162, 232, defAlphaDimmedUp);
				guideStyle	= wxPENSTYLE_SHORT_DASH;
				break;
			}
			case CncPathListManager::GuideType::ORIG_PATH:
			{
				guideColour	= wxColour(  0, 162, 232, defAlphaDimmedUp);
				guideStyle	= wxPENSTYLE_DOT;
				break;
			}
			case CncPathListManager::GuideType::NO_GUIDE_PATH:
			{
				std::cerr 	<< CNC_LOG_FUNCT
							<< ": Invalid CncPathListManager::GuideType"
							<< std::endl
							;
				break;
			}
		}
		
		// fill guide path list
		if ( isValid() ) {
			for (auto it = plm.cbegin(); it != plm.cend(); ++it) {
				
				const CncPathListEntry& entry =  *it;
				if ( entry.isPositionChange() != true )
					continue;
					
				const double zv = entry.entryTarget.getZ() + zOffset;
				
				const double x  = ( entry.entryTarget.getX() * THE_CONFIG->getCalculationFactX() ) / THE_CONFIG->getDispFactX3D(); 
				const double y  = ( entry.entryTarget.getY() * THE_CONFIG->getCalculationFactY() ) / THE_CONFIG->getDispFactY3D(); 
				const double z  = (                       zv * THE_CONFIG->getCalculationFactZ() ) / THE_CONFIG->getDispFactZ3D(); 
				
				push_back(std::move(CncDoublePosition(x, y, z)));
				clientIds.insert(entry.clientId);
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
GLGuidePath::~GLGuidePath() {
///////////////////////////////////////////////////////////////////
	clear();
}
///////////////////////////////////////////////////////////////////
const bool GLGuidePath::hasClientId(long cid) const {
///////////////////////////////////////////////////////////////////
	return clientIds.find(cid) != clientIds.end();
}
///////////////////////////////////////////////////////////////////
void GLGuidePath::dimDownClientId(long cid) {
///////////////////////////////////////////////////////////////////
	if ( hasClientId(cid) )
		dimDown();
}
///////////////////////////////////////////////////////////////////
void GLGuidePath::dimDown() {
///////////////////////////////////////////////////////////////////
	guideColour.Set(guideColour.Red(), guideColour.Green(), guideColour.Blue(), defAlphaDimmedDown);
}
///////////////////////////////////////////////////////////////////
void GLGuidePath::dimUp() {
///////////////////////////////////////////////////////////////////
	guideColour.Set(guideColour.Red(), guideColour.Green(), guideColour.Blue(), defAlphaDimmedUp);
}

