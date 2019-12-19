#include "MainFrame.h"
#include "CncConfig.h"
#include "SerialCircularBuffer.h"
#include "SerialThreadStub.h"

///////////////////////////////////////////////////////////////////
SerialThreadStub::SerialThreadStub(CncControl* cnc)
: SerialSpyPort(cnc)
, readBuffer(new SerialCircularBuffer())
, writeBuffer(new SerialCircularBuffer())
///////////////////////////////////////////////////////////////////
{
	wxASSERT( THE_APP->isSerialThreadInstalled() == true );
}
//////////////////////////////////////////////////////////////////
SerialThreadStub::SerialThreadStub(const char *portName)
: SerialSpyPort(portName) 
, readBuffer(new SerialCircularBuffer())
, writeBuffer(new SerialCircularBuffer())
///////////////////////////////////////////////////////////////////
{
	wxASSERT( THE_APP->isSerialThreadInstalled() == true );
}
///////////////////////////////////////////////////////////////////
SerialThreadStub::~SerialThreadStub() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
bool SerialThreadStub::connect(const char* portName) { 
///////////////////////////////////////////////////////////////////
	wxASSERT( THE_APP->isSerialThreadInstalled() == true );

	if ( THE_APP->isSerialThreadRunning() == false )
		THE_APP->resumeSerialThread();

	if ( THE_APP->isSerialThreadRunning() == false )
		return false;
		
		
		
	return true; 
}
///////////////////////////////////////////////////////////////////
void SerialThreadStub::disconnect(void) {
///////////////////////////////////////////////////////////////////
	wxASSERT( THE_APP->isSerialThreadInstalled() == true );

	if ( THE_APP->isSerialThreadRunning() == false )
		THE_APP->pauseSerialThread();
}






		void SerialThreadStub::onPeriodicallyAppEvent(bool interrupted) {}
		// simulate connection
		// close the connection
		// simulate read
		int SerialThreadStub::readData(void *buffer, unsigned int nbByte) { return 42; }
		// simulate write
		bool SerialThreadStub::writeData(void *buffer, unsigned int nbByte) { return false; }
