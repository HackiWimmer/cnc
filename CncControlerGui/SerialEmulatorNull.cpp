#include "CncControl.h"
#include "SerialEmulatorNULL.h"

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
	
	// debug only
	if ( false ) {
		std::clog << lastCommand.cmd << ", "
		          << cncControl->getCurPos().getX() << ", "
				  << cncControl->getCurPos().getY() << std::endl;
	}

	wxString firmWare(wxString::Format("%s", FIRMWARE_VERSION));
	wxString retStr;
	switch( lastCommand.cmd ) { 
		case 'm':
		case 'M':	ret = readMove(buffer, nbByte);
					break;
					
		case 'V': 	ret = readDefault(buffer, nbByte, firmWare);
					break;
					
		case '?': 	ret = readDefault(buffer, nbByte, "1:0:Not available, because there's no controller connection\n");
					break;
					
		case 'c': 	ret = readDefault(buffer, nbByte, getConfiguration(retStr));
					break;
					
		case 'Q': 	ret = readDefault(buffer, nbByte, wxString::Format("%i:0:0:0\n", MAX_PINS)); // see DataControlModel for more details
					break;
					
		default: 	((char*)buffer)[0] = RET_OK;
					ret = 1;
					lastCommand.restLastCmd();
	}
	
	spyReadData(ret, buffer, nbByte);
	return ret;
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
	lastCommand.cmd = buffer[0];
	
	switch ( lastCommand.cmd ) {
		case 'S':	return writeSetter(b, nbByte);
		case 'x':
		case 'X':
		case 'y':
		case 'Y':
		case 'z':
		case 'Z':
		case 'm':
		case 'M':	return writeMoveCmd(b, nbByte);
		default:	; // Do nothing
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
		
		setterMap[(int)id] = val;
		return true;
	}

	return false;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeMoveCmd(void *b, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	char* buffer = ((char*)b);
	int32_t x = 0L, y = 0L, z = 0L;
	
	if      ( lastCommand.cmd == 'x' ) 	x = -1;
	else if ( lastCommand.cmd == 'X' ) 	x = +1;
	else if ( lastCommand.cmd == 'y' ) 	y = -1;
	else if ( lastCommand.cmd == 'Y' ) 	y = +1;
	else if ( lastCommand.cmd == 'z' ) 	z = -1;
	else if ( lastCommand.cmd == 'Z' ) 	z = +1;
	else if ( lastCommand.cmd == 'M'  || lastCommand.cmd == 'm' ) {
		
		//M....;			[len =  5]; only z axis
		//M........;		[len =  9]; x and y axis
		//M............;	[len = 13]; x, y and z axis
		
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
	
	// todo
	bool ret = renderMove(x, y, z, b, nbByte);
	//bool ret = writeMoveCmd(x, y, z, b, nbByte);
	
	// check for end state switches
	CncLimitStates ls;
	ls.setXYZLimit(cncControl->getControllerLimitState());
	
	if ( lastCommand.Mc.lastLimitState != ls.hasLimit() ) {
		lastCommand.Mc.lastLimitState = ls.hasLimit();
		lastCommand.Mc.respondCount = 2;
	}

	return ret;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::renderMove(int32_t x , int32_t y , int32_t z, void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( lastCommand.cmd != 'M' ) {
		return writeMoveCmd(x, y, z, buffer, nbByte);
	}
	
	// render here
	int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
	int pointA[3], pointB[3];

	pointA[0] = 0;
	pointA[1] = 0;
	pointA[2] = 0;
	
	pointB[0] = 0;
	pointB[1] = 0;
	pointB[2] = 0;
	
	dx = x;
	dy = y;
	dz = z;
	
	x_inc = (dx < 0) ? -1 : 1;
	l = abs(dx);
	
	y_inc = (dy < 0) ? -1 : 1;
	m = abs(dy);
	
	z_inc = (dz < 0) ? -1 : 1;
	n = abs(dz);
	
	dx2 = l << 1;
	dy2 = m << 1;
	dz2 = n << 1;

	if ((l >= m) && (l >= n)) {
		err_1 = dy2 - l;
		err_2 = dz2 - l;
		for (i = 0; i < l; i++) {
			
			//output
			if ( writeMoveCmd(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte) == false )
				return false;
		
			for (int j=0; j<3; j++ )
				pointB[j] = pointA[j];
			
			if (err_1 > 0) {
				pointA[1] += y_inc;
				err_1 -= dx2;
			}
			if (err_2 > 0) {
				pointA[2] += z_inc;
				err_2 -= dx2;
			}
			err_1 += dy2;
			err_2 += dz2;
			pointA[0] += x_inc;
		}
	} else if ((m >= l) && (m >= n)) {
		err_1 = dx2 - m;
		err_2 = dz2 - m;
		for (i = 0; i < m; i++) {
			
			//output
			if ( writeMoveCmd(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte) == false )
				return false;
		
			for (int j=0; j<3; j++ )
				pointB[j] = pointA[j];
				
			if (err_1 > 0) {
				pointA[0] += x_inc;
				err_1 -= dy2;
			}
			if (err_2 > 0) {
				pointA[2] += z_inc;
				err_2 -= dy2;
			}
			err_1 += dx2;
			err_2 += dz2;
			pointA[1] += y_inc;
		}
	} else {
		err_1 = dy2 - n;
		err_2 = dx2 - n;
		for (i = 0; i < n; i++) {
			
			//output
			if ( writeMoveCmd(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte) == false )
				return false;
		
			for (int j=0; j<3; j++ )
				pointB[j] = pointA[j];
				
			if (err_1 > 0) {
				pointA[1] += y_inc;
				err_1 -= dz2;
			}
			if (err_2 > 0) {
				pointA[0] += x_inc;
				err_2 -= dz2;
			}
			err_1 += dy2;
			err_2 += dx2;
			pointA[2] += z_inc;
		}
	}
	
	//output
	if ( writeMoveCmd(pointA[0] - pointB[0], pointA[1] - pointB[1], pointA[2] - pointB[2], buffer, nbByte) == false )
		return false;
		
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorNULL::writeMoveCmd(int32_t x, int32_t y, int32_t z, void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	// inheried classes do the work here
	return true;
}

