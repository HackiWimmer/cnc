#include "CncPositionStorageView.h"
#include "CncPositionStorage.h"

////////////////////////////////////////////////////////////////////////
CncPositionStorageView* PositionStorage::storage	= NULL;
// This initalized all array items with false (default value)
bool PositionStorage::posStorageIdState[MAX_SIDS] 	= {};

////////////////////////////////////////////////////////////////////////
void PositionStorage::init(uint8_t sid, bool state) {
////////////////////////////////////////////////////////////////////////
	if ( storage == NULL )
		return;
	
	if ( storage->init(sid, state) == false )
		;
}
////////////////////////////////////////////////////////////////////////
void PositionStorage::clear(uint8_t sid) {
////////////////////////////////////////////////////////////////////////
	if ( storage == NULL )
		return;
	
	if ( storage->clear(sid) == false )
		;
}
////////////////////////////////////////////////////////////////////////
void PositionStorage::addPos(uint8_t sid, const CncLongPosition& p) {
////////////////////////////////////////////////////////////////////////
	if ( PositionStorage::isPosStorageEnabledFor(sid) == false )
		return;
	
	if ( storage == NULL )
		return;
	
	if ( storage->addPos(sid, p) == false )
		;
}
////////////////////////////////////////////////////////////////////////
void PositionStorage::addPos(uint8_t sid, const CncDoublePosition& p) {
////////////////////////////////////////////////////////////////////////
	if ( PositionStorage::isPosStorageEnabledFor(sid) == false )
		return;
		
	if ( storage == NULL )
		return;
	
	if ( storage->addPos(sid, p) == false )
		;
}
////////////////////////////////////////////////////////////////////////
void PositionStorage::addPos(uint8_t sid, int32_t px, int32_t py, int32_t pz) {
////////////////////////////////////////////////////////////////////////
	if ( PositionStorage::isPosStorageEnabledFor(sid) == false )
		return;
		
	if ( storage == NULL )
		return;
	
	if ( storage->addPos(sid, px, py, pz) == false )
		;
}
////////////////////////////////////////////////////////////////////////
void PositionStorage::addPos(uint8_t sid, double px, double py, double pz) {
////////////////////////////////////////////////////////////////////////
	if ( PositionStorage::isPosStorageEnabledFor(sid) == false )
		return;
		
	if ( storage == NULL )
		return;
	
	if ( storage->addPos(sid, px, py, pz) == false )
		;
}
////////////////////////////////////////////////////////////////////////
void PositionStorage::addMove(uint8_t sid, int32_t dx, int32_t dy, int32_t dz) {
////////////////////////////////////////////////////////////////////////
	if ( PositionStorage::isPosStorageEnabledFor(sid) == false )
		return;
		
	if ( storage == NULL )
		return;
	
	if ( storage->addMove(sid, dx, dy, dz) == false )
		;
}
////////////////////////////////////////////////////////////////////////
void PositionStorage::addMove(uint8_t sid, double dx, double dy, double dz) {
////////////////////////////////////////////////////////////////////////
	if ( PositionStorage::isPosStorageEnabledFor(sid) == false )
		return;
		
	if ( storage == NULL )
		return;
	
	if ( storage->addMove(sid, dx, dy, dz) == false )
		;
}
