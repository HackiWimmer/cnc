#include <fstream>
#include <wx/file.h>
#include "CncConfig.h"
#include "CncCommandDecoder.h"
#include "SerialEmulatorTextStreamer.h"

///////////////////////////////////////////////////////////////////
SerialEmulatorTextStreamer::SerialEmulatorTextStreamer(CncControl* cnc)
: SerialEmulatorNULL(cnc)
, fileName("")
, currentSpeedMode(CncSpeedUserDefined)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
//Initialize Serial communication with the given COM port
SerialEmulatorTextStreamer::SerialEmulatorTextStreamer(const char* fileName) 
: SerialEmulatorNULL(fileName)
, fileName("")
, currentSpeedMode(CncSpeedUserDefined)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
SerialEmulatorTextStreamer::~SerialEmulatorTextStreamer() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorTextStreamer::isOutputAsTemplateAvailable() {
///////////////////////////////////////////////////////////////////
	if ( wxFile::Exists(getPortName()) == true )
		return true;
	
	return false; 
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorTextStreamer::connect(const char* fileName) {
///////////////////////////////////////////////////////////////////
	this->fileName.assign(fileName);
	
	setConnected(true);
	return isConnected();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::disconnect() {
/////////////////////////////////////////////////////////////////
	setConnected(false);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorTextStreamer::writeSetterRawCallback(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( buffer == NULL || nbByte == 0 ) {
		std::cerr << "SerialEmulatorTextStreamer::writeSetterRawCallback(): Empty buffer!" << std::endl;
		return false;
	}
	
	CncCommandDecoder::SetterInfo csi;
	if ( CncCommandDecoder::decodeSetter(buffer, nbByte, csi) == false ) {
		std::cerr << "SerialEmulatorTextStreamer::writeSetterRawCallback(): Decode failed!" << std::endl;
		return false;
	}
	
	SetterInfo si;
	si.pid 		= csi.pid;
	si.values	= csi.values;
	
	if ( si.pid == PID_SPEED_FEED_MODE ) {
		si.values	= csi.values;
		
		auto checkValue = [&]() {
			if ( si.values.size() <= 0 ) {
				std::cerr << "SerialEmulatorTextStreamer::writeSetterRawCallback(): Empty setter values!" << std::endl;
				return false;
			}
			
			int32_t value = si.values.front();
			if ( value < CncSpeedWork || value > CncSpeedUserDefined ) {
				std::cerr << "SerialEmulatorTextStreamer::writeSetterRawCallback(): Invalid setter value: '" 
				          << value
				          << "'" << std::endl;
				return false;
			}
			
			return true;
		};
		
		if ( checkValue() == true )
			currentSpeedMode = (CncSpeedMode)si.values.front();
	}
	
	return writeEncodedSetterCallback(si);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorTextStreamer::writeMoveRawCallback(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( buffer == NULL || nbByte == 0 ) {
		std::cerr << "SerialEmulatorTextStreamer::writeMoveRawCallback(): Empty buffer!" << std::endl;
		return false;
	}
	
	MoveInfo mi;
	mi.speedMode 	= currentSpeedMode;
	mi.cmd 			= buffer[0];
	
	if ( CncCommandDecoder::decodeMove(buffer, nbByte, mi.sdx, mi.sdy, mi.sdz) == false ) {
		std::cerr << "SerialEmulatorTextStreamer::writeMoveRawCallback(): Decode failed!" << std::endl;
		return false;
	}
	
	mi.mdx = GBL_CONFIG->convertStepsToMetricX(mi.sdx);
	mi.mdy = GBL_CONFIG->convertStepsToMetricX(mi.sdy);
	mi.mdz = GBL_CONFIG->convertStepsToMetricX(mi.sdz);
	
	return writeEncodedMoveCallback(mi);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorTextStreamer::writeEncodedSetterCallback(const SetterInfo& si) {
///////////////////////////////////////////////////////////////////
	bodyStream << " Setter('" << ArduinoPIDs::getPIDLabel(si.pid) << "[" << (int)si.pid << "]', values[ ";
	
	for ( auto it = si.values.begin(); it != si.values.end(); ++it)
		bodyStream << *it << " ";
		
	bodyStream << "])\n";
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorTextStreamer::writeEncodedMoveCallback(const MoveInfo& mi) {
///////////////////////////////////////////////////////////////////
	bodyStream << "  Move(" << mi.sdx << ", " << mi.sdy << ", " << mi.sdz << ")\n";
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::processTrigger(const Serial::Trigger::BeginRun& tr) {
///////////////////////////////////////////////////////////////////
	bodyStream.str("");
	initializeFile(tr);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::processTrigger(const Serial::Trigger::EndRun& tr) {
///////////////////////////////////////////////////////////////////
	finalizeFile(tr);
	
	if ( fileName.IsEmpty() == true ) {
		std::cerr << "SerialEmulatorTextStreamer::processTrigger(): INvalid filename!" <<  std::endl; 
		return;
	}
	
	std::ofstream out(fileName, std::ofstream::out);
	if ( out.good() == false ) {
		std::cerr << "SerialEmulatorTextStreamer::processTrigger(): Can't create file: '" 
				  << fileName
				  << "'" << std::endl;
		return;
	}
	
	out << bodyStream.str();
	out.close();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::initializeFile(const Serial::Trigger::BeginRun& tr) {
///////////////////////////////////////////////////////////////////
	bodyStream << "File start\n";
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::finalizeFile(const Serial::Trigger::EndRun& tr) {
///////////////////////////////////////////////////////////////////
	bodyStream << "File end\n";
}

