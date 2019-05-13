#include <stdint.h>
#include "CncConfig.h"
#include "CncArduino.h"
#include "BinaryPathHandlerBase.h"

//------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////
BinaryPathHandlerBase::BinaryPathHandlerBase()
: PathHandlerBase()
, lineNumberTranslater()
, totalLineNumberOffset(0)
, lineNumberCounter(0)
////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////
BinaryPathHandlerBase::~BinaryPathHandlerBase() {
/////////////////////////////////////////////////////////////
	lineNumberTranslater.clear();
}
/////////////////////////////////////////////////////////////
void BinaryPathHandlerBase::translateLineNumber(unsigned long offset) {
/////////////////////////////////////////////////////////////
	lineNumberCounter++;
	
	totalLineNumberOffset += offset;
	lineNumberTranslater[lineNumberCounter] = lineNumberCounter + totalLineNumberOffset;
}
/////////////////////////////////////////////////////////////
void BinaryPathHandlerBase::resetLineNumberTranslation() {
/////////////////////////////////////////////////////////////
	lineNumberTranslater.clear();
	totalLineNumberOffset 	= 0;
	lineNumberCounter 		= 0;
}

//------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////
BinaryPathHandlerHexView::BinaryPathHandlerHexView(FormatType ft) 
: BinaryPathHandlerBase()
, formatType(ft)
/////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////
void BinaryPathHandlerHexView::prepareWork() {
/////////////////////////////////////////////////////////////
	hexContent.str("");
	resetLineNumberTranslation();
}
/////////////////////////////////////////////////////////////
void BinaryPathHandlerHexView::finishWork() {
/////////////////////////////////////////////////////////////
	// currently nothing to do
}
/////////////////////////////////////////////////////////////
bool BinaryPathHandlerHexView::processCommand(const unsigned char* buffer, int nbBytes) {
/////////////////////////////////////////////////////////////
	if ( nbBytes <= 0 || buffer == NULL)
		return true;
	
	switch ( formatType ) {
		case FormatType::Raw: 		hexContent << wxString::Format("%08d:\t", nbBytes);
									
									for (int i = 0; i < nbBytes; i++)
										hexContent << wxString::Format("0x%02X ",buffer[i]);
									
									hexContent<< std::endl;
									translateLineNumber(0);
									break;
									
		case FormatType::CStyle:	hexContent << "int size = " << wxString::Format("% 5d", nbBytes) << "; cnc->exec( ";
									
									for (int i = 0; i < nbBytes; i++)
										hexContent << wxString::Format("0x%02X ",buffer[i]);
											   
									hexContent << ");" << std::endl;
									translateLineNumber(0);
									break;
	}
	
	return true;
}

