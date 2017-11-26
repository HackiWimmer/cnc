#include "CncControl.h"
#include "MainFrame.h"
#include "SerialEmulatorNULL.h"

int pointA[3], pointB[3];

///////////////////////////////////////////////////////////////////
SerialEmulatorNULL::SerialEmulatorNULL(CncControl* cnc)
: SerialSpyPort(cnc)
, posReplyThreshold(10)
, positionCounter(0)
, stepCounter(0)
, repeatCount(0)
, setterMap()
, curEmulatorPos(0L, 0L, 0L)
, lastSignal(CMD_INVALID)
, lastCommand()
///////////////////////////////////////////////////////////////////
{
	reset();
}
///////////////////////////////////////////////////////////////////
SerialEmulatorNULL::SerialEmulatorNULL(const char *portName)
: SerialSpyPort(portName) 
, posReplyThreshold(10)
, positionCounter(0)
, stepCounter(0)
, setterMap()
, curEmulatorPos(0L, 0L, 0L)
, lastSignal(CMD_INVALID)
, lastCommand()
///////////////////////////////////////////////////////////////////
{
	reset();
}
///////////////////////////////////////////////////////////////////
SerialEmulatorNULL::~SerialEmulatorNULL() {
///////////////////////////////////////////////////////////////////
	reset();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::reset() {
	lastCommand.restLastCmd();
	lastCommand.resetM();
	
	lastSignal = CMD_INVALID;
	
	posReplyThreshold = 10;
	
	curEmulatorPos.setXYZ(0L, 0L, 0L);
	
	setterMap.clear();
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::evaluatePositions(std::vector<int32_t>& ret) {
///////////////////////////////////////////////////////////////////
	ret.push_back(cncControl->getCurPos().getX() + lastCommand.Mc.lastMoveX);
	ret.push_back(cncControl->getCurPos().getY() + lastCommand.Mc.lastMoveY);
	ret.push_back(cncControl->getCurPos().getZ() + lastCommand.Mc.lastMoveZ);
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::evaluateLimitStates(std::vector<int32_t>& ret) {
///////////////////////////////////////////////////////////////////
	int limitStateX = LIMIT_UNSET;
	int limitStateY = LIMIT_UNSET;
	int limitStateZ = LIMIT_UNSET;
	
	CncConfig* cncConfig = CncConfig::getGlobalCncConfig();
	
	if ( (cncControl->getCurPos().getX() + lastCommand.Mc.lastMoveX)/cncConfig->getCalculationFactX() <= -cncConfig->getMaxDimensionX() )
		limitStateX = LIMIT_MIN;
		
	if ( (cncControl->getCurPos().getX() + lastCommand.Mc.lastMoveX)/cncConfig->getCalculationFactX() >= +cncConfig->getMaxDimensionX() )
		limitStateX = LIMIT_MAX;
	
	if ( (cncControl->getCurPos().getY() + lastCommand.Mc.lastMoveY)/cncConfig->getCalculationFactY() <= -cncConfig->getMaxDimensionY() )
		limitStateY = LIMIT_MIN;
		
	if ( (cncControl->getCurPos().getY() + lastCommand.Mc.lastMoveY)/cncConfig->getCalculationFactY() >= +cncConfig->getMaxDimensionY() )
		limitStateY = LIMIT_MAX;
		
	if ( (cncControl->getCurPos().getZ() + lastCommand.Mc.lastMoveZ)/cncConfig->getCalculationFactZ() <= -cncConfig->getMaxDimensionZ() )
		limitStateZ = LIMIT_MIN;
		
	if ( (cncControl->getCurPos().getZ() + lastCommand.Mc.lastMoveZ)/cncConfig->getCalculationFactZ() >= +cncConfig->getMaxDimensionZ() )
		limitStateZ = LIMIT_MAX;
		
	ret.push_back(limitStateX);
	ret.push_back(limitStateY);
	ret.push_back(limitStateZ);
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::processGetter(unsigned char pid, std::vector<int32_t>& ret) {
///////////////////////////////////////////////////////////////////
	switch ( pid ) {
		case PID_XYZ_POS:		return evaluatePositions(ret);
		case PID_LIMIT:			return evaluateLimitStates(ret);
		case PID_ERROR_COUNT:	ret.push_back(0); return true;
		default:				
								SetterMap::iterator it;
								it = setterMap.find((int)pid);
								
								if ( it != setterMap.end() ) {
									ret.push_back((*it).second);
								} else {
									std::cerr << "SerialEmulatorNULL::processGetter: Invalid getter pid: " << pid << std::endl;
								}
	}
	return false;
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::getCurrentMoveCmdPID() {
///////////////////////////////////////////////////////////////////
	if ( lastCommand.Mc.respondCounter > 0 ) {
		return PID_LIMIT;
	}
	return PID_XYZ_POS;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::getCurrentMoveCmdValues(int32_t &x, int32_t &y, int32_t &z) {
///////////////////////////////////////////////////////////////////
	if ( lastCommand.Mc.respondCounter > 0 ) {
		CncLimitStates ls;
		ls.setXYZLimit(cncControl->getControllerLimitState());
		
		x = ls.getXLimit();
		y = ls.getYLimit();
		z = ls.getZLimit();
		return;
	}
	
	x = cncControl->getCurPos().getX() + lastCommand.Mc.lastMoveX;
	y = cncControl->getCurPos().getY() + lastCommand.Mc.lastMoveY;
	z = cncControl->getCurPos().getZ() + lastCommand.Mc.lastMoveZ;
}
///////////////////////////////////////////////////////////////////
const char* SerialEmulatorNULL::getConfiguration(wxString& ret) {
///////////////////////////////////////////////////////////////////
	ret.clear();
	ret << wxString::Format("%d:%s\n", PID_COMMON, "Here only collected setter values, because there's no controller connection");
	
	SetterMap::iterator it;
	for ( it=setterMap.begin(); it!=setterMap.end(); ++it ) {
		if ( it->first >= PID_PITCH && it->first <= PID_PITCH_Z)	ret << wxString::Format(" %d:%.2lf\n", it->first, (double)(it->second/1000));
		else														ret << wxString::Format(" %d:%d\n",    it->first, it->second);
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::readData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	int ret = 0;
	
	wxString firmWare(wxString::Format("%s", FIRMWARE_VERSION));
	wxString retStr;
	
	switch( lastCommand.cmd ) { 
		case CMD_MOVE:
		case CMD_RENDER_AND_MOVE:		ret = readMove(buffer, nbByte);
										break;
		
		case CMD_PRINT_VERSION: 		ret = readDefault(buffer, nbByte, firmWare);
										break;
		
		case CMD_PRINT_ERRORINFO: 		ret = readDefault(buffer, nbByte, "1:0:Not available, because there's no controller connection\n");
										break;
		
		case CMD_PRINT_CONFIG: 			ret = readDefault(buffer, nbByte, getConfiguration(retStr));
										break;
		
		case CMD_PRINT_PIN_REPORT: 		ret = readDefault(buffer, nbByte, wxString::Format("%i:0:0:0\n", MAX_PINS)); // see DataControlModel for more details
										break;
									
		case CMD_TEST_INFO_MESSAGE:		ret = readMessage(buffer, nbByte, "INFO");
										break;
										
		case CMD_TEST_WARN_MESSAGE:		ret = readMessage(buffer, nbByte, "WARNING");
										break;
										
		case CMD_TEST_ERROR_MESSAGE:	ret = readMessage(buffer, nbByte, "ERROR");
										break;
		
		default:						((char*)buffer)[0] = RET_OK;
										ret = 1;
										lastCommand.restLastCmd();
	}
	
	spyReadData(ret, buffer, nbByte);
	return ret;
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::readMessage(void *buffer, unsigned int nbByte, const char* response) {
///////////////////////////////////////////////////////////////////
	if ( response == NULL )
		return -1;
		
	if ( buffer == NULL )
		return -1;
	
	// prepare the test message and start responding (publish RET_MSG)
	if ( lastCommand.index == 0 ) {
		((char*)buffer)[0] = RET_MSG;
		
		lastCommand.Msg.text.assign("This is a test message from type: ");
		char type = response[0];
		
		switch ( type ) {
			case 'W':	lastCommand.Msg.text.append(response);
						break;
						
			case 'E':	lastCommand.Msg.text.append(response);
						break;
						
			default:	type = 'I';
						lastCommand.Msg.text.append(response);
		}
		
		lastCommand.Msg.type = type;
		lastCommand.index++;
		return 1;
	}
	
	// provide message type
	if ( lastCommand.index == 1 ) {
		((char*)buffer)[0] = lastCommand.Msg.type;
		
		lastCommand.index++;
		return 1;
	}
	
	// from here on spool the string
	// consider: regarding the message type above the index is two to high.
	int textPos = lastCommand.index - 2;
	if ( textPos < 0 ) {
		// this is an error
		lastCommand.restLastCmd();
		return -1;
	}
		
	if ( (unsigned int)textPos < lastCommand.Msg.text.length() ) {
		((char*)buffer)[0] = (char)lastCommand.Msg.text[textPos];
		
		lastCommand.index++;
		return 1;
	}
	
	// close spooling
	if ( (unsigned int)textPos == lastCommand.Msg.text.length() ) {
		((char*)buffer)[0] = MSG_CLOSE;
		
		lastCommand.index++;
		return 1;
	}
	
	// finalize the command
	if ( (unsigned int)textPos > lastCommand.Msg.text.length() ) {
		((char*)buffer)[0] = RET_OK;
		
		lastCommand.restLastCmd();
		return 1;
	}
	
	// this is an error
	return -1;
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::readDefault(void *buffer, unsigned int nbByte, const char* response) {
///////////////////////////////////////////////////////////////////
	int ret = 0;
	
	if ( response != NULL ) {
		switch ( lastCommand.index ) {
			
			case 0: 	lastCommand.index++;
						ret = 1;
						((char*)buffer)[0] = RET_SOT;
						break;
						
			default:	ret = strlen(response);
						memcpy(buffer, response, ret);
						lastCommand.restLastCmd();
		}
	}
	
	return ret;
}
///////////////////////////////////////////////////////////////////
int SerialEmulatorNULL::readMove(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	int ret = 0;
	
	//RET_SOH, PID_XYZ_POS, X->getPosition(), Y->getPosition(), Z->getPosition()
	char tmpBuf[14];
	int32_t x=0, y=0, z=0;
	char* p = NULL;
	
	switch ( lastCommand.Mc.index ) {
		case 0:		lastCommand.Mc.index++;
					tmpBuf[0] = RET_SOH;
					
					ret = 1;
					break;
					
		case 1:		lastCommand.Mc.index++;
					tmpBuf[0] = getCurrentMoveCmdPID();
					
					ret = 1;
					break;
					
		case 2:		lastCommand.Mc.index++;
					getCurrentMoveCmdValues(x, y, z);
					
					p = tmpBuf;
					memcpy(p, &x, LONG_BUF_SIZE);

					p += LONG_BUF_SIZE;
					memcpy(p, &y, LONG_BUF_SIZE);

					p += LONG_BUF_SIZE;
					memcpy(p, &z, LONG_BUF_SIZE);
					
					ret = 12;
					break;

		default:	lastCommand.Mc.respondCounter++;

					if ( lastCommand.Mc.respondCounter == lastCommand.Mc.respondCount ) {
						tmpBuf[0] = RET_OK;
						ret = 1;
						lastCommand.resetM();
					} else {
						lastCommand.Mc.index = 0;
					}
	}

	memcpy(buffer, &tmpBuf, ret);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeData(void *b, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( connected == false )
		return false;
		
	spyWriteData(b, nbByte);
	
	char* buffer = ((char*)b);
	unsigned char cmd = buffer[0];
	
	switch ( cmd ) {
		
		case SIG_INTERRUPPT:
		case SIG_HALT:
		case SIG_PAUSE:
		case SIG_RESUME:			lastSignal = cmd;
									return true;

		case CMD_RESET_CONTROLLER:	reset();
									return true;
				
		case CMD_SETTER:			lastCommand.cmd = cmd; 
									return writeSetter(b, nbByte);
		
		case CMD_NEG_STEP_X:
		case CMD_POS_STEP_X:
		case CMD_NEG_STEP_Y:
		case CMD_POS_STEP_Y:
		case CMD_NEG_STEP_Z:
		case CMD_POS_STEP_Z:
		case CMD_MOVE:
		case CMD_RENDER_AND_MOVE:	lastCommand.cmd = cmd;
									return writeMoveCmd(b, nbByte);
		
		default:					lastCommand.cmd = cmd;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeSetter(void *b, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	char* buffer = ((char*)b);
	
	if ( nbByte == LONG_BUF_SIZE + 2 ) {
		unsigned char id = buffer[1];
		unsigned char valBuf[4];
		int32_t val = 0;
	
		valBuf[3] = buffer[2];
		valBuf[2] = buffer[3];
		valBuf[1] = buffer[4];
		valBuf[0] = buffer[5];
		memcpy(&val, valBuf, LONG_BUF_SIZE);
		
		ContollerInfo ci;
		ci.infoType    = CITSetterInfo;
		ci.setterId    = (int)id;
		ci.setterValue = val;
		cncControl->SerialControllerCallback(ci);
		
		// update setter map
		setterMap[(int)id] = val;
		
		// special handling for later use
		if ( (int)id == PID_POS_REPLY_THRESHOLD )
			posReplyThreshold = val;
		
		return true;
	}

	return false;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeMoveCmd(void *b, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	char* buffer = ((char*)b);
	int32_t x = 0L, y = 0L, z = 0L;
	
	if      ( lastCommand.cmd == CMD_NEG_STEP_X ) 	x = -1;
	else if ( lastCommand.cmd == CMD_POS_STEP_X ) 	x = +1;
	else if ( lastCommand.cmd == CMD_NEG_STEP_Y ) 	y = -1;
	else if ( lastCommand.cmd == CMD_POS_STEP_Y ) 	y = +1;
	else if ( lastCommand.cmd == CMD_NEG_STEP_Z ) 	z = -1;
	else if ( lastCommand.cmd == CMD_POS_STEP_Z ) 	z = +1;
	else if ( lastCommand.cmd == CMD_RENDER_AND_MOVE  || 
	          lastCommand.cmd == CMD_MOVE) {
		
		// update the current emulator position
		curEmulatorPos.set(cncControl->getCurPos());
		
		// Move cmd format
		// M....;			[len =  5]; only z axis
		// M........;		[len =  9]; x and y axis
		// M............;	[len = 13]; x, y and z axis
		
		unsigned int idx = 1;
		unsigned char buf[4];

		switch ( nbByte ) {
			case  5:	buf[3] = buffer[idx++];
						buf[2] = buffer[idx++];
						buf[1] = buffer[idx++];
						buf[0] = buffer[idx++];
						memcpy(&z, buf, LONG_BUF_SIZE);
						lastCommand.Mc.lastMoveX = x;
						lastCommand.Mc.lastMoveY = y;
						lastCommand.Mc.lastMoveZ = z;
						
						break;
						
			case 9:		lastCommand.Mc.lastMoveZ = z;
						buf[3] = buffer[idx++];
						buf[2] = buffer[idx++];
						buf[1] = buffer[idx++];
						buf[0] = buffer[idx++];
						memcpy(&x, buf, LONG_BUF_SIZE);
						lastCommand.Mc.lastMoveX = x;
						
						buf[3] = buffer[idx++];
						buf[2] = buffer[idx++];
						buf[1] = buffer[idx++];
						buf[0] = buffer[idx++];
						memcpy(&y, buf, LONG_BUF_SIZE);
						lastCommand.Mc.lastMoveY = y;
						
						break;
						
			case 13:	buf[3] = buffer[idx++];
						buf[2] = buffer[idx++];
						buf[1] = buffer[idx++];
						buf[0] = buffer[idx++];
						memcpy(&x, buf, LONG_BUF_SIZE);
						lastCommand.Mc.lastMoveX = x;
						
						buf[3] = buffer[idx++];
						buf[2] = buffer[idx++];
						buf[1] = buffer[idx++];
						buf[0] = buffer[idx++];
						memcpy(&y, buf, LONG_BUF_SIZE);
						lastCommand.Mc.lastMoveY = y;
						
						buf[3] = buffer[idx++];
						buf[2] = buffer[idx++];
						buf[1] = buffer[idx++];
						buf[0] = buffer[idx++];
						memcpy(&z, buf, LONG_BUF_SIZE);
						lastCommand.Mc.lastMoveZ = z;
						break;
			default:
						std::cerr << "SerialEmulatorNULL::handleMoveCmd error." << std::endl;
						std::cerr << " Invalid byte count: " << nbByte << std::endl;
						return false;
		}
	}
	
	// the emulator function readData and writeData runs in the same thread.
	// so, it isn't possible to repeat a move command with serval position callbacks
	// as a real mirco controller can do.
	// Instead the the move is supported with its total distance - see readMove. 
	// however, for a preview this is good enougth!
	//
	// the following linear rendering is only to support a more detailed writeMoveCmd(...)
	bool ret = renderMove(x, y, z, b, nbByte);
	
	// check for end state switches
	CncLimitStates ls;
	ls.setXYZLimit(cncControl->getControllerLimitState());
	
	if ( lastCommand.Mc.lastLimitState != ls.hasLimit() ) {
		lastCommand.Mc.lastLimitState = ls.hasLimit();
		lastCommand.Mc.respondCount = 2;
	}

	// reset last signal
	lastSignal = CMD_INVALID;
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::renderMove(int32_t dx , int32_t dy , int32_t dz, void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	// already rendered ?
	if ( lastCommand.cmd != CMD_RENDER_AND_MOVE )
		return provideMove(dx, dy, dz, buffer, nbByte, true);
	// else render above

	// initialize
	int i, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
	memset(&pointA, 0, sizeof(pointA));
	memset(&pointB, 0, sizeof(pointB));
	
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
			
			if ( provideMove(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte) == false )
				return false;
			
			if (err_1 > 0) {
				pointA[1] += y_inc;
				err_1     -= dx2;
			}
			if (err_2 > 0) {
				pointA[2] += z_inc;
				err_2     -= dx2;
			}
			err_1     += dy2;
			err_2     += dz2;
			pointA[0] += x_inc;
		}
	
	// -------------------------------------------------------------
	} else if ((m >= l) && (m >= n)) {
		err_1 = dx2 - m;
		err_2 = dz2 - m;
		for (i = 0; i < m; i++) {
			
			if ( provideMove(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte) == false )
				return false;
			
			if (err_1 > 0) {
				pointA[0] += x_inc;
				err_1     -= dy2;
			}
			if (err_2 > 0) {
				pointA[2] += z_inc;
				err_2     -= dy2;
			}
			err_1     += dx2;
			err_2     += dz2;
			pointA[1] += y_inc;
		}
		
	// -------------------------------------------------------------
	} else {
		err_1 = dy2 - n;
		err_2 = dx2 - n;
		for (i = 0; i < n; i++) {
			
			if ( provideMove(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte) == false )
				return false;
			
			if (err_1 > 0) {
				pointA[1] += y_inc;
				err_1     -= dz2;
			}
			if (err_2 > 0) {
				pointA[0] += x_inc;
				err_2     -= dz2;
			}
			err_1     += dy2;
			err_2     += dx2;
			pointA[2] += z_inc;
		}
	}
	
	// -------------------------------------------------------------
	if ( provideMove(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte, true) == false )
		return false;
		
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::provideMove(int32_t dx , int32_t dy , int32_t dz, void *buffer, unsigned int nbByte, bool force) {
///////////////////////////////////////////////////////////////////
	// statistic counting
	positionCounter++;
	if ( dx != 0 ) stepCounter++;
	if ( dy != 0 ) stepCounter++;
	if ( dz != 0 ) stepCounter++;
	
	// position management
	curEmulatorPos.incX(dx);
	curEmulatorPos.incY(dy);
	curEmulatorPos.incZ(dz);
	
	// signal handling
	switch ( lastSignal ) {
	
		case SIG_INTERRUPPT:
		case SIG_HALT:				return false;
		
		case SIG_PAUSE:				// pause handling
									while ( lastSignal == SIG_PAUSE ) {
										THE_APP->dispatchAll();
										THE_APP->waitActive(25, true);
									}
									break;
		
		case SIG_RESUME:			lastSignal = CMD_INVALID; 
									break;
		
		default:					; // Do nothing
	}
	
	// simulate a direct controler callback
	static CncLongPosition lastReplyPos;
	
	CncLongPosition diff(curEmulatorPos - lastReplyPos);
	
	#warning !!!!!
	//if ( true ) {
	if ( abs( diff.getX() ) > 40 || abs( diff.getY() ) > 40 || abs( diff.getZ() ) > 120) {
	
	/*
	repeatCount++;
	if ( repeatCount%posReplyThreshold == 0 || force == true ) {
	 */
		
		ContollerInfo ci;
		ci.infoType = CITPosition;
		ci.command  = lastCommand.cmd;
		ci.posType 	= PID_XYZ_POS;

		ci.xCtrlPos = curEmulatorPos.getX();
		ci.yCtrlPos = curEmulatorPos.getY();
		ci.zCtrlPos = curEmulatorPos.getZ();

		cncControl->SerialControllerCallback(ci);
		
		repeatCount = 0;
		lastReplyPos.set(curEmulatorPos);
	}
	
	// do something with this coordinates
	bool ret = writeMoveCmd(dx, dy, dz, buffer, nbByte);
	
	// copy point A into point B
	memcpy(&pointB, &pointA, sizeof(pointA));
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeMoveCmd(int32_t x, int32_t y, int32_t z, void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	// inheried classes do the work here
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::resetPostionCounter() {
///////////////////////////////////////////////////////////////////
	positionCounter = 0;
}
///////////////////////////////////////////////////////////////////
size_t SerialEmulatorNULL::getPostionCounter() {
///////////////////////////////////////////////////////////////////
	return positionCounter;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorNULL::resetStepCounter() {
///////////////////////////////////////////////////////////////////
	stepCounter = 0;
}
///////////////////////////////////////////////////////////////////
size_t SerialEmulatorNULL::getStepCounter() {
///////////////////////////////////////////////////////////////////
	return stepCounter;
}

