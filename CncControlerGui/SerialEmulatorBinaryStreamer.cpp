#include <wx/file.h>
#include "SerialEmulatorBinaryStreamer.h"

///////////////////////////////////////////////////////////////////
SerialEmulatorBinaryStreamer::SerialEmulatorBinaryStreamer(CncControl* cnc)
: SerialEmulatorNULL			(cnc)
, CncBinaryTemplateStreamer		()
, fileName						("")
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
	//std::cout << CNC_LOG_FUNCT_A("\n %s\n exists == %d\n", getPortName(), wxFile::Exists(getPortName()));

	switch ( inboundFormat ) {
		case TplBinary:
		case TplUnknown:
		case TplTest:		return false;
		default:			return wxFile::Exists(getPortName());
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
bool SerialEmulatorBinaryStreamer::writeSetterRawCallback(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	return appendDataBlock(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorBinaryStreamer::writeMoveRawCallback(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	return appendDataBlock(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorBinaryStreamer::writeMoveSequenceRawCallback(unsigned char* buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	//std::cout << nbByte << " -> " << (int)buffer[0] << " " << (int)buffer[1] << " " << (int)buffer[2] << " " << (int)buffer[3] << " " << (int)buffer[4] << " " << std::endl;
	return appendDataBlock(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorBinaryStreamer::processTrigger(const Trigger::BeginRun& tr) {
///////////////////////////////////////////////////////////////////
	Serial::processTrigger(tr);
	
	if ( isReadyToStream() )
		CncBinaryTemplateStreamer::finalize();
	
	Trigger::ParameterSet ps = tr.parameter;
	ps.fullFileName = fileName;
	
	inboundFormat = cnc::getTemplateFormatFromExtention(ps.SRC.fileType);
	
	if ( initNextSourceTemplateFileName(ps) == false) {
		std::cerr << CNC_LOG_FUNCT_A(": initNextSourceTemplateFileName failed\n");
		disconnect();
	}
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorBinaryStreamer::processTrigger(const Trigger::EndRun& tr) {
///////////////////////////////////////////////////////////////////
	Serial::processTrigger(tr);
	
	if ( CncBinaryTemplateStreamer::finalize() == false )
		std::cerr << CNC_LOG_FUNCT_A(": finalize() failed\n");
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorBinaryStreamer::processTrigger(const Trigger::NextPath& tr) {
///////////////////////////////////////////////////////////////////
	Serial::processTrigger(tr);
	//std::clog << CNC_LOG_FUNCT_A("\n");
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorBinaryStreamer::processTrigger(const Trigger::GuidePath& tr) {
///////////////////////////////////////////////////////////////////
	Serial::processTrigger(tr);
	#warning processTrigger(const Trigger::GuidePath& tr) impl. missing 
}
