#include "CncConfig.h"
#include "CncClipperLib.h"
#include "CncPathListManager.h"

//////////////////////////////////////////////////////////////
#define ASSERT_CHECK_PLML_SIZE_NOT_NULL \
	if ( getPathList().size() == 0 ) { \
		std::cerr	<< CNC_LOG_FUNCT \
					<< ": Cant be the first entry. Use initNext[Cnc|Guide]Path() before!" \
					<< " Nothing will be added!" \
					<< std::endl; \
		return defaultEntry; \
	}
 
//////////////////////////////////////////////////////////////////
CncPathListManager* CncPathListManager::clone(const CncPathListManager& plm) {
//////////////////////////////////////////////////////////////////
	return new CncPathListManager(plm);
}
//////////////////////////////////////////////////////////////////
ClipperLib::JoinType CncPathListManager::transform(JoinType jt) {
//////////////////////////////////////////////////////////////////
	switch ( jt ) {
		case jtSquare:	return ClipperLib::jtSquare;
		case jtRound:	return ClipperLib::jtRound;
		case jtMiter:	return ClipperLib::jtMiter;
	}
	
	return ClipperLib::jtSquare;
}
//////////////////////////////////////////////////////////////////
ClipperLib::EndType CncPathListManager::transform(EndType et) {
//////////////////////////////////////////////////////////////////
	switch ( et ) {
		case etClosedPolygon:	return ClipperLib::etClosedPolygon;
		case etClosedLine:		return ClipperLib::etClosedLine;
		case etOpenButt:		return ClipperLib::etOpenButt;
		case etOpenSquare:		return ClipperLib::etOpenSquare;
		case etOpenRound:		return ClipperLib::etOpenRound;
	}
	
	return ClipperLib::etClosedPolygon;
}
//////////////////////////////////////////////////////////////////
CncPathListManager::JoinType CncPathListManager::transform(ClipperLib::JoinType jt) {
//////////////////////////////////////////////////////////////////
	switch ( jt ) {
		case ClipperLib::jtSquare:	return jtSquare;
		case ClipperLib::jtRound:	return jtRound;
		case ClipperLib::jtMiter:	return jtMiter;
	}
	
	return jtSquare;

}
//////////////////////////////////////////////////////////////////
CncPathListManager::EndType CncPathListManager::transform(ClipperLib::EndType et) {
//////////////////////////////////////////////////////////////////
	switch ( et ) {
		case ClipperLib::etClosedPolygon:	return etClosedPolygon;
		case ClipperLib::etClosedLine:		return etClosedLine;
		case ClipperLib::etOpenButt:		return etOpenButt;
		case ClipperLib::etOpenSquare:		return etOpenSquare;
		case ClipperLib::etOpenRound:		return etOpenRound;
	}
	
	return etClosedPolygon;

}

