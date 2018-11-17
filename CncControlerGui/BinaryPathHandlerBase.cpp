#include <stdint.h>
#include "CncConfig.h"
#include "CncArduino.h"
#include "CncCommandDecoder.h"
#include "BinaryPathHandlerBase.h"

//------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////
BinaryPathHandlerBase::BinaryPathHandlerBase()
: PathHandlerBase()
////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////
BinaryPathHandlerBase::~BinaryPathHandlerBase() {
/////////////////////////////////////////////////////////////
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
									break;
									
		case FormatType::CStyle:	hexContent << "int size = " << wxString::Format("% 3d", nbBytes) << "; m_cncController->execute( ";
									
									for (int i = 0; i < nbBytes; i++)
										hexContent << wxString::Format("0x%02X ",buffer[i]);
											   
									hexContent << ");" << std::endl;
									break;
	}
	
	return true;
}

//------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////
BinaryPathHandlerHumanReadableView::BinaryPathHandlerHumanReadableView(FormatType ft) 
: BinaryPathHandlerBase()
, formatType(ft)
/////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////
void BinaryPathHandlerHumanReadableView::prepareWork() {
/////////////////////////////////////////////////////////////
	readableContent.str("");
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

	unsigned char cmd = buffer[0];
	switch ( cmd ) {
		case CMD_RENDER_AND_MOVE:
		case CMD_MOVE:				{
										int32_t x=0, y=0, z=0;
										CncCommandDecoder::decodeMove(buffer, nbBytes, x, y, z);
										readableContent << wxString::Format("cnc->exec('%c', %+ 10ld, %+ 10ld, %+ 8ld ); //%s", cmd, x, y, z, ArduinoCMDs::getCMDLabel(cmd));
										break;
									}
	}
	
	readableContent << std::endl;
	return true;
}
/////////////////////////////////////////////////////////////
bool BinaryPathHandlerHumanReadableView::displayMetric(const unsigned char* buffer, int nbBytes) {
/////////////////////////////////////////////////////////////
	if ( nbBytes <= 0 || buffer == NULL )
		return true;

	unsigned char cmd = buffer[0];
	switch ( cmd ) {
		case CMD_RENDER_AND_MOVE:
		case CMD_MOVE:				{
										const double factX = GBL_CONFIG->getDisplayFactX();
										const double factY = GBL_CONFIG->getDisplayFactY();
										const double factZ = GBL_CONFIG->getDisplayFactZ();
										
										int32_t x=0, y=0, z=0;
										CncCommandDecoder::decodeMove(buffer, nbBytes, x, y, z);
										readableContent << wxString::Format("cnc->exec('%c', %+ 10.3lf, %+ 10.3lf, %+ 8.3lf ); //%s", 
										                                     cmd, 
																			 x * factX, 
																			 y * factY, 
																			 z * factZ, 
																			 ArduinoCMDs::getCMDLabel(cmd));
										break;
									}
	}
	
	readableContent << std::endl;
	return true;
}
/////////////////////////////////////////////////////////////
bool BinaryPathHandlerHumanReadableView::displaySetter(const unsigned char* buffer, int nbBytes) {
/////////////////////////////////////////////////////////////
	CncCommandDecoder::SetterInfo si;
	if ( CncCommandDecoder::decodeSetter(buffer, nbBytes, si) == false )
		return false;
		
	readableContent << wxString::Format("cnc->exec('S', '%u', ", si.pid);
	
	for ( auto it = si.values.begin(); it != si.values.end(); ++it ) {
		const int32_t v = *it;
		readableContent << v << " ";
	}
	readableContent << wxString::Format(" ); // %s", ArduinoPIDs::getPIDLabel(si.pid));
	readableContent << std::endl;
	
	return true;
}
/////////////////////////////////////////////////////////////
bool BinaryPathHandlerHumanReadableView::processCommand(const unsigned char* buffer, int nbBytes) {
/////////////////////////////////////////////////////////////
	if ( nbBytes <= 0 || buffer == NULL )
		return true;
	
	bool ret = false;
	
	unsigned char cmd = buffer[0];
	switch ( cmd ) {
		case CMD_SETTER:			ret = displaySetter(buffer, nbBytes);
									break;
		
		case CMD_RENDER_AND_MOVE:
		case CMD_MOVE:				{
										switch ( formatType ) {
											case Steps:		ret = displaySteps(buffer,  nbBytes); 	break;
											case Metric:	ret = displayMetric(buffer, nbBytes);	break;
										}
										
										break;
									}
									
		default: 					readableContent << "No encoder specified for: " << cmd;
	}
	
	return ret;
}

