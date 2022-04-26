#include "CncConfig.h"
#include "CncClipperLib.h"
#include "CncPathListManager.h"

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
: pathList					()
, guideList					()
, clipperPath				()
, pathType					(PT_CNC_PATH)
, guideType 				(NO_GUIDE_PATH)
, defaultEntry				()
, executionRecommend		(ER_STAIRS)
, notNormalizeStartDistance	(0.0, 0.0, 0.0)
, minPosX					(0.0)
, minPosY					(0.0)
, minPosZ					(0.0)
, maxPosX					(0.0)
, maxPosY					(0.0)
, maxPosZ					(0.0)
, totalDistance				(0.0)
//////////////////////////////////////////////////////////////////
{
	// reservate memory
	pathList .reserve(THE_CONFIG->getConstReserveCapacity());
	guideList.reserve(THE_CONFIG->getConstReserveCapacity());
	
	// create initial entry
	initNextPath(false);
}
//////////////////////////////////////////////////////////////////
CncPathListManager::CncPathListManager(const CncPathListEntry& initialEntry)
: pathList					()
, guideList					()
, clipperPath				()
, pathType					(PT_CNC_PATH)
, guideType 				(NO_GUIDE_PATH)
, defaultEntry				()
, executionRecommend		(ER_STAIRS)
, notNormalizeStartDistance	(0.0, 0.0, 0.0)
, minPosX					(0.0)
, minPosY					(0.0)
, minPosZ					(0.0)
, maxPosX					(0.0)
, maxPosY					(0.0)
, maxPosZ					(0.0)
, totalDistance				(0.0)
//////////////////////////////////////////////////////////////////
{
	// reservate memory
	pathList .reserve(THE_CONFIG->getConstReserveCapacity());
	guideList.reserve(THE_CONFIG->getConstReserveCapacity());
	
	// create initial entry
	initNextPath(initialEntry);
}
//////////////////////////////////////////////////////////////////
CncPathListManager::CncPathListManager(const CncPathListManager& plm) 
: pathList					(plm.getProtectedPathList())
, guideList					(plm.getProtectedGuideList())
, clipperPath				(plm.getProtectedClipperPath())
, pathType					(plm.getPathType())
, guideType 				(plm.getGuideType())
, defaultEntry				(plm.getDefaultEntry())
, executionRecommend		(plm.getExecRecommendation())
, notNormalizeStartDistance	(plm.getNorNormalizedStartDist())
, minPosX					(plm.getMinPosX())
, minPosY					(plm.getMinPosY())
, minPosZ					(plm.getMinPosZ())
, maxPosX					(plm.getMaxPosX())
, maxPosY					(plm.getMaxPosY())
, maxPosZ					(plm.getMaxPosZ())
, totalDistance				(plm.getTotalDistance())
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
CncPathListManager::CncPathListManager(CncPathListManager& plm) 
: pathList					(plm.getProtectedPathList())
, guideList					(plm.getProtectedGuideList())
, clipperPath				(plm.getProtectedClipperPath())
, pathType					(plm.getPathType())
, guideType 				(plm.getGuideType())
, defaultEntry				(plm.getDefaultEntry())
, executionRecommend		(plm.getExecRecommendation())
, notNormalizeStartDistance	(plm.getNorNormalizedStartDist())
, minPosX					(plm.getMinPosX())
, minPosY					(plm.getMinPosY())
, minPosZ					(plm.getMinPosZ())
, maxPosX					(plm.getMaxPosX())
, maxPosY					(plm.getMaxPosY())
, maxPosZ					(plm.getMaxPosZ())
, totalDistance				(plm.getTotalDistance())
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
CncPathListManager& CncPathListManager::operator=(const CncPathListManager& plm) {
//////////////////////////////////////////////////////////////////
	// self-assignment check
	if ( this != &plm ) {
		
		pathList				= plm.getProtectedPathList();
		guideList				= plm.getProtectedGuideList();
		clipperPath				= plm.getProtectedClipperPath();
 		pathType				= plm.getPathType();
		guideType 				= plm.getGuideType();
		defaultEntry			= plm.getDefaultEntry();
		executionRecommend		= plm.getExecRecommendation();
		minPosX					= plm.getMinPosX();
		minPosY					= plm.getMinPosY();
		minPosZ					= plm.getMinPosZ();
		maxPosX					= plm.getMaxPosX();
		maxPosY					= plm.getMaxPosY();
		maxPosZ					= plm.getMaxPosZ();
		totalDistance			= plm.getTotalDistance();
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
	clear();
}
//////////////////////////////////////////////////////////////////
auto CncPathListManager::firstPosEntryIterator() {
//////////////////////////////////////////////////////////////////
	auto it = begin();
	while ( it != end() ) {
		if ( it->isPositionChange() )
			break;
			
		it++;
	}
	
	return it;
}
//////////////////////////////////////////////////////////////////
auto CncPathListManager::lastPosEntryIterator() {
//////////////////////////////////////////////////////////////////
	auto it = rbegin();
	while ( it != rend() ) {
		if ( it->isPositionChange() )
			break;
			
		it++;
	}
	
	return it;
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
bool CncPathListManager::hasMovement() const { 
//////////////////////////////////////////////////////////////////
	return cFirstPosEntryIterator() != cend(); 
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
	initNextPath(true);
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::initNextCncPath() {
//////////////////////////////////////////////////////////////////
	pathType  = PT_CNC_PATH;
	guideType = NO_GUIDE_PATH;
	initNextPath(true);
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::initNextPath(const CncPathListEntry& initialEntry) {
//////////////////////////////////////////////////////////////////
	totalDistance			= 0.0;
	
	clear();
	
	CncPathListEntry cpe(initialEntry);
	appendEntry(cpe);
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::initNextPath(bool linked) {
//////////////////////////////////////////////////////////////////
	totalDistance			= 0.0;
	
	CncPathListEntry cpe;
	cpe.setNothingChanged();

	cpe.entryDistance		= CncPathListEntry::NoDistance;
	cpe.pathListReference	= CncTimeFunctions::getNanoTimestamp();
	
	auto itLast = crLastPosEntryIterator();
	const bool b = (pathType == PT_CNC_PATH && itLast != crend());

	cpe.clientId			= b ? itLast->clientId         : CncPathListEntry::DefaultClientID;
	cpe.feedSpeedMode		= b ? itLast->feedSpeedMode    : CncPathListEntry::DefaultSpeedMode;
	cpe.feedSpeed_MM_MIN	= b ? itLast->feedSpeed_MM_MIN : CncPathListEntry::DefaultSpeedValue;
	cpe.entryTarget			= b ? itLast->entryTarget      : CncPathListEntry::ZeroTarget;

	clear();
	
	if ( linked == true )
		appendEntry(cpe);
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::appendEntry(CncPathListEntry& cpe) {
//////////////////////////////////////////////////////////////////
	// additionally calculate length and distance
	if ( cpe.hasPositionChange() )
	{
		totalDistance += sqrt(  pow(cpe.entryDistance.getX(), 2)
							  + pow(cpe.entryDistance.getY(), 2)
							  + pow(cpe.entryDistance.getZ(), 2)
						 );

		cpe.totalDistance = totalDistance;
	}

	// additionally determine fences
	minPosX = std::min(minPosX, cpe.entryTarget.getX());
	minPosY = std::min(minPosY, cpe.entryTarget.getY());
	minPosZ = std::min(minPosZ, cpe.entryTarget.getZ());

	maxPosX = std::max(maxPosX, cpe.entryTarget.getX());
	maxPosY = std::max(maxPosY, cpe.entryTarget.getY());
	maxPosZ = std::max(maxPosZ, cpe.entryTarget.getZ());
	
	// determine index
	cpe.listIndex = (long)getPathList().size();
	
	// store
	switch ( pathType )
	{
		case PT_CNC_PATH: 		pathList.push_back(std::move(cpe));
								break;
								
		case PT_GUIDE_PATH: 	guideList.push_back(std::move(cpe)); 
								break;
	}
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryAdm(long clientId) {
//////////////////////////////////////////////////////////////////
	CncPathListEntry cpe;
	CncPathListEntry& prevEntry	= getPathListSize() ? getPathListIntern().back() : defaultEntry;

	cpe.setClientIdChange();
	cpe.entryDistance			= CncPathListEntry::NoDistance;
	cpe.clientId				= clientId;
	
	cpe.pathListReference		= prevEntry.pathListReference;
	cpe.alreadyRendered			= prevEntry.alreadyRendered;
	cpe.entryTarget				= prevEntry.entryTarget;
	cpe.feedSpeedMode			= prevEntry.feedSpeedMode;
	cpe.feedSpeed_MM_MIN		= prevEntry.feedSpeed_MM_MIN;
	cpe.spindleState			= prevEntry.spindleState;
	cpe.spindleSpeed_U_MIN		= prevEntry.spindleSpeed_U_MIN;

	// append
	return appendEntry(cpe), getPathList().back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryAdm(CncSpeedMode mode, double feedSpeed_MM_MIN) {
//////////////////////////////////////////////////////////////////
	CncPathListEntry cpe;
	CncPathListEntry& prevEntry	= getPathListSize() ? getPathListIntern().back() : defaultEntry;
	
	cpe.setSpeedChange();
	cpe.entryDistance			= CncPathListEntry::NoDistance;
	cpe.feedSpeedMode			= mode;
	cpe.feedSpeed_MM_MIN		= feedSpeed_MM_MIN;
	
	cpe.pathListReference		= prevEntry.pathListReference;
	cpe.clientId				= prevEntry.clientId;
	cpe.alreadyRendered			= prevEntry.alreadyRendered;
	cpe.entryTarget				= prevEntry.entryTarget;
	cpe.spindleState			= prevEntry.spindleState;
	cpe.spindleSpeed_U_MIN		= prevEntry.spindleSpeed_U_MIN;
	
	// append
	return appendEntry(cpe), getPathList().back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntrySpl(bool spindleState) {
//////////////////////////////////////////////////////////////////
	CncPathListEntry& prevEntry	= getPathListSize() ? getPathListIntern().back() : defaultEntry;
	return addEntrySpl(spindleState, prevEntry.spindleSpeed_U_MIN);
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntrySpl(double spindleSpeed_U_MIN) {
//////////////////////////////////////////////////////////////////
	CncPathListEntry& prevEntry	= getPathListSize() ? getPathListIntern().back() : defaultEntry;
	return addEntrySpl(prevEntry.spindleState, spindleSpeed_U_MIN);
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntrySpl(bool spindleState, double spindleSpeed_U_MIN) {
//////////////////////////////////////////////////////////////////
	CncPathListEntry cpe;
	CncPathListEntry& prevEntry	= getPathListSize() ? getPathListIntern().back() : defaultEntry;
	
	cpe.setToolChange();
	cpe.entryDistance			= CncPathListEntry::NoDistance;
	cpe.spindleState			= spindleState;
	cpe.spindleSpeed_U_MIN		= spindleSpeed_U_MIN;
	
	cpe.pathListReference		= prevEntry.pathListReference;
	cpe.clientId				= prevEntry.clientId;
	cpe.alreadyRendered			= prevEntry.alreadyRendered;
	cpe.entryTarget				= prevEntry.entryTarget;
	cpe.feedSpeedMode			= prevEntry.feedSpeedMode;
	cpe.feedSpeed_MM_MIN		= prevEntry.feedSpeed_MM_MIN;
	
	// append
	return appendEntry(cpe), getPathList().back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryAbs(double newAbsPosX, double newAbsPosY, double newAbsPosZ, bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	CncPathListEntry cpe(CncPathListEntry::TYPE_POSITION);
	CncPathListEntry& prevEntry	= getPathListSize() ? getPathListIntern().back() : defaultEntry;

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
	cpe.spindleState			= prevEntry.spindleState;
	cpe.spindleSpeed_U_MIN		= prevEntry.spindleSpeed_U_MIN;
	
	// append
	return appendEntry(cpe), getPathList().back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryRel(double deltaX, double deltaY, double deltaZ, bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	CncPathListEntry cpe(CncPathListEntry::TYPE_POSITION);
	CncPathListEntry& prevEntry	= getPathListSize() ? getPathListIntern().back() : defaultEntry;
	
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
	cpe.spindleState			= prevEntry.spindleState;
	cpe.spindleSpeed_U_MIN		= prevEntry.spindleSpeed_U_MIN;
	
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
size_t CncPathListManager::normalizeLinkedEntry(long clientId, CncSpeedMode mode, double feedSpeed_MM_MIN) {
//////////////////////////////////////////////////////////////////
	if ( getPathListSize() == 0 )
		return 0;
		
	CncPathListEntry& firstEntry = *(getPathListIntern().begin());
	if ( firstEntry.isNothingChanged() == false )
		;//return 0;
	
	for ( auto it = begin(); it != end(); ++it ) {
		CncPathListEntry& ple 	 = *it;
		
		ple.clientId			 = clientId;
		ple.feedSpeedMode		 = mode;
		ple.feedSpeed_MM_MIN	 = feedSpeed_MM_MIN;
		
		if ( std::distance(begin(), it) == 0 )
			ple.content |= CncPathListEntry::CONT_CLIENTID | CncPathListEntry::CONT_SPEED;
	}
	
	return getPathListSize();
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::init(const CncDoublePosition& p) {
//////////////////////////////////////////////////////////////////
	// first clear the content
	clear();
	
	// .. and then create the first entry based on the given 
	// current position
	CncPathListEntry initialEntry;
	initialEntry.content			= CncPathListEntry::CONT_POSITION;
	initialEntry.pathListReference	= CncTimeFunctions::getNanoTimestamp();
	initialEntry.entryDistance		= CncPathListEntry::NoDistance;
	initialEntry.entryTarget		= p;
	initialEntry.clientId			= 0;
	initialEntry.feedSpeedMode		= CncPathListEntry::DefaultSpeedMode;
	initialEntry.feedSpeed_MM_MIN	= CncPathListEntry::DefaultSpeedValue;
	initialEntry.spindleState		= CncPathListEntry::DefaultSpindleState;
	initialEntry.spindleSpeed_U_MIN	= CncPathListEntry::DefaultSpindleSpeedValue;
	
	initNextPath(initialEntry);
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
	minPosX = minPosY = minPosZ = cnc::dbl::MAX;
	maxPosX = maxPosY = maxPosZ = cnc::dbl::MIN;
	
	totalDistance				= 0.0;
}
//////////////////////////////////////////////////////////////////
long CncPathListManager::firstClientID() const {
//////////////////////////////////////////////////////////////////
	if ( getPathList().size() > 0 )
		return cFirstPosEntryIterator()->clientId;
		
	return CLIENT_ID.INVALID;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition& CncPathListManager::getStartPos() const {
//////////////////////////////////////////////////////////////////
	static CncDoublePosition p(0.0, 0.0, 0.0);
	
	if ( getPathList().size() > 0 )
		return cFirstPosEntryIterator()->entryTarget;
	
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
	
	return p1.isFloatingEqual(p2, 0.001);
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::getTargetPos(CncDoublePosition& ret) const {
//////////////////////////////////////////////////////////////////
	auto itLast = crLastPosEntryIterator();
	if ( itLast == crend() )
		return false;
		
	ret = itLast->entryTarget;
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::isRightTriangle(CncDoublePosition& ret) const {
//////////////////////////////////////////////////////////////////
	// non closed can be an triangle
	if ( isPathClosed() == false )
		return false;
	
	// not enough points for an triangle
	if ( clipperPath.size() < 4 )
		return false;
		
	// using the clipper path to search for a right angle 
	// because it contains only positions
	
	bool findRightAngle = false;
	for ( auto it = clipperPath.begin() + 2; it != clipperPath.end(); ++it )
	{
		const ClipperLib::IntPoint p1 = *(it - 2 );
		const ClipperLib::IntPoint p2 = *(it - 1 );
		const ClipperLib::IntPoint p3 = *(it - 0 );
		
		const float dx1 = ClipperLib::transform(p2.X - p1.X);
		const float dx2 = ClipperLib::transform(p3.X - p2.X);
		const float dy1 = ClipperLib::transform(p2.Y - p1.Y);
		const float dy2 = ClipperLib::transform(p3.Y - p2.Y);
		
		const float a1 = atan2(dx1, dy1);
		const float a2 = atan2(dx2, dy2);
		
		// is it 90' ?
		if ( cnc::fltCmp::eq(fabs(a1 -a2), PI /2.0) )
		{
			//cnc::cex3 << "a1 - a2; " << (a1 - a2) << ", " << (a1 - a2) * 180 / PI << std::endl;
			//cnc::cex3 << wxString::Format("%ld, %ld, %ld\n", p2.X, p2.Y, p2.Z);
			
			ret = ClipperLib::asCncDoublePosition(p2);
			findRightAngle = true;
			break;
		}
	}
	
	return findRightAngle;
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
CncDirection CncPathListManager::getOrientation(ClipperLib::Path& path) const {
//////////////////////////////////////////////////////////////////
	// On Y-axis positive upward displays,   Orientation will return true if the polygon's orientation is counter-clockwise.
	// On Y-axis positive downward displays, Orientation will return true if the polygon's orientation is clockwise.
	const bool orientation = ClipperLib::Orientation(path);
	
	CncDirection curDirection = CncUndefDir;
	if ( THE_CONFIG->getSvgConvertToRightHandFlag() )	curDirection = (orientation ? CncCounterClockwise : CncClockwise);
	else												curDirection = (orientation ? CncClockwise        : CncCounterClockwise);
	
	return curDirection;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::ensureDirection(CncDirection d, ClipperLib::Path& path) const {
//////////////////////////////////////////////////////////////////
	if ( d == CncUndefDir )
		return false;
	
	if ( getOrientation(path) != d )
		ClipperLib::ReversePath(path);
		
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::ensureDirection(CncDirection d) {
//////////////////////////////////////////////////////////////////
	// empty or single point, nothing should happen
	if ( std::distance(cFirstPosEntryIterator(), cend()) < 2 )
		return true;
	
	// nothing to do
	if ( getOrientation(clipperPath) == d )
		return true;
	
	return reversePath();
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::reversePath() {
//////////////////////////////////////////////////////////////////
	// empty or single point, nothing should happen
	if ( std::distance(cFirstPosEntryIterator(), cend()) < 2 )
		return true;
	
	// entry point: reverse
	ClipperLib::Path reversedPath = clipperPath;
	ClipperLib::ReversePath(reversedPath);
	
	getPathListIntern().erase(cFirstPosEntryIterator(), cend());
	clipperPath.clear();
	resetStatistics();
	
	for (auto it=reversedPath.begin(); it != reversedPath.end(); ++it)
		addEntryAbs(ClipperLib::asCncDoublePosition(*it), false);
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::adjustZOffsetAbs(double offset) {
//////////////////////////////////////////////////////////////////
	return adjustZOffset(offset, false);
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::adjustZOffsetRel(double offset) {
//////////////////////////////////////////////////////////////////
	return adjustZOffset(offset, true);
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::adjustZOffset(double offset, bool relative) {
//////////////////////////////////////////////////////////////////
	// optimization
	if ( relative == true && cnc::dblCmp::nu(offset) == true )
		return true;
		
	// This method only adjusts the Z target value. It also expects that the given Z value 
	// is already reached outside thi method. Therefore, the total distance leaves unchanged, 
	// only the z related min/max values have to be adjusted. 
	minPosZ			= 0.0;
	maxPosZ			= 0.0;
	
	for ( auto it = begin(); it != end(); ++it) {
		CncPathListEntry& pe1 = *it;
		
		if ( pe1.hasPositionChange() == true ) {
			
			if ( relative == true ) {
				pe1.entryTarget.incZ(offset);
			}
			else {
				// optimization
				if ( pe1.entryTarget.getZ() == offset )
					continue;
					
				pe1.entryTarget.setZ(offset);
			}
			
			minPosZ = std::min(minPosZ, pe1.entryTarget.getZ());
			maxPosZ = std::max(maxPosZ, pe1.entryTarget.getZ());
		}
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::processXYHelix(double zBegDepth, double zTotDepth, double zFeed360) {
//////////////////////////////////////////////////////////////////
	if ( cnc::dblCmp::le(zFeed360, 0.0) ) {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" Invalid z feed parameter: %.3lf", zFeed360));
		return false;
	}
	
	// ----------------------------------------------------------
	auto getPositionCount = [&]() {
		if ( true ) {
			return getPathListSize();
		}
		
		unsigned int c = 0;
		for ( auto it = begin(); it != end(); ++it) {
			CncPathListEntry& pe = *it;
			
			if ( pe.hasPositionChange() == true ) {
				c++;
			}
		}
		return c;
	};
	
	// ----------------------------------------------------------
	// entry point
	const long pathSteps360	= getPositionCount();
	const double stepSize	= -(zFeed360 / pathSteps360);
	
	CncPathListManager source(*this);
	
	double zCurDepth		= zBegDepth;
	unsigned int duration	= 1;
	
	do
	{
		double zCurStop = std::max(zBegDepth - (zFeed360 * duration), zTotDepth );
		
		// modify the exiting path to go the first helix step down
		if ( duration == 1 ) {
			
			for ( auto it = begin(); it != end(); ++it) {
				CncPathListEntry& pe = *it;
				
				if ( pe.hasPositionChange() == true ) {
					zCurDepth += stepSize;
					
					if ( cnc::dblCmp::lt(zCurDepth, zCurStop) ) {
						pe.entryDistance.setZ(stepSize - abs(zCurStop - zCurDepth) );
						pe.entryTarget.setZ(zCurStop);
					}
					else {
						pe.entryDistance.setZ(stepSize);
						pe.entryTarget.setZ(zCurDepth);
					}
				}
			}
		}
		// append further helix steps down to reach the final depth
		else {
			
			for ( auto it = source.begin() + 1; it != source.end(); ++it) {
				const CncPathListEntry& pe = *it;
				
				if ( cnc::dblCmp::nu(pe.entryDistance.getX()) && cnc::dblCmp::nu(pe.entryDistance.getY()) )
					continue;
				
				if ( pe.hasPositionChange() == true ) {
					
					zCurDepth += stepSize;
					
					if ( cnc::dblCmp::lt(zCurDepth, zCurStop) )	addEntryAbs(pe.entryTarget.getX(), pe.entryTarget.getY(), zCurStop);
					else										addEntryAbs(pe.entryTarget.getX(), pe.entryTarget.getY(), zCurDepth);
				}
			}
		}
		
		duration++;
		
	} while ( cnc::dblCmp::gt(zCurDepth, zTotDepth) );
	
	// last try to step minimal to a inner radius before finalize 
	// and moving up again (outside this method)
	source.processXYOffset(-0.5);
	if ( source.getPathListSize() > 1 ) {
		const CncPathListEntry& pe = *(source.begin() + 1);
		addEntryAbs(pe.entryTarget.getX(), pe.entryTarget.getY(), zTotDepth);
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::roundXYCorners(double toolDiameter) {
//////////////////////////////////////////////////////////////////
	if ( cnc::dblCompareNull(toolDiameter) )
		return true;
		
	const double offset = toolDiameter / 2.0;
	
	// this is a two step process, first step to inner and 
	// the the same distance outer again
	bool ret = processXYOffset(-offset, jtRound, etClosedPolygon);
	if ( ret == false )
		return false;
	
	return processXYOffset(+offset , jtRound, etClosedPolygon);
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::processXYInnerOffset(double offset) {
//////////////////////////////////////////////////////////////////
	// to be always negative
	const double o = -abs(offset);
	return processXYOffset(o, jtRound, etClosedPolygon);
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::processXYOuterOffset(double offset) {
//////////////////////////////////////////////////////////////////
	// to be always positive
	const double o = abs(offset);
	return processXYOffset(o, jtMiter, etClosedPolygon);
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::processXYOffset(double offset, JoinType jt, EndType et) {
//////////////////////////////////////////////////////////////////
	if ( hasMovement() == false )
		return true;
		
	// -----------------------------------------------------------
	// perform some checks
	if ( isPathClosed() == false ) {
		std::cerr << CNC_LOG_FUNCT_A(" Can't process. Path isn't closed!\n");
		return false;
	}
	
	// offset has to be given in the same unit as the path itself
	if ( cnc::dblCompareNull(offset) )  {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" Can't process. A offset of %.3lf is not sufficient!\n", fabs(offset)));
		return false;
	}
	
	// offset has to be given in the same unit as the path itself
	if ( fabs(offset) > 20.0 ) {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" Can't process. A offset of %.3lf is to great\n", fabs(offset)));
		return false;
	}
	
	// entry point
	const CncDirection finalDirection = offset < 0.0 ? CncCounterClockwise : CncClockwise;
	
	ClipperLib::ClipperOffset co;
	ClipperLib::Paths result;
	co.AddPath(clipperPath, transform(jt), transform(et));
	co.Execute(result, ClipperLib::convert(offset));
	
	getPathListIntern().erase(cFirstPosEntryIterator(), cend());
	clipperPath.clear();
	resetStatistics();
	
	size_t points = 0;
	
	for (auto it=result.begin(); it != result.end(); ++it) {
		
		ClipperLib::Path& path = *it;
		closePath(path);
		ensureDirection(finalDirection, path);
		
		points += path.size();
		
		for (auto itP=path.begin(); itP != path.end(); ++itP) {
			addEntryAbs(ClipperLib::asCncDoublePosition(*itP), false);
		}
	}
	
	if ( points == 0 ) {
		std::cerr << CNC_LOG_FUNCT_A("Too much (inner) offset for the underlying path, therefore the resulted path is empty! Line: %ld\n", CLIENT_ID.lineNumber(firstClientID()));
	}
	
	const bool beToterant = true;
	return beToterant ? true : points != 0;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::processXYPocket(double toolDiameter) {
//////////////////////////////////////////////////////////////////
	const double	firstOffset				= - ( abs(toolDiameter) / 2.0 );		// CncCT_Inner (shrinking)-> value has to negative!
	const double	nextOffset				= - ( abs(toolDiameter) / 2.0 * 0.3 );	// CncCT_Inner (shrinking)-> value has to negative!
	const double 	maxOffset				= - 0.01;
	
	const unsigned	maxRecursionDeep		= 128;
	unsigned int	callCounter				= 0;
	
	// -----------------------------------------------------------
	// perform some checks
	if ( cnc::dblCompareNull(toolDiameter) )  {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" Tool diameter=%.3lf not sufficient. The pocket hole creation is stopped here!\n", abs(toolDiameter)));
		return false;
	}
	
	if ( fabs(toolDiameter) > 30.0 )  {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" Tool diameter=%.3lf to great. The pocket hole creation is stopped here!\n", abs(toolDiameter)));
		return false;
	}
	
	if ( firstOffset > maxOffset ) {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" Invalid first offset=%.3lf. The pocket hole creation is stopped here!\n", firstOffset));
		return false;
	}
	
	if ( nextOffset > maxOffset ) {
		std::cerr << CNC_LOG_FUNCT_A(wxString::Format(" Invalid next offset=%.3lf. The pocket hole creation is stopped here!\n", nextOffset));
		return false;
	}
	
	if ( isPathClosed() == false ) {
		std::cerr << CNC_LOG_FUNCT_A(" Path isn't closed. The pocket hole creation is stopped here!\n");
		return false;
	}
	
	const CncDirection finalDirection = CncCounterClockwise;
	
	// -----------------------------------------------------------
	auto isOffsetPossible = [] (ClipperLib::Path& path, double offset) {
		
		ClipperLib::ClipperOffset co;
		ClipperLib::Paths result;
		co.AddPath(path, ClipperLib::jtRound, ClipperLib::etClosedPolygon);
		co.Execute(result, ClipperLib::convert(offset));
		
		return result.size() > 0;
	};
	
	// -----------------------------------------------------------
	// add the given path information
	auto addPath = [&](ClipperLib::Path& path, bool first) {
		
		ClipperLib::closePath(path);
		ensureDirection(finalDirection, path);
		
		for (auto it = path.begin(); it != path.end(); ++it)
			addEntryAbs(ClipperLib::asCncDoublePosition(*it), false);
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
			std::cerr << CNC_LOG_FUNCT_A(" Max recursion deep reached. The pocket hole creation is stopped here!\n");
			return;
		}
		
		// in this condition the pocket hole layer it's already cleared 
		// nothing more has to be done.
		// remember: nextOffset is always less as firstOffset
		if ( isOffsetPossible(path, firstOffset) == false ) {
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
		std::cerr << CNC_LOG_FUNCT_A(" The first step don't deliver a result. The pocket hole creation is stopped here!\n");
		return false;
	}
	
	// clear the curent path position information
	getPathListIntern().erase(cFirstPosEntryIterator(), cend());
	clipperPath.clear();
	resetStatistics();
	
	// note: the result can be splitted into more the one path
	for (auto it=result.begin(); it != result.end(); ++it) {
		ClipperLib::Path& path = *it;
		
		if ( isOffsetPossible(path, firstOffset) == false ) {
			
			if ( ClipperLib::isHelix(path, toolDiameter) )
				executionRecommend = ER_HELIX;
		}
		
		// start the recursion . . .
		nextRound(path, true);
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::normalizeStartPosDistance(const CncDoublePosition& pos) {
//////////////////////////////////////////////////////////////////
	auto it = firstPosEntryIterator();
	if ( it == end() )
		return false;

	notNormalizeStartDistance = it->entryDistance;
	it->entryDistance.set(pos);
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::normalizeStartPos(const CncDoublePosition& pos) {
//////////////////////////////////////////////////////////////////
	auto it = firstPosEntryIterator();
	if ( it == end() )
		return false;
	
	const double transX	= pos.getX();
	const double transY	= pos.getY();
	const double dist	= sqrt(transX * transX + transY * transY);
	
	#warning sign or not sign????
	it->entryTarget.setXYZ(	pos.getX(),
							pos.getY(),
							it->entryTarget.getZ() - pos.getZ()
	);
	
	it->totalDistance += dist;
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::normalizeEndPos(const CncDoublePosition& pos) {
//////////////////////////////////////////////////////////////////
	auto it = lastPosEntryIterator();
	if ( it == rend() )
		return false;
		
	const long ciSuffix = it->clientId - (( it->clientId / 10 ) * 10);
	
	if ( ciSuffix == 9/*CO::FINALIZE_TEMPLATE*/ )
	{
		const double transX	= pos.getX();
		const double transY	= pos.getY();
		const double dist	= sqrt(transX * transX + transY * transY);
		
		#warning sign or not sign????
		it->entryTarget.setXYZ(	pos.getX(),
								pos.getY(),
								it->entryTarget.getZ() - pos.getZ()
		);
		
		it->totalDistance += dist;
		
		return true;
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
std::ostream& CncPathListManager::outputOperator(std::ostream &ostr) const {
//////////////////////////////////////////////////////////////////
	const CncDoublePosition& sp = getStartPos();
	
	ostr 	<< "CncPathListInfo entries : " << getPathList().size()											<< std::endl
			<< " Path Type              : " << getPathTypeAsStr()											<< std::endl
			<< " Is closed              : " << isPathClosed()												<< std::endl
			<< " Total Distance         : " << cnc::dblFormat1(getTotalDistance()) 							<< std::endl
			<< " Min Pos (x, y, z)      : " << cnc::dblFormat3(getMinPosX(), getMinPosY(), getMinPosZ())	<< std::endl
			<< " Max Pos (x, y, z)      : " << cnc::dblFormat3(getMaxPosX(), getMaxPosY(), getMaxPosZ())	<< std::endl
			<< " Start Pos              : " << cnc::dblFormat3(sp.getX(),    sp.getY(),    sp.getZ())		<< std::endl
			<< " Entries                : " << std::endl
			;
	
	for ( auto it = getPathList().begin(); it != getPathList().end(); ++it )
		it->traceEntry(ostr);
	
	return ostr;
}