//------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////
BinaryPathHandlerHumanReadableView::BinaryPathHandlerHumanReadableView(FormatType ft) 
: BinaryPathHandlerBase()
, CncCommandDecoder::CallbackInterface()
, formatType(ft)
/////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////
void BinaryPathHandlerHumanReadableView::prepareWork() {
/////////////////////////////////////////////////////////////
	readableContent.str("");
	resetLineNumberTranslation();
}
/////////////////////////////////////////////////////////////
void BinaryPathHandlerHumanReadableView::finishWork() {
/////////////////////////////////////////////////////////////
	// currently nothing to do
}
/////////////////////////////////////////////////////////////
bool BinaryPathHandlerHumanReadableView::displaySteps(const unsigned char* buffer, int nbBytes) {
/////////////////////////////////////////////////////////////
	if ( nbBytes <= 0 || buffer == NULL )
		return true;

	const unsigned char cmd = buffer[0];
	switch ( cmd ) {
		case CMD_RENDER_AND_MOVE:
		case CMD_MOVE:				{
										int32_t x=0, y=0, z=0;
										CncCommandDecoder::decodeMove(buffer, nbBytes, x, y, z);
										readableContent << wxString::Format("   cnc->exec('%c', %+ 10ld, %+ 10ld, %+ 8ld ); //%s", cmd, x, y, z, ArduinoCMDs::getCMDLabel(cmd));
										break;
									}
	}
	
	readableContent << std::endl;
	translateLineNumber(0);
	
	return true;
}
/////////////////////////////////////////////////////////////
bool BinaryPathHandlerHumanReadableView::displayMetric(const unsigned char* buffer, int nbBytes) {
/////////////////////////////////////////////////////////////
	if ( nbBytes <= 0 || buffer == NULL )
		return true;

	const unsigned char cmd = buffer[0];
	switch ( cmd ) {
		case CMD_RENDER_AND_MOVE:
		case CMD_MOVE:				{
										const double factX = GBL_CONFIG->getDisplayFactX();
										const double factY = GBL_CONFIG->getDisplayFactY();
										const double factZ = GBL_CONFIG->getDisplayFactZ();
										
										int32_t x=0, y=0, z=0;
										CncCommandDecoder::decodeMove(buffer, nbBytes, x, y, z);
										readableContent << wxString::Format("   cnc->exec('%c', %+ 10.3lf, %+ 10.3lf, %+ 8.3lf ); //%s", 
										                                     cmd, 
																			 x * factX, 
																			 y * factY, 
																			 z * factZ, 
																			 ArduinoCMDs::getCMDLabel(cmd));
										break;
									}
	}
	
	readableContent << std::endl;
	translateLineNumber(0);
	
	return true;
}
/////////////////////////////////////////////////////////////
bool BinaryPathHandlerHumanReadableView::displayMoveSequence(const unsigned char* buffer, int nbBytes) {
/////////////////////////////////////////////////////////////
	CncCommandDecoder::MoveSequenceInfo seqInfo;
	seqInfo.In.parseAllPortions = true;
	
	// this call will activate: notifyMove(int32_t dx, int32_t dy, int32_t dz, int32_t f)
	if ( CncCommandDecoder::decodeMoveSequence(buffer, nbBytes, seqInfo, this) == false )
		return false;
		
	return true;
}
/////////////////////////////////////////////////////////////
bool BinaryPathHandlerHumanReadableView::displaySetter(const unsigned char* buffer, int nbBytes) {
/////////////////////////////////////////////////////////////
	CncCommandDecoder::SetterInfo si;
	if ( CncCommandDecoder::decodeSetter(buffer, nbBytes, si) == false )
		return false;
		
	readableContent << wxString::Format("\n//%s\ncnc->exec('S', '%u', ", ArduinoPIDs::getPIDLabel(si.pid), si.pid);
	
	for ( auto it = si.values.begin(); it != si.values.end(); ++it ) {
		const int32_t& v = *it;
		readableContent << v << " ";
	}
	readableContent << " );";
	readableContent << std::endl;
	
	// offset: 2 = additional \n count
	translateLineNumber(2);
	return true;
}
/////////////////////////////////////////////////////////////
bool BinaryPathHandlerHumanReadableView::processCommand(const unsigned char* buffer, int nbBytes) {
/////////////////////////////////////////////////////////////
	if ( nbBytes <= 0 || buffer == NULL )
		return true;
	
	bool ret = false;
	
	const unsigned char cmd = buffer[0];
	switch ( cmd ) {
		//--------------------------------------------------
		case CMD_SETTER:
		{	
			ret = displaySetter(buffer, nbBytes);
			break;
		}
		//--------------------------------------------------
		case CMD_RENDER_AND_MOVE:
		case CMD_MOVE:
		{
			switch ( formatType ) {
				case Steps:		ret = displaySteps(buffer,  nbBytes); 	break;
				case Metric:	ret = displayMetric(buffer, nbBytes);	break;
			}
			
			break;
		}
		//--------------------------------------------------
		case CMD_MOVE_SEQUENCE:
		case CMD_RENDER_AND_MOVE_SEQUENCE:
		{
			ret = displayMoveSequence(buffer, nbBytes);
			break;
		}
		//--------------------------------------------------
		default:
		{ 
			readableContent << "No encoder specified for command: '" << cmd << "'" << std::endl;
			translateLineNumber(0);
		}
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////
void BinaryPathHandlerHumanReadableView::notifyMove(int32_t dx, int32_t dy, int32_t dz, int32_t f) {
/////////////////////////////////////////////////////////////
	const double factX = GBL_CONFIG->getDisplayFactX();
	const double factY = GBL_CONFIG->getDisplayFactY();
	const double factZ = GBL_CONFIG->getDisplayFactZ();

	switch ( formatType ) {
		case Steps:		readableContent << wxString::Format("    cnc->seq->exec(%+ 10ld, %+ 10ld, %+ 8ld ); // Sequence Move\n", dx, dy, dz);
						break;
						
		case Metric:	readableContent << wxString::Format("    cnc->seq->exec(%+ 10.3lf, %+ 10.3lf, %+ 8.3lf ); // Sequence Move\n", dx * factX, dy * factY, dz * factZ);
						break;
	}
	
	translateLineNumber(1);
}
/////////////////////////////////////////////////////////////
void BinaryPathHandlerHumanReadableView::notifyMoveSequenceBegin(const CncCommandDecoder::MoveSequenceInfo& sequence) {
/////////////////////////////////////////////////////////////
	readableContent << "   cnc->nextMoveSequcence();\n";
	readableContent << "   {\n";
	translateLineNumber(2);
}
/////////////////////////////////////////////////////////////
void BinaryPathHandlerHumanReadableView::notifyMoveSequenceNext(const CncCommandDecoder::MoveSequenceInfo& sequence) {
/////////////////////////////////////////////////////////////
	readableContent << "   }\n";
	readableContent << "   {\n";
	translateLineNumber(2);
}
/////////////////////////////////////////////////////////////
void BinaryPathHandlerHumanReadableView::notifyMoveSequenceEnd(const CncCommandDecoder::MoveSequenceInfo& sequence) {
/////////////////////////////////////////////////////////////
	readableContent << "   }\n";
	translateLineNumber(1);
}
