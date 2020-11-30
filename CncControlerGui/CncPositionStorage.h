#ifndef CNC_POS_STORAGE_H
#define CNC_POS_STORAGE_H

#include "ArduinoEnvWrapper.h"
#include "CncPosition.h"

class CncPositionStorageView;

struct PositionStorage {
	
	static const uint8_t ReservedID					= 0;
	static const uint8_t TRIGGER_CTL_POS			= POS_STORE_SID_CTL_POS;
	static const uint8_t TRIGGER_APP_POS			= POS_STORE_SID_APP_POS;
	static const uint8_t TRIGGER_DRM_POS			= POS_STORE_SID_DIRECT_MOVE;
	static const uint8_t TRIGGER_RAM_POS			= POS_STORE_SID_RENDER_AND_MOVE;
	static const uint8_t TRIGGER_STP_POS			= POS_STORE_SID_STEPPING;
	static const uint8_t TRIGGER_PH_CB_POS			= POS_STORE_SID_PATH_HANDLER_OUTPUT;
	static const uint8_t TRIGGER_PH_LST_RUN			= POS_STORE_SID_PATH_LIST_RUNNER;
	static const uint8_t TRIGGER_MOV_SEQ_ADD		= POS_STORE_SID_MOV_SEQ_ADD;
	static const uint8_t TRIGGER_SERIAL_NULL		= POS_STORE_SID_SERIAL_NULL;
	
	static const uint8_t MAX_SIDS					= 16;
	
	static CncPositionStorageView*	storage;
	static bool posStorageIdState[MAX_SIDS];
	static bool isPosStorageEnabledFor(uint8_t id)		{ return ( id > 0 &&  id < MAX_SIDS - 1 ? posStorageIdState[id] : false ); }
	static void enablePosStorageFor(uint8_t id, bool s)	{ if ( id > 0 &&  id < MAX_SIDS - 1 ) { posStorageIdState[id] = s; } }
	
	static void init	(uint8_t sid, bool state);
	static void clear	(uint8_t sid);
	static void addPos	(uint8_t sid, const CncLongPosition& p);
	static void addPos	(uint8_t sid, const CncDoublePosition& p);
	static void addPos	(uint8_t sid, int32_t px, int32_t py, int32_t pz);
	static void addPos	(uint8_t sid, double px, double py, double pz);
	static void addMove	(uint8_t sid, int32_t dx, int32_t dy, int32_t dz);
	static void addMove	(uint8_t sid, double dx, double dy, double dz);
};

#endif