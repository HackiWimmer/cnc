#include "CncConfig.h"
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
 
//////////////////////////////////////////////////////////////
CncPathListManager::CncPathListManager()
: pathList			()
, guideList			()
, pathType			(PT_CNC_PATH)
, guideType 		(NO_GUIDE_PATH)
, defaultEntry		()
, lastCncEntry		(NULL)
, isFirstPath		(false)
, isCorrected		(false)
, referencePos		()
, minPosX			(0.0)
, minPosY			(0.0)
, minPosZ			(0.0)
, maxPosX			(0.0)
, maxPosY			(0.0)
, maxPosZ			(0.0)
, totalDistance		(0.0)
//////////////////////////////////////////////////////////////////
{
	// reservate memory
	pathList .reserve(THE_CONFIG->getConstReserveCapacity());
	guideList.reserve(THE_CONFIG->getConstReserveCapacity());
	
	// create initial entry
	initNextPath();
}
//////////////////////////////////////////////////////////////////
CncPathListManager::~CncPathListManager() {
//////////////////////////////////////////////////////////////////
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
	
	const bool b = pathType == PT_CNC_PATH && lastCncEntry != NULL;

	CncPathListEntry cpe;
	cpe.type				= CncPathListEntry::CHG_NOTHING;
	cpe.entryDistance		= CncPathListEntry::NoDistance;
	cpe.pathListReference	= CncTimeFunctions::getNanoTimestamp();
	
	cpe.clientId			= b ? lastCncEntry->clientId         : CncPathListEntry::DefaultClientID;
	cpe.feedSpeedMode		= b ? lastCncEntry->feedSpeedMode    : CncPathListEntry::DefaultSpeedMode;
	cpe.feedSpeed_MM_MIN	= b ? lastCncEntry->feedSpeed_MM_MIN : CncPathListEntry::DefaultSpeedValue;
	cpe.entryTarget			= b ? lastCncEntry->entryTarget      : CncPathListEntry::ZeroTarget;
	
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
		case PT_CNC_PATH: 		pathList.push_back(cpe);
								lastCncEntry = &pathList.back();
								break;
								
		case PT_GUIDE_PATH: 	guideList.push_back(cpe); 
								break;
	}
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryAdm(long clientId) {
//////////////////////////////////////////////////////////////////
	ASSERT_CHECK_PLML_SIZE_NOT_NULL // will return the defaultEntry on failure

	CncPathListEntry cpe;
	CncPathListEntry& prevEntry	= getPathListIntern().back();

	cpe.type					= CncPathListEntry::CHG_CLIENTID;
	cpe.entryDistance			= CncPathListEntry::NoDistance;
	cpe.clientId				= clientId;
	
	cpe.pathListReference		= prevEntry.pathListReference;
	cpe.alreadyRendered			= prevEntry.alreadyRendered;
	cpe.entryTarget				= prevEntry.entryTarget;
	cpe.feedSpeedMode			= prevEntry.feedSpeedMode;
	cpe.feedSpeed_MM_MIN		= prevEntry.feedSpeed_MM_MIN;

	// append
	//appendEntry(cpe);
	return appendEntry(cpe), getPathList().back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryAdm(CncSpeedMode mode, double feedSpeed_MM_MIN) {
//////////////////////////////////////////////////////////////////
	ASSERT_CHECK_PLML_SIZE_NOT_NULL // will return the defaultEntry on failure

	CncPathListEntry cpe;
	CncPathListEntry& prevEntry	= getPathListIntern().back();

	cpe.type					= CncPathListEntry::CHG_SPEED;
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

	CncPathListEntry cpe;
	CncPathListEntry& prevEntry	= getPathListIntern().back();

	cpe.type					= CncPathListEntry::CHG_POSITION;
	cpe.alreadyRendered			= alreadyRendered;
	cpe.entryTarget				= { newAbsPosX, newAbsPosY, newAbsPosZ };
	
	cpe.pathListReference		= prevEntry.pathListReference;
	cpe.clientId				= prevEntry.clientId;
	cpe.feedSpeedMode			= prevEntry.feedSpeedMode;
	cpe.feedSpeed_MM_MIN		= prevEntry.feedSpeed_MM_MIN;
	
	cpe.entryDistance.setX(newAbsPosX - prevEntry.entryTarget.getX());
	cpe.entryDistance.setY(newAbsPosY - prevEntry.entryTarget.getY());
	cpe.entryDistance.setZ(newAbsPosZ - prevEntry.entryTarget.getZ());
	
	// append
	return appendEntry(cpe), getPathList().back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryRel(double deltaX, double deltaY, double deltaZ, bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	ASSERT_CHECK_PLML_SIZE_NOT_NULL // will return the defaultEntry on failure

	CncPathListEntry cpe;
	CncPathListEntry& prevEntry	= getPathListIntern().back();
	
	cpe.type					= CncPathListEntry::CHG_POSITION;
	cpe.alreadyRendered			= alreadyRendered;
	cpe.entryDistance			= { deltaX, deltaY, deltaZ };
	
	cpe.pathListReference		= prevEntry.pathListReference;
	cpe.clientId				= prevEntry.clientId;
	cpe.feedSpeedMode			= prevEntry.feedSpeedMode;
	cpe.feedSpeed_MM_MIN		= prevEntry.feedSpeed_MM_MIN;

	cpe.entryTarget.setX(prevEntry.entryTarget.getX() + deltaX);
	cpe.entryTarget.setY(prevEntry.entryTarget.getY() + deltaY);
	cpe.entryTarget.setZ(prevEntry.entryTarget.getZ() + deltaZ);
	
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
			<< " Is corrected           : " << isPathCorrected()											<< std::endl
			<< " Is first Path          : " << getFirstPathFlag()											<< std::endl
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
	resetMinMax();
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::resetMinMax() {
//////////////////////////////////////////////////////////////////
	minPosX = minPosY = minPosZ = DBL_MAX;
	maxPosX = maxPosY = maxPosZ = DBL_MIN;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition& CncPathListManager::getStartPos() const {
//////////////////////////////////////////////////////////////////
	static CncDoublePosition p(0.0, 0.0, 0.0);
	
	if ( getPathList().size() > 0 )
		return const_begin()->entryTarget;
	
	return p; 
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::isPathClosed() const {
//////////////////////////////////////////////////////////////////
	if ( getPathListSize() > 0 ) {
		
		const CncPathList::const_iterator itFirst = const_begin();
		const CncPathList::const_iterator itLast  = const_end() - 1;
		const bool xc = cnc::dblCompare((*itFirst).entryTarget.getX(), (*itLast).entryTarget.getX());
		const bool yc = cnc::dblCompare((*itFirst).entryTarget.getY(), (*itLast).entryTarget.getY());
		const bool zc = cnc::dblCompare((*itFirst).entryTarget.getZ(), (*itLast).entryTarget.getZ());
		
		return ( xc && yc && zc ); 
	}
	
	return true;
}






//////////////////////////////////////////////////////////////////
bool CncPathListManager::eraseEntryAndRecalcuate(const CncPathList::iterator& itToErase) {
//////////////////////////////////////////////////////////////////
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
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::reversePath() {
//////////////////////////////////////////////////////////////////
	// empty or single point, nothing should happen
	// the size check is also very importent for the further implementation
	if ( getPathListSize() < 2 )
		return true;

	const CncSpeedMode oldStartMode = begin()->feedSpeedMode;
	const double oldStartSpeed 		= begin()->feedSpeed_MM_MIN;
	const bool oldStartRenderFlag   = begin()->alreadyRendered;

	const CncSpeedMode oldStopMode  = last()->feedSpeedMode;
	const double oldStopSpeed  		= last()->feedSpeed_MM_MIN;
	const bool oldStopRenderFlag    = last()->alreadyRendered;

	// reverse pathList
	std::reverse(begin(), end());

	// correct new first entry
	begin()->entryDistance		= begin()->entryTarget - referencePos;
	begin()->alreadyRendered 	= oldStopRenderFlag;
	begin()->feedSpeed_MM_MIN	= oldStopSpeed;
	begin()->feedSpeedMode		= oldStopMode;

	// correct last entry;
	last()->alreadyRendered 	= oldStartRenderFlag;
	last()->feedSpeed_MM_MIN	= oldStartSpeed;
	last()->feedSpeedMode		= oldStartMode;

	// initNextCncPath length - will be recalculate in next loop
	totalDistance = 0.0;

	// over all entries
	for (auto it = getPathListIntern().begin(); it != getPathListIntern().end(); ++it) {
		if ( std::distance(getPathListIntern().begin(), it) > 0 ) {
			// reverse relative move steps
			(*it).entryDistance = it->entryTarget - (it - 1)->entryTarget;

			// recalculate distance
			totalDistance += sqrt(  pow(it->entryDistance.getX(), 2)
					              + pow(it->entryDistance.getY(), 2)
								  + pow(it->entryDistance.getZ(), 2)
						     );

			it->totalDistance = totalDistance;

		}
		else {

			it->totalDistance = 0.0;
		}
	}

	return true;
}


