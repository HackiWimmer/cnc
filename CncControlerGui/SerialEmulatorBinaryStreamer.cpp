#include <wx/file.h>
#include "SerialEmulatorBinaryStreamer.h"

///////////////////////////////////////////////////////////////////
SerialEmulatorBinaryStreamer::SerialEmulatorBinaryStreamer(CncControl* cnc)
: SerialEmulatorNULL(cnc)
, CncBinaryTemplateStreamer()
, fileName("")
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
//Initialize Serial communication with the given COM port
SerialEmulatorBinaryStreamer::SerialEmulatorBinaryStreamer(const char* fileName) 
: SerialEmulatorNULL(fileName)
, CncBinaryTemplateStreamer()
, fileName("")
, inboundFormat(TplUnknown)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
SerialEmulatorBinaryStreamer::~SerialEmulatorBinaryStreamer() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorBinaryStreamer::isOutputAsTemplateAvailable() {
///////////////////////////////////////////////////////////////////
	switch ( inboundFormat ) {
		case TplBinary:
		case TplUnknown:
		case TplTest:		return false;
		
		default:			if ( wxFile::Exists(getPortName()) == true )
								return true;
	}
	
	return false; 
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorBinaryStreamer::connect(const char* fileName) {
///////////////////////////////////////////////////////////////////
	this->fileName.assign(fileName);
	
	setConnected(true);
	return isConnected();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorBinaryStreamer::disconnect() {
/////////////////////////////////////////////////////////////////
	setConnected(false);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorBinaryStreamer::writeMoveRawCallback(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	return appendDataBlock(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorBinaryStreamer::writeSetterRawCallback(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	return appendDataBlock(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorBinaryStreamer::processTrigger(const Serial::Trigger::BeginRun& tr) {
///////////////////////////////////////////////////////////////////
	if ( isReadyToStream() )
		finalize();
	
	CncBinaryTemplateStreamer::ParameterSet ps = tr.parameter;
	ps.fullFileName = fileName;
	
	inboundFormat = cnc::getTemplateFormatFromExtention(ps.SRC.fileType);
	
	if ( initNextSourceTemplateFileName(ps) == false) {
		std::cerr << "SerialEmulatorStreamer::processTrigger(BeginRun): initNextSourceTemplateFileName failed" << std::endl;
		disconnect();
	}
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorBinaryStreamer::processTrigger(const Serial::Trigger::EndRun& tr) {
///////////////////////////////////////////////////////////////////
	if ( finalize() == false ) {
		std::cerr << "SerialEmulatorStreamer::processTrigger(TrEndRun): finalize() failed" << std::endl;
	}
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorBinaryStreamer::processTrigger(const Serial::Trigger::NextPath& tr) {
///////////////////////////////////////////////////////////////////
	//std::clog << "SerialEmulatorStreamer::processTrigger(TrNextPath)" << std::endl;
}

