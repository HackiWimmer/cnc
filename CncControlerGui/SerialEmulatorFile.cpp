#include "SerialEmulatorFile.h"

///////////////////////////////////////////////////////////////////
bool SerialEmulatorFile::connect(const char* fileName) {
///////////////////////////////////////////////////////////////////
	portName = fileName;
	
	fileStream.open(fileName, fileFlags);
	if ( fileStream.is_open() ) {
		connected = true;
	} else {
		std::cerr << "SerialEmulatorFile::connect: Failed" << std::endl;
		std::cerr << "Can't open file: '" << fileName << "'" << std::endl;
		connected = false;
	}

	return connected;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorFile::disconnect(void) {
///////////////////////////////////////////////////////////////////
	fileStream.close();
	connected = false;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorFile::writeMoveCmd(int32_t x , int32_t y , int32_t z, void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( isMoveCommand(lastCommand.cmd) == false )
		return true;
	
	static const int size = LONG_BUF_SIZE * 3 + 1;
	char moveCommand[size];
	char* p = moveCommand;
	int32_t v = 0L;
	
	moveCommand[0] = lastCommand.cmd;

	v = htonl(x);
	p++;
	memcpy(p, &v, LONG_BUF_SIZE);
		
	v = htonl(y);
	p += LONG_BUF_SIZE;
	memcpy(p, &v, LONG_BUF_SIZE);
	
	v = htonl(z);
	p += LONG_BUF_SIZE;
	memcpy(p, &v, LONG_BUF_SIZE);

	//std::clog << x << "," << y << "," << z << std::endl;
	fileStream.write(moveCommand, size);
	return true;
}