//////////////////////////////////////////////////////////////
CncPathListManager::CncPathListManager()
: pathList				()
, guideList				()
, clipperPath			()
, pathType				(PT_CNC_PATH)
, guideType 			(NO_GUIDE_PATH)
, defaultEntry			()
, prevCncStartEntry		(NULL)
, isFirstPath			(false)
, isCorrected			(false)
, referencePos			(0.0, 0.0, 0.0)
, minPosX				(0.0)
, minPosY				(0.0)
, minPosZ				(0.0)
, maxPosX				(0.0)
, maxPosY				(0.0)
, maxPosZ				(0.0)
, totalDistance			(0.0)
//////////////////////////////////////////////////////////////////
{
	// reservate memory
	pathList .reserve(THE_CONFIG->getConstReserveCapacity());
	guideList.reserve(THE_CONFIG->getConstReserveCapacity());
	
	// create initial entry
	initNextPath();
}
//////////////////////////////////////////////////////////////////
CncPathListManager::CncPathListManager(const CncPathListManager& plm) 
: pathList				(plm.getProtectedPathList())
, guideList				(plm.getProtectedGuideList())
, clipperPath			(plm.getProtectedClipperPath())
, pathType				(plm.getPathType())
, guideType 			(plm.getGuideType())
, defaultEntry			(plm.getDefaultEntry())
, prevCncStartEntry		(new CncPathListEntry(*plm.getPrevCncStartEntry()))
, isFirstPath			(plm.isPathFlaggedAsFirst())
, isCorrected			(plm.isPathCorrected())
, referencePos			(plm.getReferencePos())
, minPosX				(plm.getMinPosX())
, minPosY				(plm.getMinPosY())
, minPosZ				(plm.getMinPosZ())
, maxPosX				(plm.getMaxPosX())
, maxPosY				(plm.getMaxPosY())
, maxPosZ				(plm.getMaxPosZ())
, totalDistance			(plm.getTotalDistance())
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
CncPathListManager::CncPathListManager(CncPathListManager& plm) 
: pathList				(plm.getProtectedPathList())
, guideList				(plm.getProtectedGuideList())
, clipperPath			(plm.getProtectedClipperPath())
, pathType				(plm.getPathType())
, guideType 			(plm.getGuideType())
, defaultEntry			(plm.getDefaultEntry())
, prevCncStartEntry		(new CncPathListEntry(*plm.getPrevCncStartEntry()))
, isFirstPath			(plm.isPathFlaggedAsFirst())
, isCorrected			(plm.isPathCorrected())
, referencePos			(plm.getReferencePos())
, minPosX				(plm.getMinPosX())
, minPosY				(plm.getMinPosY())
, minPosZ				(plm.getMinPosZ())
, maxPosX				(plm.getMaxPosX())
, maxPosY				(plm.getMaxPosY())
, maxPosZ				(plm.getMaxPosZ())
, totalDistance			(plm.getTotalDistance())
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
CncPathListManager& CncPathListManager::operator=(const CncPathListManager& plm) {
//////////////////////////////////////////////////////////////////
	// self-assignment check
	if ( this != &plm ) {
		
		pathList			= plm.getProtectedPathList();
		guideList			= plm.getProtectedGuideList();
		clipperPath			= plm.getProtectedClipperPath();
 		pathType			= plm.getPathType();
		guideType 			= plm.getGuideType();
		defaultEntry		= plm.getDefaultEntry();
		prevCncStartEntry	= new CncPathListEntry(*plm.getPrevCncStartEntry());
		isFirstPath			= plm.isPathFlaggedAsFirst();
		isCorrected			= plm.isPathCorrected();
		referencePos		= plm.getReferencePos();
		minPosX				= plm.getMinPosX();
		minPosY				= plm.getMinPosY();
		minPosZ				= plm.getMinPosZ();
		maxPosX				= plm.getMaxPosX();
		maxPosY				= plm.getMaxPosY();
		maxPosZ				= plm.getMaxPosZ();
		totalDistance		= plm.getTotalDistance();
	}
	
	return *this;
}
//////////////////////////////////////////////////////////////////
CncPathListManager& CncPathListManager::operator= (CncPathListManager& plm) {
//////////////////////////////////////////////////////////////////
	return CncPathListManager::operator=((const CncPathListManager)plm);
}
//////////////////////////////////////////////////////////////////
CncPathListManager::~CncPathListManager() {
//////////////////////////////////////////////////////////////////
	wxDELETE(prevCncStartEntry);
	clear();
}
//////////////////////////////////////////////////////////////////
auto CncPathListManager::cFirstPosEntryIterator() const {
//////////////////////////////////////////////////////////////////
	auto it = cbegin();
	while ( it != cend() ) {
		if ( it->isPositionChange() )
			break;
			
		it++;
	}
	
	return it;
}
//////////////////////////////////////////////////////////////////
auto CncPathListManager::crLastPosEntryIterator() const {
//////////////////////////////////////////////////////////////////
	auto it = crbegin();
	while ( it != crend() ) {
		if ( it->isPositionChange() )
			break;
			
		it++;
	}
	
	return it;
}
//////////////////////////////////////////////////////////////////
const char* CncPathListManager::getPathTypeAsStr() const {
//////////////////////////////////////////////////////////////////
	switch ( pathType ) {
		case PT_CNC_PATH: 		return "CNC Path";
		case PT_GUIDE_PATH: 	return "GUIDE Path";
	}
	
	return wxString::Format("%s: ?????", CNC_LOG_FUNCT);
}
//////////////////////////////////////////////////////////////////
CncPathList& CncPathListManager::getPathListIntern() {
//////////////////////////////////////////////////////////////////
	return pathType == PT_CNC_PATH ? pathList : guideList;
}
//////////////////////////////////////////////////////////////////
const CncPathList& CncPathListManager::getPathList() const {
//////////////////////////////////////////////////////////////////
	return pathType == PT_CNC_PATH ? pathList : guideList;
}
//////////////////////////////////////////////////////////////////
unsigned int CncPathListManager::getPathListSize() const {
//////////////////////////////////////////////////////////////////
	return pathType == PT_CNC_PATH ? pathList.size() : guideList.size();
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::initNextGuidePath(GuideType gt) {
//////////////////////////////////////////////////////////////////
	pathType  = PT_GUIDE_PATH;
	guideType = gt == NO_GUIDE_PATH ? HELP_PATH : gt;
	initNextPath();
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::initNextCncPath() {
//////////////////////////////////////////////////////////////////
	pathType  = PT_CNC_PATH;
	guideType = NO_GUIDE_PATH;
	initNextPath();
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::initNextPath() {
//////////////////////////////////////////////////////////////////
	isFirstPath   			= false;
	isCorrected 			= false;
	referencePos			= CncPathListEntry::ZeroTarget;
	totalDistance			= 0.0;
	
	const bool b = pathType == PT_CNC_PATH && prevCncStartEntry != NULL;

	CncPathListEntry cpe;
	cpe.setNothingChanged();

	cpe.entryDistance		= CncPathListEntry::NoDistance;
	cpe.pathListReference	= CncTimeFunctions::getNanoTimestamp();
	
	cpe.clientId			= b ? prevCncStartEntry->clientId         : CncPathListEntry::DefaultClientID;
	cpe.feedSpeedMode		= b ? prevCncStartEntry->feedSpeedMode    : CncPathListEntry::DefaultSpeedMode;
	cpe.feedSpeed_MM_MIN	= b ? prevCncStartEntry->feedSpeed_MM_MIN : CncPathListEntry::DefaultSpeedValue;
	cpe.entryTarget			= b ? prevCncStartEntry->entryTarget      : CncPathListEntry::ZeroTarget;
	
	clear();
	appendEntry(cpe);
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::appendEntry(CncPathListEntry& cpe) {
//////////////////////////////////////////////////////////////////
	// additionally calculate length and distance
	if ( pathList.size() > 0 ) {
		totalDistance += sqrt(  pow(cpe.entryDistance.getX(), 2)
							  + pow(cpe.entryDistance.getY(), 2)
							  + pow(cpe.entryDistance.getZ(), 2)
						 );

		cpe.totalDistance = totalDistance;
	}

	// addionally determine fences
	minPosX = std::min(minPosX, cpe.entryTarget.getX());
	minPosY = std::min(minPosY, cpe.entryTarget.getY());
	minPosZ = std::min(minPosZ, cpe.entryTarget.getZ());

	maxPosX = std::max(maxPosX, cpe.entryTarget.getX());
	maxPosY = std::max(maxPosY, cpe.entryTarget.getY());
	maxPosZ = std::max(maxPosZ, cpe.entryTarget.getZ());
	
	// determine index
	cpe.listIndex = (long)getPathList().size();
	
	// store
	switch ( pathType ) {
		case PT_CNC_PATH: 		pathList.push_back(std::move(cpe));
								
								// prevCncStartEntry contains the start entry of the previous cnc path.
								// it isn't part of the clear method an it must be a deep copy 
								// . . .
								wxDELETE(prevCncStartEntry);
								prevCncStartEntry = new CncPathListEntry(pathList.back());
								break;
								
		case PT_GUIDE_PATH: 	guideList.push_back(std::move(cpe)); 
								break;
	}
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryAdm(long clientId) {
//////////////////////////////////////////////////////////////////
	ASSERT_CHECK_PLML_SIZE_NOT_NULL // will return the defaultEntry on failure

	CncPathListEntry cpe;
	CncPathListEntry& prevEntry	= getPathListIntern().back();

	cpe.setClientIdChange();
	cpe.entryDistance			= CncPathListEntry::NoDistance;
	cpe.clientId				= clientId;
	
	cpe.pathListReference		= prevEntry.pathListReference;
	cpe.alreadyRendered			= prevEntry.alreadyRendered;
	cpe.entryTarget				= prevEntry.entryTarget;
	cpe.feedSpeedMode			= prevEntry.feedSpeedMode;
	cpe.feedSpeed_MM_MIN		= prevEntry.feedSpeed_MM_MIN;

	// append
	return appendEntry(cpe), getPathList().back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryAdm(CncSpeedMode mode, double feedSpeed_MM_MIN) {
//////////////////////////////////////////////////////////////////
	ASSERT_CHECK_PLML_SIZE_NOT_NULL // will return the defaultEntry on failure

	CncPathListEntry cpe;
	CncPathListEntry& prevEntry	= getPathListIntern().back();

	cpe.setSpeedChange();
	cpe.entryDistance			= CncPathListEntry::NoDistance;
	cpe.feedSpeedMode			= mode;
	cpe.feedSpeed_MM_MIN		= feedSpeed_MM_MIN;
	
	cpe.pathListReference		= prevEntry.pathListReference;
	cpe.clientId				= prevEntry.clientId;
	cpe.alreadyRendered			= prevEntry.alreadyRendered;
	cpe.entryTarget				= prevEntry.entryTarget;
	
	// append
	return appendEntry(cpe), getPathList().back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryAbs(double newAbsPosX, double newAbsPosY, double newAbsPosZ, bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	ASSERT_CHECK_PLML_SIZE_NOT_NULL // will return the defaultEntry on failure

	CncPathListEntry cpe(CncPathListEntry::TYPE_POSITION);
	CncPathListEntry& prevEntry	= getPathListIntern().back();

	const double distanceX	= newAbsPosX - prevEntry.entryTarget.getX();
	const double distanceY	= newAbsPosY - prevEntry.entryTarget.getY();
	const double distanceZ	= newAbsPosZ - prevEntry.entryTarget.getZ();

	const long clipperIndex	= (long)clipperPath.size();
	clipperPath.push_back(std::move(ClipperLib::transform(newAbsPosX, newAbsPosY, newAbsPosZ)));
	
	cpe.clipperIndex			= clipperIndex;
	cpe.alreadyRendered			= alreadyRendered;
	cpe.entryTarget				= { newAbsPosX, newAbsPosY, newAbsPosZ };
	cpe.entryDistance			= { distanceX,  distanceY,  distanceZ  };
	
	cpe.pathListReference		= prevEntry.pathListReference;
	cpe.clientId				= prevEntry.clientId;
	cpe.feedSpeedMode			= prevEntry.feedSpeedMode;
	cpe.feedSpeed_MM_MIN		= prevEntry.feedSpeed_MM_MIN;
	
	// append
	return appendEntry(cpe), getPathList().back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryRel(double deltaX, double deltaY, double deltaZ, bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	ASSERT_CHECK_PLML_SIZE_NOT_NULL // will return the defaultEntry on failure

	CncPathListEntry cpe(CncPathListEntry::TYPE_POSITION);
	CncPathListEntry& prevEntry	= getPathListIntern().back();
	
	const double targetX		= prevEntry.entryTarget.getX() + deltaX;
	const double targetY		= prevEntry.entryTarget.getY() + deltaY;
	const double targetZ		= prevEntry.entryTarget.getZ() + deltaZ;
	
	const long clipperIndex	= (long)clipperPath.size();
	clipperPath.push_back(std::move(ClipperLib::transform(targetX, targetY, targetZ)));

	cpe.setPositionChange();

	cpe.clipperIndex			= clipperIndex;
	cpe.alreadyRendered			= alreadyRendered;
	cpe.entryDistance			= { deltaX,  deltaY,  deltaZ  };
	cpe.entryTarget				= { targetX, targetY, targetZ };
	
	cpe.pathListReference		= prevEntry.pathListReference;
	cpe.clientId				= prevEntry.clientId;
	cpe.feedSpeedMode			= prevEntry.feedSpeedMode;
	cpe.feedSpeed_MM_MIN		= prevEntry.feedSpeed_MM_MIN;
	
	// append
	return appendEntry(cpe), getPathList().back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryAbs(const CncDoublePosition& newAbsPos, bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	return addEntryAbs(	newAbsPos.getX(), 
						newAbsPos.getY(),
						newAbsPos.getZ(),
						alreadyRendered
					  );
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryRel(const CncDoublePosition& distance, bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	return addEntryRel(	distance.getX(), 
						distance.getY(),
						distance.getZ(),
						alreadyRendered
					  );
}
//////////////////////////////////////////////////////////////////
std::ostream& CncPathListManager::outputOperator(std::ostream &ostr) const {
//////////////////////////////////////////////////////////////////
	const CncDoublePosition& rp = getReferencePos();
	const CncDoublePosition& sp = getStartPos();
	
	ostr 	<< "CncPathListInfo entries : " << getPathList().size()											<< std::endl
			<< " Path Type              : " << getPathTypeAsStr()											<< std::endl
			<< " Is closed              : " << isPathClosed()												<< std::endl
			<< " Is corrected           : " << isPathCorrected()											<< std::endl
			<< " Is first Path          : " << isPathFlaggedAsFirst()										<< std::endl
			<< " Total Distance         : " << cnc::dblFormat1(getTotalDistance()) 							<< std::endl
			<< " Min Pos (x, y, z)      : " << cnc::dblFormat3(getMinPosX(), getMinPosY(), getMinPosZ())	<< std::endl
			<< " Max Pos (x, y, z)      : " << cnc::dblFormat3(getMaxPosX(), getMaxPosY(), getMaxPosZ())	<< std::endl
			<< " Reference Pos          : " << cnc::dblFormat3(rp.getX(),    rp.getY(),    rp.getZ())		<< std::endl
			<< " Start Pos              : " << cnc::dblFormat3(sp.getX(),    sp.getY(),    sp.getZ())		<< std::endl
			<< " Entries                : " << std::endl
			;
	
	for ( auto it = getPathList().begin(); it != getPathList().end(); ++it )
		it->traceEntry(ostr);
	
	return ostr;
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::clear() {
//////////////////////////////////////////////////////////////////
	getPathListIntern().clear();
	clipperPath.clear();
	resetStatistics();
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::resetStatistics() {
//////////////////////////////////////////////////////////////////
	minPosX = minPosY = minPosZ = DBL_MAX;
	maxPosX = maxPosY = maxPosZ = DBL_MIN;
	
	totalDistance = 0.0;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition& CncPathListManager::getStartPos() const {
//////////////////////////////////////////////////////////////////
	static CncDoublePosition p(0.0, 0.0, 0.0);
	
	if ( getPathList().size() > 0 )
		return cbegin()->entryTarget;
	
	return p; 
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::isPathClosed() const {
//////////////////////////////////////////////////////////////////
	if ( getPathListSize() == 0 )
		return false;
		
	auto itFirst = cFirstPosEntryIterator();
	if ( itFirst == cend() )
		return false;
	
	auto itLast = crLastPosEntryIterator();
	if ( itLast == crend() )
		return false;
	
	const CncDoublePosition& p1 = itFirst->entryTarget;
	const CncDoublePosition& p2 = itLast->entryTarget;
	
	return p1 == p2;
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::changeToGuideType(GuideType gt) {
//////////////////////////////////////////////////////////////////
	if ( pathType != PT_GUIDE_PATH )
		guideList.swap(pathList);
		
	pathType  = PT_GUIDE_PATH;
	guideType = gt;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::roundCorners(double toolDiameter) {
//////////////////////////////////////////////////////////////////
	if ( cnc::dblCompareNull(toolDiameter) )
		return true;
		
	const double offset = toolDiameter / 2.0;
	
	bool ret = processOffset(-offset, jtRound, etClosedPolygon);
	if ( ret == false )
		return false;
	 
	return processOffset(+offset , jtRound, etClosedPolygon);
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::processInnerOffset(double offset) {
//////////////////////////////////////////////////////////////////
	// to be always negative
	const double o = -abs(offset);
	return processOffset(o, jtRound, etClosedPolygon);
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::processOuterOffset(double offset) {
//////////////////////////////////////////////////////////////////
	// to be always positive
	const double o = abs(offset);
	return processOffset(o, jtMiter, etClosedPolygon);
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::processOffset(double offset, JoinType jt, EndType et) {
//////////////////////////////////////////////////////////////////
	
	// -----------------------------------------------------------
	// perform some checks
	if ( isPathClosed() == false ) {
		std::cerr << CNC_LOG_FUNCT_A(" Can't process. Path isn't closed!\n");
		return false;
	}
	
	// offset has to be given in the same unit as the path itself
	if ( abs(offset) > 50 ) {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" Can't process. Offset(%ld) is to great\n", offset));
		return false;
	}
	
	ClipperLib::ClipperOffset co;
	ClipperLib::Paths result;
	co.AddPath(clipperPath, transform(jt), transform(et));
	co.Execute(result, ClipperLib::convert(offset));
	
	getPathListIntern().erase(cFirstPosEntryIterator(), cend());
	clipperPath.clear();
	resetStatistics();
	
	for (auto it=result.begin(); it != result.end(); ++it) {
		
		ClipperLib::Path& path = *it;
		closePath(path);
		
		for (auto itP=path.begin(); itP != path.end(); ++itP) {
			addEntryAbs(ClipperLib::asCncDoublePosition(*itP), false);
			
			#warning 4242.42
			if ( std::distance(path.begin(), itP) == 0 )
				addEntryAdm(CncSpeedWork, 4242.42);
		}
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::reversePath() {
//////////////////////////////////////////////////////////////////
	// empty or single point, nothing should happen
	if ( std::distance(cFirstPosEntryIterator(), cend()) < 2 )
		return true;
	
	// reverse
	ClipperLib::ReversePath(clipperPath);
	
	getPathListIntern().erase(cFirstPosEntryIterator(), cend());
	clipperPath.clear();
	resetStatistics();
	
	for (auto it=clipperPath.begin(); it != clipperPath.end(); ++it) {
		
		addEntryAbs(ClipperLib::asCncDoublePosition(*it), false);
		
		#warning 4242.42
		if ( std::distance(clipperPath.begin(), it) == 0 )
			addEntryAdm(CncSpeedWork, 4242.42);
		
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::processPocket(double toolDiameter) {
//////////////////////////////////////////////////////////////////
	
	#warning 4242.42
	const double	workSpeed				= 4242.42;
	const double	firstOffset				= - ( abs(toolDiameter) / 2.0 );		// CncCT_Inner (shrinking)-> value has to negative!
	const double	nextOffset				= - ( abs(toolDiameter) / 2.0 * 0.3 );	// CncCT_Inner (shrinking)-> value has to negative!
	const double 	maxOffset				= 0.01;
	
	const unsigned	maxRecursionDeep		= 128;
	unsigned int	callCounter				= 0;
	
	// -----------------------------------------------------------
	// perform some checks
	if ( abs(toolDiameter) > 25.0 )  {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" Tool diameter=%ld to great. The pocket whole creation is stopped here!\n", abs(toolDiameter)));
		return false;
	}
	
	if ( firstOffset > maxOffset ) {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" Invalid first offset=%ld. The pocket whole creation is stopped here!\n", firstOffset));
		return false;
	}
	
	if ( nextOffset > maxOffset ) {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" Invalid next offset=%ld. The pocket whole creation is stopped here!\n", nextOffset));
		return false;
	}
	
	if ( isPathClosed() == false ) {
		std::cerr << CNC_LOG_FUNCT_A(" Path isn't closed. The pocket whole creation is stopped here!\n");
		return false;
	}
	
	// -----------------------------------------------------------
	// add the given path information
	auto addPath = [&](ClipperLib::Path& path, bool first) {
		
		ClipperLib::closePath(path);
		
		for (auto it = path.begin(); it != path.end(); ++it) {
			// add position entry
			addEntryAbs(ClipperLib::asCncDoublePosition(*it), false);
			
			// add speed entry
			if ( first && std::distance(path.begin(), it) == 0 )
				addEntryAdm(CncSpeedWork, workSpeed);
		}
	};
	
	// -----------------------------------------------------------
	// perform further shrink offsets until no more results are available
	// note: this lambda is called recursive
	std::function<void(ClipperLib::Path&, bool)>
	nextRound = [&](ClipperLib::Path& path, bool first) {
		
		// first add the given path
		addPath(path, first);
		
		// final safty - breaks the recursion
		if ( ++callCounter > maxRecursionDeep ) {
			std::cerr << CNC_LOG_FUNCT_A(" Max recursion deep reached. The pocket whole creation is stopped here!\n");
			return;
		}
		
		// next inner correction (shrinking)
		ClipperLib::ClipperOffset co;
		ClipperLib::Paths result;
		co.AddPath(path, ClipperLib::jtRound, ClipperLib::etClosedPolygon);
		co.Execute(result, ClipperLib::convert(nextOffset));
		
		// recursion break condition
		if ( result.size() == 0 )
			return;
		
		// note: the result can be splitted into moe the one path
		for (auto it=result.begin(); it != result.end(); ++it) {
			ClipperLib::Path& nPath = *it;
			
			// recursion call
			nextRound(nPath, false);
		}
	};
	
	// -----------------------------------------------------------
	// start point - first inner shrinking
	ClipperLib::ClipperOffset co;
	ClipperLib::Paths result;
	co.AddPath(clipperPath, ClipperLib::jtRound, ClipperLib::etClosedPolygon);
	co.Execute(result, ClipperLib::convert(firstOffset));
	
	if ( result.size() == 0 ) {
		std::cerr << CNC_LOG_FUNCT_A(" The first step don't deliver a result. The pocket whole creation is stopped here!\n");
		return false;
	}
	
	// clear the curent path position information
	getPathListIntern().erase(cFirstPosEntryIterator(), cend());
	clipperPath.clear();
	resetStatistics();
	
	// note: the result can be splitted into more the one path
	for (auto it=result.begin(); it != result.end(); ++it) {
		ClipperLib::Path& path = *it;
		
		// start the recursion . . .
		nextRound(path, true);
	}
	
	return true;
}










//////////////////////////////////////////////////////////////////
bool CncPathListManager::eraseEntryAndRecalcuate(const CncPathList::iterator& itToErase) {
//////////////////////////////////////////////////////////////////
	/*
	if ( itToErase < begin() )
		return false;

	if ( itToErase >= end() )
		return false;
		
	// store entry 
	CncPathListEntry entry = *itToErase;
	
	// check first position
	bool first = itToErase == begin() ? true : false;
	
	// remove entry
	if ( getPathListIntern().erase(itToErase) == end() )
		return false;
		
	// nothing further to do if the pathList is now empty
	if ( getPathList().size() == 0 )
		return true;
	
	// redetermine additional values
	if ( first == true ) {
		begin()->entryDistance		= begin()->entryTarget - referencePos;
		begin()->alreadyRendered 	= true;
	}
	
	// set recalculateMinMax flag
	bool recalculateMinMax = false;
	if ( cnc::dblCompare(entry.entryTarget.getX(), minPosX) ||
		 cnc::dblCompare(entry.entryTarget.getY(), minPosY) ||
		 cnc::dblCompare(entry.entryTarget.getZ(), minPosZ) ||

		 cnc::dblCompare(entry.entryTarget.getY(), maxPosX) ||
		 cnc::dblCompare(entry.entryTarget.getY(), maxPosY) ||
	     cnc::dblCompare(entry.entryTarget.getZ(), maxPosZ)) {
		
		recalculateMinMax = true;
		resetMinMax();
	}

	// recalculate length and min or max on demand
	totalDistance = 0.0;
	unsigned int cnt = 0;
	for ( auto it=begin(); it !=end(); ++it ) {
		
		if ( cnt > 0 ) {
			totalDistance += sqrt(  pow(it->entryDistance.getX(), 2)
					              + pow(it->entryDistance.getY(), 2)
								  + pow(it->entryDistance.getZ(), 2)
							 );



			it->totalDistance = totalDistance;
		}
		else {

			it->totalDistance = 0.0;
		}
		
		if ( recalculateMinMax == true ) {
			minPosX = std::min(minPosX, it->entryTarget.getX());
			minPosY = std::min(minPosY, it->entryTarget.getY());
			minPosZ = std::min(minPosZ, it->entryTarget.getZ());

			maxPosX = std::max(maxPosX, it->entryTarget.getX());
			maxPosY = std::max(maxPosY, it->entryTarget.getY());
			maxPosZ = std::max(maxPosZ, it->entryTarget.getZ());
		}
		
		cnt++;
	}
	*/
	return true;
}



void CncPathListManager::testSomething() {
	
	
	ClipperLib::ClipperOffset co;
	ClipperLib::Paths solution;
	co.AddPath(clipperPath, ClipperLib::jtRound, ClipperLib::etClosedPolygon);
	co.Execute(solution, ClipperLib::convert(-10.0));
	
	
	for (auto it=solution.begin(); it != solution.end(); ++it) {
		std::cout << "CL solution: " << std::distance(solution.begin(), it) << std::endl;
		
		const ClipperLib::Path& path = *it;
		
		for (auto itP=path.begin(); itP != path.end(); ++itP) {
			std::cout	<< *itP
						<< " --> "
						<< ClipperLib::asCncDoublePosition(*itP)
						<< std::endl
						;
		}
	}
}
