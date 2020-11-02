#include "CncConfig.h"
#include "CncPathListManager.h"

//////////////////////////////////////////////////////////////
CncPathListManager::CncPathListManager()
: list				()
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
	//preallocate memory
	list.reserve(THE_CONFIG->getConstReserveCapacity());
	reset();
}
//////////////////////////////////////////////////////////////////
CncPathListManager::~CncPathListManager() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
std::ostream& CncPathListManager::outputOperator(std::ostream &ostr) const {
//////////////////////////////////////////////////////////////////
	const CncDoublePosition& rp = getReferencePos();
	const CncDoublePosition& sp = getStartPos();
	
	ostr << "CncPathListInfo entries : " << list.size() 												<< std::endl;
	ostr << " Is corrected           : " << isPathCorrected() 											<< std::endl;
	ostr << " Is first Path          : " << getFirstPathFlag() 											<< std::endl;
	ostr << " Total Distance         : " << cnc::dblFormat1(getTotalDistance()) 						<< std::endl;
	ostr << " Min Pos (x, y, z)      : " << cnc::dblFormat3(getMinPosX(), getMinPosY(), getMinPosZ())	<< std::endl;
	ostr << " Max Pos (x, y, z)      : " << cnc::dblFormat3(getMaxPosX(), getMaxPosY(), getMaxPosZ())	<< std::endl;
	ostr << " Reference Pos          : " << cnc::dblFormat3(rp.getX(),    rp.getY(),    rp.getZ())		<< std::endl;
	ostr << " Start Pos              : " << cnc::dblFormat3(sp.getX(),    sp.getY(),    sp.getZ())		<< std::endl;
	ostr << " Entries                : " << std::endl;
	
	for ( auto it=getPathList().begin(); it!=getPathList().end(); ++it )
		it->traceEntry(ostr);
	
	return ostr;
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::clear() {
//////////////////////////////////////////////////////////////////
	resetMinMax();
	list.clear();
	
	reset();
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::reset() {
//////////////////////////////////////////////////////////////////
	if ( false ) {
		std::cout << "CncPathListManager::reset() size = " << list.size() << std::endl;
		for ( auto it = list.begin(); it != list.end(); ++it)
			std::cout << (*it);
	}

	isFirstPath   						= false;
	isCorrected 						= false;
	referencePos						= CncPathListEntry::ZeroTarget;
	totalDistance						= 0.0;
		
	const bool initialize	= list.size() > 0;
	CncPathListEntry cpe;
	cpe.type				= CncPathListEntry::CHG_NOTHING;
	cpe.pathListReference	= CncTimeFunctions::getNanoTimestamp();
	cpe.entryDistance		= CncPathListEntry::NoDistance;
	
	cpe.clientId			= initialize == true ? last()->clientId         : CncPathListEntry::DefaultClientID;
	cpe.feedSpeedMode		= initialize == true ? last()->feedSpeedMode    : CncPathListEntry::DefaultSpeedMode;
	cpe.feedSpeed_MM_MIN	= initialize == true ? last()->feedSpeed_MM_MIN : CncPathListEntry::DefaultSpeedValue;
	cpe.entryTarget			= initialize == true ? last()->entryTarget      : CncPathListEntry::ZeroTarget;
	
	resetMinMax();
	list.clear();

	appendEntry(cpe);
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::resetMinMax() {
//////////////////////////////////////////////////////////////////
	minPosX			= DBL_MAX;
	minPosY			= DBL_MAX;
	minPosZ			= DBL_MAX;

	maxPosX			= DBL_MIN;
	maxPosY			= DBL_MIN;
	maxPosZ			= DBL_MIN;
}
//////////////////////////////////////////////////////////////////
const CncDoublePosition& CncPathListManager::getStartPos() const {
//////////////////////////////////////////////////////////////////
	static CncDoublePosition p(0.0, 0.0, 0.0);
	if ( list.size() > 0 )
		return const_begin()->entryTarget;
	
	return p; 
}
//////////////////////////////////////////////////////////////////
bool CncPathListManager::isPathClosed() const {
//////////////////////////////////////////////////////////////////
	if ( getPathListSize() > 0 ) {
		const CncPathList::const_iterator itFirst = const_begin();
		const CncPathList::const_iterator itLast  = const_end() - 1;
		
		return ( cnc::dblCompare((*itFirst).entryTarget.getX(), (*itLast).entryTarget.getX()) &&
				 cnc::dblCompare((*itFirst).entryTarget.getY(), (*itLast).entryTarget.getY()) &&
				 cnc::dblCompare((*itFirst).entryTarget.getZ(), (*itLast).entryTarget.getZ())
			   ); 
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
void CncPathListManager::appendEntry(CncPathListEntry& cpe) {
//////////////////////////////////////////////////////////////////
	// additionally calculate length and distance
	if ( list.size() > 0 ) {
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
	
	// store
	cpe.listIndex = (long)list.size();
	list.push_back(cpe);
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryAdm(long clientId) {
//////////////////////////////////////////////////////////////////
	CncPathListEntry cpe;

	cpe.type					= CncPathListEntry::CHG_CLIENTID;
	cpe.clientId				= clientId;
	cpe.entryDistance			= CncPathListEntry::NoDistance;

	// calculate
	if ( list.size() == 0 ) {
		cpe.pathListReference	= CncPathListEntry::NoReference;

		cpe.alreadyRendered		= CncPathListEntry::DefaultAlreadyRendered;
		cpe.entryTarget			= CncPathListEntry::ZeroTarget;

		cpe.feedSpeedMode		= CncPathListEntry::DefaultSpeedMode;
		cpe.feedSpeed_MM_MIN	= CncPathListEntry::DefaultSpeedValue;
	}
	else {
		cpe.pathListReference	= list.back().pathListReference;

		cpe.alreadyRendered		= list.back().alreadyRendered;
		cpe.entryTarget			= list.back().entryTarget;

		cpe.feedSpeedMode		= list.back().feedSpeedMode;
		cpe.feedSpeed_MM_MIN	= list.back().feedSpeed_MM_MIN;
	}

	// append
	appendEntry(cpe);
	return list.back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryAdm(CncSpeedMode mode, double feedSpeed_MM_MIN) {
//////////////////////////////////////////////////////////////////
	CncPathListEntry cpe;

	cpe.type					= CncPathListEntry::CHG_SPEED;
	cpe.feedSpeedMode			= mode;
	cpe.feedSpeed_MM_MIN		= feedSpeed_MM_MIN;
	cpe.entryDistance			= CncPathListEntry::NoDistance;

	// calculate
	if ( list.size() == 0 ) {
		cpe.pathListReference	= CncPathListEntry::NoReference;
		cpe.clientId			= CncPathListEntry::DefaultClientID;

		cpe.alreadyRendered		= CncPathListEntry::DefaultAlreadyRendered;
		cpe.entryTarget			= CncPathListEntry::ZeroTarget;
	}
	else {
		cpe.pathListReference	= list.back().pathListReference;
		cpe.clientId			= list.back().clientId;

		cpe.alreadyRendered		= list.back().alreadyRendered;
		cpe.entryTarget			= list.back().entryTarget;
	}
	
	// append
	appendEntry(cpe);
	return list.back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryAbs(double newAbsPosX, double newAbsPosY, double newAbsPosZ, bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	CncPathListEntry cpe;

	cpe.type					= CncPathListEntry::CHG_POSITION;
	cpe.alreadyRendered			= alreadyRendered;
	cpe.entryTarget				= {newAbsPosX, newAbsPosY, newAbsPosZ};

	// calculate
	if ( list.size() == 0 ) {
		cpe.pathListReference	= CncPathListEntry::NoReference;
		cpe.clientId			= CncPathListEntry::DefaultClientID;
		
		cpe.feedSpeedMode		= CncPathListEntry::DefaultSpeedMode;
		cpe.feedSpeed_MM_MIN	= CncPathListEntry::DefaultSpeedValue;

		cpe.entryDistance		= {newAbsPosX, newAbsPosY, newAbsPosZ};
	}
	else {
		cpe.pathListReference	= list.back().pathListReference;
		cpe.clientId			= list.back().clientId;

		cpe.feedSpeedMode		= list.back().feedSpeedMode;
		cpe.feedSpeed_MM_MIN	= list.back().feedSpeed_MM_MIN;

		cpe.entryDistance.setX(newAbsPosX - list.back().entryTarget.getX());
		cpe.entryDistance.setY(newAbsPosY - list.back().entryTarget.getY());
		cpe.entryDistance.setZ(newAbsPosZ - list.back().entryTarget.getZ());
	}
	
	// append
	appendEntry(cpe);
	return list.back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryRel(double newRelPosX, double newRelPosY, double newRelPosZ, bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	CncPathListEntry cpe;
	
	cpe.type					= CncPathListEntry::CHG_POSITION;
	cpe.alreadyRendered			= alreadyRendered;
	cpe.entryDistance			= {newRelPosX, newRelPosY, newRelPosZ};

	// calculate
	if ( list.size() == 0 ) {
		cpe.pathListReference	= CncPathListEntry::NoReference;
		cpe.clientId			= CncPathListEntry::DefaultClientID;
		
		cpe.feedSpeedMode		= CncPathListEntry::DefaultSpeedMode;
		cpe.feedSpeed_MM_MIN	= CncPathListEntry::DefaultSpeedValue;

		cpe.entryTarget			= {newRelPosX, newRelPosY, newRelPosZ};
		}
	else {
		cpe.pathListReference	= list.back().pathListReference;
		cpe.clientId			= list.back().clientId;

		cpe.feedSpeedMode		= list.back().feedSpeedMode;
		cpe.feedSpeed_MM_MIN	= list.back().feedSpeed_MM_MIN;

		cpe.entryTarget.setX(list.back().entryTarget.getX() + newRelPosX);
		cpe.entryTarget.setY(list.back().entryTarget.getY() + newRelPosY);
		cpe.entryTarget.setZ(list.back().entryTarget.getZ() + newRelPosZ);
	}
	
	// append
	appendEntry(cpe);
	return list.back();
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryAbs(const CncDoublePosition& newAbsPos, bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	return addEntryAbs(newAbsPos.getX(), newAbsPos.getY(),	newAbsPos.getZ(), alreadyRendered);
}
//////////////////////////////////////////////////////////////////
const CncPathListEntry& CncPathListManager::addEntryRel(const CncDoublePosition& newRelPos, bool alreadyRendered) {
//////////////////////////////////////////////////////////////////
	return addEntryRel(newRelPos.getX(), newRelPos.getY(),	newRelPos.getZ(), alreadyRendered);
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
	if ( list.erase(itToErase) == end() )
		return false;
		
	// nothing further to do if the list is now empty
	if ( list.size() == 0 )
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

	// reverse list
	std::reverse(list.begin(), list.end());

	// correct new first entry
	begin()->entryDistance		= begin()->entryTarget - referencePos;
	begin()->alreadyRendered 	= oldStopRenderFlag;
	begin()->feedSpeed_MM_MIN	= oldStopSpeed;
	begin()->feedSpeedMode		= oldStopMode;

	// correct last entry;
	last()->alreadyRendered 	= oldStartRenderFlag;
	last()->feedSpeed_MM_MIN	= oldStartSpeed;
	last()->feedSpeedMode		= oldStartMode;

	// reset length - will be recalculate in next loop
	totalDistance = 0.0;

	// over all entries
	for (CncPathList::iterator it = list.begin(); it != list.end(); ++it) {
		if ( std::distance(list.begin(), it) > 0 ) {
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
