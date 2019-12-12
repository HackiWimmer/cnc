#include "CncArduino.h"
#include "MainFrame.h"
#include "SerialSimulatorBase.h"

///////////////////////////////////////////////////////////////////
SerialSimulatorThread::SerialSimulatorThread() 
: wxThread(wxTHREAD_DETACHED)
, writeDataQueue()
, readDataQueue()
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
SerialSimulatorThread::~SerialSimulatorThread() {
///////////////////////////////////////////////////////////////////
	writeDataQueue.reset();
	readDataQueue.reset();
}
///////////////////////////////////////////////////////////////////
wxThread::ExitCode SerialSimulatorThread::Entry() {
///////////////////////////////////////////////////////////////////
	unsigned int sleep = 1;
	
	while ( !TestDestroy() ) {
		process();
		this->Sleep(sleep);
	}
	
	return NULL;
}
///////////////////////////////////////////////////////////////////
int SerialSimulatorThread::readData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	static const unsigned int MAX_ITEMS = 32000;
	static SerialByte serialBytes[MAX_ITEMS];
	
	unsigned int sizeAvailable = readDataQueue.pop(serialBytes, nbByte);
	
	if ( sizeAvailable == 0 )
		return 0;
	
	memcpy(buffer, serialBytes, nbByte);
	return sizeAvailable;
}
///////////////////////////////////////////////////////////////////
bool SerialSimulatorThread::writeData(void *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( buffer == NULL || nbByte == 0 )
		return true;
	
	writeDataQueue.push(SerialData(buffer, nbByte));
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::process() {
///////////////////////////////////////////////////////////////////
	static SerialData serialData;
	
	if ( writeDataQueue.pop(serialData) == false )
		return;
		
	SerialDataReader reader(serialData);
	if ( reader.available() <= 0 )
		return;
		
	unsigned char cmd = reader.read();
	unsigned char ret = RET_OK;
	
	switch ( cmd ) {
		
		// --------------------------------------------------------------------------
		// Commands - single byte return
		// - return default --> r = RET_OK
		// --------------------------------------------------------------------------
		case CMD_RESET_CONTROLLER:		ret = reset(); 
										break;
		
		// --------------------------------------------------------------------------
		// Commands - multi byte return
		// - must always return directly
		// --------------------------------------------------------------------------
		case CMD_PRINT_VERSION: 		Serial_write(RET_SOT);
										Serial_write(FIRMWARE_VERSION);
										return;
		
		// --------------------------------------------------------------------------
		// Error handling
		// --------------------------------------------------------------------------
		default:						// with respect to the fact that not all commands
										// are managed in this case return default RET_OK
										ret = RET_OK;
	}
	
	Serial_write(ret);
}
///////////////////////////////////////////////////////////////////
unsigned char SerialSimulatorThread::reset() {
///////////////////////////////////////////////////////////////////
	// to do something here
	
	return RET_OK;
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::Serial_write(const unsigned char b) { 
///////////////////////////////////////////////////////////////////
	readDataQueue.push(SerialByte(b));
}
///////////////////////////////////////////////////////////////////
void SerialSimulatorThread::Serial_write(const char* response) { 
///////////////////////////////////////////////////////////////////
	if ( response == NULL )
		return;
	
	unsigned int size = strlen(response);
	if ( size > maxByteArraySize ) {
		size = maxByteArraySize;

		wxThreadEvent evt(wxEVT_TRACE_FROM_THREAD, MainFrame::EventId::POST_ERROR);
		evt.SetString("SerialFacadeThread::Serial_write: The given response is to large");
		wxPostEvent(THE_CONFIG->getTheApp(), evt);
	}
	
	memcpy(bytes, response, size);
	
	if ( readDataQueue.push(bytes, size) != size ) {
		
		wxThreadEvent evt(wxEVT_TRACE_FROM_THREAD, MainFrame::EventId::POST_ERROR);
		evt.SetString("SerialFacadeThread::Serial_write: readDataQueue.push failed");
		wxPostEvent(THE_CONFIG->getTheApp(), evt);
	}
}