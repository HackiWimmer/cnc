#include <fstream>
#include <wx/file.h>
#include "CncConfig.h"
#include "CncCommandDecoder.h"
#include "SerialEmulatorTextStreamer.h"

namespace Streamer {
	const char* indent1 = "\t";
	const char* indent2 = "\t\t";
	const char* indent3 = "\t\t\t";
	
	const unsigned int BUFFER_SIZE = 64 * 1024;
	char buffer [BUFFER_SIZE];
};

///////////////////////////////////////////////////////////////////
SerialEmulatorTextStreamer::SerialEmulatorTextStreamer(CncControl* cnc)
: SerialEmulatorNULL(cnc)
, fileName("")
, startPos(0.0, 0.0, 0.0)
, currentSpeedMode(CncSpeedUserDefined)
, currentSpeedValue(0.0)
, metricBoundbox()
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
//Initialize Serial communication with the given COM port
SerialEmulatorTextStreamer::SerialEmulatorTextStreamer(const char* fileName) 
: SerialEmulatorNULL(fileName)
, fileName("")
, currentSpeedMode(CncSpeedUserDefined)
, currentSpeedValue(0.0)
, metricBoundbox()
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
const wxString& SerialEmulatorTextStreamer::formatPosition(const int32_t value)          const {
	static wxString ret;

	ret.assign(wxString::Format("%d", (long)value));
	return ret;
}
///////////////////////////////////////////////////////////////////
const wxString& SerialEmulatorTextStreamer::formatPosition(const double value)           const {
	static wxString ret;
	ret.assign(wxString::Format("%.3lf", value));
	return ret;
}
///////////////////////////////////////////////////////////////////
const wxString& SerialEmulatorTextStreamer::formatPosition(const CncLongPosition& pos)   const {
///////////////////////////////////////////////////////////////////
	static wxString ret;
	ret.assign(wxString::Format("%ld %ld %ld", (long)pos.getX(), (long)pos.getY(), (long)pos.getZ()));
	return ret;
}
///////////////////////////////////////////////////////////////////
const wxString& SerialEmulatorTextStreamer::formatPosition(const CncDoublePosition& pos) const {
///////////////////////////////////////////////////////////////////
	static wxString ret;
	ret.assign(wxString::Format("%.3lf %.3lf %.3lf", pos.getX(), pos.getY(), pos.getZ()));
	return ret;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::notifySetter(const CncCommandDecoder::SetterInfo& si) {
///////////////////////////////////////////////////////////////////
	// currently nothing to do
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
			
	} else if ( si.pid == PID_SPEED_MM_MIN ) {
		si.values	= csi.values;
		
		auto checkValue = [&]() {
			if ( si.values.size() <= 0 ) {
				std::cerr << "SerialEmulatorTextStreamer::writeSetterRawCallback(): Empty setter values!" << std::endl;
				return false;
			}
			
			return true;
		};
		
		if ( checkValue() == true ) 
			currentSpeedValue = (double)si.values.front() / DBL_FACT;
	}
	
	return writeEncodedSetterCallback(si);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::notifyMove(int32_t dx, int32_t dy, int32_t dz, int32_t f) {
///////////////////////////////////////////////////////////////////
	bodyStream << Streamer::indent3 << wxString::Format("<steps x=\"%ld\" y=\"%ld\" z=\"%ld\" f=\"%ld\"/>\n",
			                                            (long)dx, (long)dy, (long)dz, (long)f);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::notifyMoveSequenceBegin(const CncCommandDecoder::MoveSequence& sequence) {
///////////////////////////////////////////////////////////////////
	bodyStream << Streamer::indent2 << wxString::Format("<MoveSequenceBegin cmd=\"%c\" description=\"%s\"/>\n",
			                                            sequence.cmd, ArduinoCMDs::getCMDLabel(sequence.cmd));


}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::notifyMoveSequenceNext(const CncCommandDecoder::MoveSequence& sequence) {
///////////////////////////////////////////////////////////////////
	bodyStream << Streamer::indent2 << wxString::Format("<MoveSequenceNext cmd=\"%c\" description=\"%s\"/>\n",
			                                            sequence.cmd, ArduinoCMDs::getCMDLabel(sequence.cmd));

}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::notifyMoveSequenceEnd(const CncCommandDecoder::MoveSequence& sequence) {
///////////////////////////////////////////////////////////////////
	bodyStream << Streamer::indent2 << wxString::Format("<MoveSequenceEnd cmd=\"%c\" description=\"%s\"/>\n",
			                                            sequence.cmd, ArduinoCMDs::getCMDLabel(sequence.cmd));
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
	mi.speedValue	= currentSpeedValue;
	mi.cmd 			= buffer[0];
	
	// reset speed value
	if ( cnc::dblCompareNull(currentSpeedValue) == false )
		currentSpeedValue = 0.0;
	
	if ( CncCommandDecoder::decodeMove(buffer, nbByte, mi.sdx, mi.sdy, mi.sdz) == false ) {
		std::cerr << "SerialEmulatorTextStreamer::writeMoveRawCallback(): Decode failed!" << std::endl;
		return false;
	}
	
	mi.mdx = GBL_CONFIG->convertStepsToMetricX(mi.sdx);
	mi.mdy = GBL_CONFIG->convertStepsToMetricY(mi.sdy);
	mi.mdz = GBL_CONFIG->convertStepsToMetricZ(mi.sdz);
		
	return writeEncodedMoveCallback(mi);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorTextStreamer::writeMoveSequenceRawCallback(unsigned char *buffer, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	// Nothing to do here
	return SerialEmulatorNULL::writeMoveSequenceRawCallback(buffer, nbByte);
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorTextStreamer::writeEncodedSetterCallback(const SetterInfo& si) {
///////////////////////////////////////////////////////////////////
	bodyStream << Streamer::indent1 << wxString::Format("<!-- %s -->", ArduinoPIDs::getPIDLabel(si.pid)) << std::endl;
	bodyStream << Streamer::indent1 << wxString::Format("<Setter pid=\"%u\" description=\"%s\" ", (int)si.pid, ArduinoPIDs::getPIDLabel(si.pid));
	
	bodyStream << "Values=\"";
	wxString values;
	for ( auto it = si.values.begin(); it != si.values.end(); ++it) {
		if ( si.pid >= PID_DOUBLE_RANG_START && si.pid <= PID_DOUBLE_RANG_END )		values.append(wxString::Format("%.3lf ", (double)(*it) / DBL_FACT));
		else																		values.append(wxString::Format("%ld ",   (long)(*it) ));
	}
		
	values.assign(values.Trim(false).Trim(true));
	bodyStream << values << "\"/>\n";
	
	return true;
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorTextStreamer::writeEncodedMoveCallback(const MoveInfo& mi) {
///////////////////////////////////////////////////////////////////
	CncDoublePosition dPos;
	GBL_CONFIG->convertStepsToMetric(dPos, getCurrentEmulatorPosition());
	
	bodyStream << Streamer::indent2 << wxString::Format("<Move cmd=\"%c\" description=\"%s\" speedMode=\"%c\"\n", mi.cmd, ArduinoCMDs::getCMDLabel(mi.cmd), cnc::getCncSpeedTypeAsCharacter(mi.speedMode));
		bodyStream << Streamer::indent3 << wxString::Format("steps=\"%ld %ld %ld\"\n",				(long)mi.sdx, (long)mi.sdy, (long)mi.sdz);
		bodyStream << Streamer::indent3 << wxString::Format("metrixs=\"%.3lf %.3lf %.3lf\"\n", 		mi.mdx, mi.mdy, mi.mdz);
		bodyStream << Streamer::indent3 << wxString::Format("posStart=\"%.3lf %.3lf %.3lf\"\n", 	dPos.getX(), dPos.getY(), dPos.getZ());
		bodyStream << Streamer::indent3 << wxString::Format("posEnd=\"%.3lf %.3lf %.3lf\"\n", 		dPos.getX() + mi.mdx, dPos.getY() + mi.mdy, dPos.getZ() + mi.mdz);
	bodyStream << Streamer::indent3 << " />\n";
	
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::processTrigger(const Serial::Trigger::BeginRun& tr) {
///////////////////////////////////////////////////////////////////
	headerStream.str("");
	bodyStream.str("");
	footerStream.str("");
	initializeFile(tr);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::processTrigger(const Serial::Trigger::EndRun& tr) {
///////////////////////////////////////////////////////////////////
	CncDoublePosition dPos;
	GBL_CONFIG->convertStepsToMetric(dPos, getCurrentEmulatorPosition());

	CncLongPosition::Watermarks wm;
	getCurrentEmulatorPosition().getWatermarks(wm);
	metricBoundbox.minX 		= GBL_CONFIG->convertStepsToMetricX(wm.xMin);
	metricBoundbox.maxX 		= GBL_CONFIG->convertStepsToMetricX(wm.xMax);
	metricBoundbox.minY 		= GBL_CONFIG->convertStepsToMetricY(wm.yMin);
	metricBoundbox.maxY 		= GBL_CONFIG->convertStepsToMetricY(wm.yMax);
	metricBoundbox.minZ 		= GBL_CONFIG->convertStepsToMetricZ(wm.zMin);
	metricBoundbox.maxZ 		= GBL_CONFIG->convertStepsToMetricZ(wm.zMax);
	metricBoundbox.distanceX	= fabs(metricBoundbox.minX) + fabs(metricBoundbox.maxX);
	metricBoundbox.distanceY	= fabs(metricBoundbox.minY) + fabs(metricBoundbox.maxY);
	metricBoundbox.distanceZ	= fabs(metricBoundbox.minZ) + fabs(metricBoundbox.maxZ);
	
	finalizeFile(tr);
	
	if ( fileName.IsEmpty() == true ) {
		std::cerr << "SerialEmulatorTextStreamer::processTrigger(): INvalid filename!" <<  std::endl; 
		return;
	}
	
	std::ofstream out(fileName.c_str().AsChar(), std::ofstream::out);
	out.rdbuf()->pubsetbuf(Streamer::buffer, Streamer::BUFFER_SIZE);
	if ( out.good() == false ) {
		std::cerr << "SerialEmulatorTextStreamer::processTrigger(): Can't create file: '" 
				  << fileName
				  << "'" << std::endl;
		return;
	}
	
	out << headerStream.str();
	out << bodyStream.str();
	out << footerStream.str();
	out.close();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::processTrigger(const Serial::Trigger::NextPath& tr) {
///////////////////////////////////////////////////////////////////
	initializePath(tr);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::initializeFile(const Serial::Trigger::BeginRun& tr) {
///////////////////////////////////////////////////////////////////
	headerStream << "<File>\n";
	
	CncDoublePosition dPos;
	GBL_CONFIG->convertStepsToMetric(dPos, getCurrentEmulatorPosition());
	startPos.set(dPos);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::initializePath(const Serial::Trigger::NextPath& tr) {
///////////////////////////////////////////////////////////////////
	bodyStream << Streamer::indent1 << wxString::Format("<Next Path />\n");
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorTextStreamer::finalizeFile(const Serial::Trigger::EndRun& tr) {
///////////////////////////////////////////////////////////////////
	headerStream << Streamer::indent1 << "<Boundbox unit=\"mm\"\n";
	headerStream << Streamer::indent2 << wxString::Format("minX=\"%.3lf\" ",  metricBoundbox.minX) 
	                                  << wxString::Format("maxX=\"%.3lf\"\n", metricBoundbox.maxX);
									  
	headerStream << Streamer::indent2 << wxString::Format("minY=\"%.3lf\" ",  metricBoundbox.minY) 
	                                  << wxString::Format("maxY=\"%.3lf\"\n", metricBoundbox.maxY);
	headerStream << Streamer::indent2 << wxString::Format("minZ=\"%.3lf\" ",  metricBoundbox.minZ) 
	
	                                  << wxString::Format("maxZ=\"%.3lf\"\n", metricBoundbox.maxZ);
	
	CncDoublePosition endPos;
	GBL_CONFIG->convertStepsToMetric(endPos, getCurrentEmulatorPosition());
	headerStream << Streamer::indent2 << wxString::Format("startPos=\"%s\" ", formatPosition(startPos)) 
	                                  << wxString::Format("endPos=\"%s\"\n",  formatPosition(endPos));
	
	headerStream << Streamer::indent2 << wxString::Format("distanceX=\"%s\"\n", formatPosition(metricBoundbox.distanceX));
	headerStream << Streamer::indent2 << wxString::Format("distanceY=\"%s\"\n", formatPosition(metricBoundbox.distanceY));
	headerStream << Streamer::indent2 << wxString::Format("distanceZ=\"%s\"\n", formatPosition(metricBoundbox.distanceZ));
	
	headerStream << Streamer::indent1 << "/>\n";
	
	footerStream << "<File/>\n";
}

