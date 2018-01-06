#include "CncArduino.h"
#include "MainFrame.h"
#include "SerialSimulatorDevNull.h"

/*
 *	Don't use std::cout|clog &Co directly in this thtead.
 *	Due to the fact that this will deadlock this thread,
 *  because the main thread sleeps in condition of
 * 	sleepMilliseconds(...) and the redirected streams cant
 * 	be well solved.
 *
 *	Use instead:
 * 		wxThreadEvent evt(wxEVT_TRACE_FROM_THREAD, MainFrame::EventId::POST_INFO|WARNING|ERROR);
 *		evt.SetString("<... message to log ...>");
 * 		wxPostEvent(GBL_CONFIG->getTheApp(), evt);
 * 
 *	Or one of the following functions:
 *		void publishLogStreamAsInfoMsg();
 *		void publishLogStreamAsWarningMsg();
 *		void publishLogStreamAsErrorMsg();
 *		void appendLogStreamToErrorInfo(unsigned char eid = E_PURE_TEXT_VALUE_ERROR);
*/

///////////////////////////////////////////////////////////////////
SerialSimulatorDevNull::SerialSimulatorDevNull(SerialSimulatorFacade* caller) 
: SerialSimulatorThread(caller)
, targetMajorPos(0L, 0L, 0L)
, curSimulatorPos(0L, 0L, 0L)
, curLimitStates(LIMIT_UNSET, LIMIT_UNSET, LIMIT_UNSET)
, speedSimulator(NULL)
, posReplyThresholdX(0)
, posReplyThresholdY(0)
, posReplyThresholdZ(0)
///////////////////////////////////////////////////////////////////
{
	memset(&renderPointA, 0, sizeof(renderPointA));
	memset(&renderPointB, 0, sizeof(renderPointB));
	
	// initial setup, will be initiated later with concret values
	speedSimulator = new CncSpeedSimulator(	defaultLoopDuration,
											0.0, 0, 0,
											0.0, 0, 0,
											0.0, 0, 0);
}
///////////////////////////////////////////////////////////////////
SerialSimulatorDevNull::~SerialSimulatorDevNull() {
///////////////////////////////////////////////////////////////////
	if ( speedSimulator != NULL )
		delete speedSimulator;
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorDevNull::resetSerial() {
///////////////////////////////////////////////////////////////////
	// to do something specialized things here
	curSimulatorPos.setXYZ(0L, 0L, 0L);
	curLimitStates.setXYZ(LIMIT_UNSET, LIMIT_UNSET, LIMIT_UNSET);
	
	posReplyThresholdX = 0;
	posReplyThresholdY = 0;
	posReplyThresholdZ = 0;
	
	speedSimulator->setup(	defaultLoopDuration,
							0.0, 0, 0,
							0.0, 0, 0,
							0.0, 0, 0);
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorDevNull::performCurrentPositions(unsigned char pid) {
///////////////////////////////////////////////////////////////////
	switch ( pid ) {
		case PID_X_POS: 			Serial_writeLongValues(pid, curSimulatorPos.getX());
									break;
									
		case PID_Y_POS: 			Serial_writeLongValues(pid, curSimulatorPos.getY());
									break;
									
		case PID_Z_POS: 			Serial_writeLongValues(pid, curSimulatorPos.getZ());
									break;
									
		case PID_XYZ_POS: 
		case PID_XYZ_POS_MAJOR: 
		case PID_XYZ_POS_DETAIL: 	Serial_writeLongValues(pid, curSimulatorPos.getX(), curSimulatorPos.getY(), curSimulatorPos.getZ());
									break;
									
		default:					; // do nothing
	}
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorDevNull::performLimitStates() {
///////////////////////////////////////////////////////////////////
	int32_t limitStateX = 0;
	int32_t limitStateY = 0;
	int32_t limitStateZ = 0;
	
	int32_t mx = 0, my = 0, mz = 0;
	getSetterValueAsLong(PID_MAX_DIMENSION_X, mx, 0);
	getSetterValueAsLong(PID_MAX_DIMENSION_Y, my, 0);
	getSetterValueAsLong(PID_MAX_DIMENSION_Z, mz, 0);
	
	if ( mx != 0 && my != 0 && mz != 0 ) {
		if ( curSimulatorPos.getX() <= -mx )	limitStateX = LIMIT_MIN;
		if ( curSimulatorPos.getX() >= +mx )	limitStateX = LIMIT_MAX;

		if ( curSimulatorPos.getY() <= -my )	limitStateY = LIMIT_MIN;
		if ( curSimulatorPos.getY() >= +my )	limitStateY = LIMIT_MAX;

		if ( curSimulatorPos.getZ() <= -mz )	limitStateZ = LIMIT_MIN;
		if ( curSimulatorPos.getZ() >= +mz )	limitStateZ = LIMIT_MAX;
	}
	
	curLimitStates.setX(limitStateX);
	curLimitStates.setY(limitStateY);
	curLimitStates.setZ(limitStateZ);
	Serial_writeGetterValues(PID_LIMIT, curLimitStates.getX(), curLimitStates.getY(), curLimitStates.getZ());
}
///////////////////////////////////////////////////////////////////
unsigned char SerialSimulatorDevNull::performGetterValue(unsigned char pid) {
///////////////////////////////////////////////////////////////////
	unsigned char ret = RET_OK;
	
	if ( false ) {
		wxThreadEvent evt(wxEVT_TRACE_FROM_THREAD, MainFrame::EventId::POST_INFO);
		evt.SetString(wxString::Format("PID: %u", pid));
		wxPostEvent(GBL_CONFIG->getTheApp(), evt);
 	}
	
	switch ( pid ) {
		
		case PID_X_POS:   					Serial_writeGetterValues(pid,  curSimulatorPos.getX()); break;
		case PID_Y_POS:   					Serial_writeGetterValues(pid,  curSimulatorPos.getY()); break;
		case PID_Z_POS:  					Serial_writeGetterValues(pid,  curSimulatorPos.getZ()); break;
		case PID_XY_POS:  					Serial_writeGetterValues(pid, curSimulatorPos.getX(), curSimulatorPos.getY()); break;
		case PID_XYZ_POS: 					Serial_writeGetterValues(pid, curSimulatorPos.getX(), curSimulatorPos.getY(), curSimulatorPos.getZ()); break;
		
		case PID_LIMIT:						performLimitStates(); break;
		
		case PID_GET_POS_COUNTER:			Serial_writeGetterValues(pid, getPositionCounter(), getPositionOverflowCounter()); break;
		
		case PID_GET_STEP_COUNTER_X:		Serial_writeGetterValues(pid, getStepCounterX(), getStepOverflowCounterX()); break;
		case PID_GET_STEP_COUNTER_Y:		Serial_writeGetterValues(pid, getStepCounterY(), getStepOverflowCounterY()); break;
		case PID_GET_STEP_COUNTER_Z:		Serial_writeGetterValues(pid, getStepCounterZ(), getStepOverflowCounterZ()); break;

		default:							ret = SerialSimulatorThread::performGetterValue(pid);
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
unsigned char SerialSimulatorDevNull::performSetterValue(unsigned char pid, int32_t value) {
///////////////////////////////////////////////////////////////////
	switch ( pid ) {
		case PID_PITCH_X:
		case PID_PITCH_Y:
		case PID_PITCH_Z:
		case PID_STEPS_X:
		case PID_STEPS_Y:
		case PID_STEPS_Z:
		case PID_PULSE_WIDTH_OFFSET_X:
		case PID_PULSE_WIDTH_OFFSET_Y:
		case PID_PULSE_WIDTH_OFFSET_Z:	speedSimulator->setup(	defaultLoopDuration,
																getSetterValueAsDouble(PID_PITCH_X, 0.0), getSetterValueAsLong(PID_STEPS_X, 0), 2 * getSetterValueAsLong(PID_PULSE_WIDTH_OFFSET_X, 0),
																getSetterValueAsDouble(PID_PITCH_Y, 0.0), getSetterValueAsLong(PID_STEPS_Y, 0), 2 * getSetterValueAsLong(PID_PULSE_WIDTH_OFFSET_Y, 0),
																getSetterValueAsDouble(PID_PITCH_Z, 0.0), getSetterValueAsLong(PID_STEPS_Z, 0), 2 * getSetterValueAsLong(PID_PULSE_WIDTH_OFFSET_Z, 0));
										break;
		
		
		case PID_SPEED_MM_MIN: 			speedSimulator->setFeedSpeed((double)(value/DBL_FACT));
										break;
										
		case PID_POS_REPLY_THRESHOLD_X:	posReplyThresholdX = value; 
										break;
										
		case PID_POS_REPLY_THRESHOLD_Y:	posReplyThresholdY = value; 
										break;
										
		case PID_POS_REPLY_THRESHOLD_Z:	posReplyThresholdZ = value; 
										break;
										
		case PID_RESERT_POS_COUNTER:	resetPositionCounter();
										break;
										
		case PID_RESERT_STEP_COUNTER:	resetStepCounter();
										break;
	}
	
	return RET_OK;
}
///////////////////////////////////////////////////////////////////
unsigned char SerialSimulatorDevNull::performMove() {
///////////////////////////////////////////////////////////////////
	unsigned ret = decodeMove();
	performCurrentPositions(PID_XYZ_POS_MAJOR);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
unsigned char SerialSimulatorDevNull::decodeMove() {
//////////////////////////////////////////////////////////////////
	unsigned char b[4];
	long v[3];
	unsigned int size  = 0;
	unsigned int count = 0;

	//fetch 1 to max 3 long values
	while ( Serial_available() > 0 ) {
		size = Serial_readBytes(b, 4);
		
		if ( size != 4 ) {
			addErrorInfo(E_INVALID_MOVE_CMD, wxString::Format("Received size: %u", size));
			return RET_ERROR;
		}
		
		v[count]  = (int32_t)b[0] << 24;
		v[count] += (int32_t)b[1] << 16;
		v[count] += (int32_t)b[2] << 8;
		v[count] += (int32_t)b[3];
		count++;
	}
	
	int32_t x = 0, y = 0, z = 0;
	switch ( count ) {
		case 1:		z = v[0]; break;
		case 2:		x = v[0]; y = v[1]; break;
		case 3:		x = v[0]; y = v[1]; z= v[2]; break;
		default:  	return RET_ERROR;
	}
	
	targetMajorPos.set(curSimulatorPos);
	targetMajorPos.inc(x, y, z);
	
	if ( renderAndStepAxisXYZ(x, y, z) == false )
		return RET_ERROR;
		
	return RET_OK;
}
///////////////////////////////////////////////////////////////////
bool SerialSimulatorDevNull::renderAndStepAxisXYZ(int32_t dx, int32_t dy, int32_t dz) {
///////////////////////////////////////////////////////////////////
	// update speed simulator values
	if ( isProbeMode() == false ) {
		wxASSERT( speedSimulator != NULL );
		speedSimulator->setNextMove(dx, dy, dz);
	}
	
	// initialize
	int i, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
	memset(&renderPointA, 0, sizeof(renderPointB));
	memset(&renderPointB, 0, sizeof(renderPointB));
	
	x_inc = (dx < 0) ? -1 : 1;
	l = abs(dx);
	
	y_inc = (dy < 0) ? -1 : 1;
	m = abs(dy);
	
	z_inc = (dz < 0) ? -1 : 1;
	n = abs(dz);
	
	dx2 = l << 1;
	dy2 = m << 1;
	dz2 = n << 1;

	// -------------------------------------------------------------
	if ((l >= m) && (l >= n)) {
		err_1 = dy2 - l;
		err_2 = dz2 - l;
		for (i = 0; i < l; i++) {
			
			if ( stepAxisXYZ(renderPointA[0] - renderPointB[0], renderPointA[1] - renderPointB[1], renderPointA[2] - renderPointB[2]) == false )
				return false;
			
			if (err_1 > 0) {
				renderPointA[1]	+= y_inc;
				err_1			-= dx2;
			}
			if (err_2 > 0) {
				renderPointA[2]	+= z_inc;
				err_2			-= dx2;
			}
			err_1			+= dy2;
			err_2			+= dz2;
			renderPointA[0]	+= x_inc;
		}
	
	// -------------------------------------------------------------
	} else if ((m >= l) && (m >= n)) {
		err_1 = dx2 - m;
		err_2 = dz2 - m;
		for (i = 0; i < m; i++) {
			
			if ( stepAxisXYZ(renderPointA[0] - renderPointB[0], renderPointA[1] - renderPointB[1], renderPointA[2] - renderPointB[2]) == false )
				return false;
			
			if (err_1 > 0) {
				renderPointA[0]	+= x_inc;
				err_1			-= dy2;
			}
			if (err_2 > 0) {
				renderPointA[2]	+= z_inc;
				err_2			-= dy2;
			}
			err_1			+= dx2;
			err_2			+= dz2;
			renderPointA[1]	+= y_inc;
		}
		
	// -------------------------------------------------------------
	} else {
		err_1 = dy2 - n;
		err_2 = dx2 - n;
		for (i = 0; i < n; i++) {
			
			if ( stepAxisXYZ(renderPointA[0] - renderPointB[0], renderPointA[1] - renderPointB[1], renderPointA[2] - renderPointB[2]) == false )
				return false;
			
			if (err_1 > 0) {
				renderPointA[1]	+= y_inc;
				err_1			-= dz2;
			}
			if (err_2 > 0) {
				renderPointA[0]	+= x_inc;
				err_2			-= dz2;
			}
			err_1     		+= dy2;
			err_2			+= dx2;
			renderPointA[2]	+= z_inc;
		}
	}
	
	// -------------------------------------------------------------
	if ( stepAxisXYZ(renderPointA[0] - renderPointB[0], renderPointA[1] - renderPointB[1], renderPointA[2] - renderPointB[2], true) == false )
		return false;
		
	// perform any rest offset
	if ( isProbeMode() == false ) {
		wxASSERT( speedSimulator != NULL );
		//realeaseCondition();
		speedSimulator->performCurrentOffsetThreadLocal(true);
		speedSimulator->reset();
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialSimulatorDevNull::stepAxisXYZ(int32_t dx , int32_t dy , int32_t dz, bool force) {
///////////////////////////////////////////////////////////////////
	if ( TestDestroy() )
		return false;
	
	// statistic
	incPosistionCounter();
	incStepCounterX(dx);
	incStepCounterY(dy);
	incStepCounterZ(dz);
		
	// position management
	curSimulatorPos.incX(dx);
	curSimulatorPos.incY(dy);
	curSimulatorPos.incZ(dz);

	// simulate speed
	if ( stepAxis('X', dx) == false ) return false;
	if ( stepAxis('Y', dy) == false ) return false;
	if ( stepAxis('Z', dz) == false ) return false;
	
	// release a controler callback
	static CncLongPosition lastReplyPos;
	CncLongPosition diff(curSimulatorPos - lastReplyPos);
	
	if ( absolute( diff.getX() ) >= posReplyThresholdX || 
	     absolute( diff.getY() ) >= posReplyThresholdY || 
		 absolute( diff.getZ() ) >= posReplyThresholdZ ||
		 force == true ) 
	{
		if ( curSimulatorPos != targetMajorPos ) {
			performCurrentPositions(PID_XYZ_POS_DETAIL);
			lastReplyPos.set(curSimulatorPos);
		}
	}
	
	// copy point A into point B
	memcpy(&renderPointB, &renderPointA, sizeof(renderPointA));
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialSimulatorDevNull::stepAxis(char axis, int32_t steps) {
///////////////////////////////////////////////////////////////////
	for ( int32_t i = 0; i < absolute(steps); i++ ) {
		// signal handling
		switch ( getLastSignal() ) {
		
			case SIG_INTERRUPPT:
			case SIG_HALT:				return false;
			
			case SIG_PAUSE:				// pause handling
										while ( getLastSignal() == SIG_PAUSE ) {
											wxThreadEvent evt(wxEVT_DISPATCH_ALL, MainFrame::EventId::DISPATCH_ALL);
											wxPostEvent(GBL_CONFIG->getTheApp(), evt);
											
											CncTimeFunctions::activeWaitMircoseconds(20000);
										}
										break;
			
			case SIG_RESUME:			resetLastSignal(); 
										break;
			
			default:					; // Do nothing
		}
		
		// simulate speed
		if ( isProbeMode() == false ) {
			wxASSERT( speedSimulator != NULL );
			int32_t val = absolute(steps);
			
			switch ( axis ) {
				case 'X':	if ( val > 0 ) speedSimulator->simulateSteppingX(val);
							break;
							
				case 'Y':	if ( val > 0 ) speedSimulator->simulateSteppingY(val);
							break;
							
				case 'Z':	if ( val > 0 ) speedSimulator->simulateSteppingZ(val);
							break;
							
				default:	wxASSERT(axis != 'X' && axis != 'Y' && axis != 'Z');
							return false;
			}
			
			speedSimulator->performCurrentOffsetThreadLocal(false);
		}
	}
	
	return true;
}