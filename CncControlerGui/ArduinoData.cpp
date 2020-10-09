#include <cstring>
#include <wx/debug.h>
#include <wx/string.h>
#include "SerialThread.h"
#include "ArduinoData.h"

////////////////////////////////////////////////////////////////////
AE::TransferData::TransferData()
////////////////////////////////////////////////////////////////////
{
	for (auto i=minPinIndex; i<=maxPinIndex; i++) {
		PinData& p = pins[i];

		p.type  = '\0';
		p.name  =   -1;
		p.name  =   -1;
		p.mode  = '\0';
		p.value	=   -1;
	}
}
////////////////////////////////////////////////////////////////////
AE::TransferData::~TransferData() {
////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////
bool AE::TransferData::isSomethingChanged(const TransferData& ref) {
////////////////////////////////////////////////////////////////////
	return std::memcmp(this, &(ref), sizeof(TransferData)) != 0;
}




////////////////////////////////////////////////////////////////////
AE::ArduinoData::ArduinoData() 
: pins			()
, exterConfig	()
, traceInfo		()
////////////////////////////////////////////////////////////////////
{
	unsigned int counter = 0;
	for ( int i = minAPinIndex; i <= maxAPinIndex; i++) 
		pins[(AE::PinName)i] = PinData('A', counter++, (AE::PinName)i);
	
	for ( int i = minDPinIndex; i <= maxDPinIndex; i++)
		pins[(AE::PinName)i] = PinData('D', (AE::PinName)i, (AE::PinName)i);
}
////////////////////////////////////////////////////////////////////
AE::ArduinoData::~ArduinoData() {
////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////
void AE::ArduinoData::pinMode(AE::PinName pin, AE::PinMode pm) {
////////////////////////////////////////////////////////////////////
	char mode = '\0';
	
	switch ( pm ) {
		case PM_INPUT:			mode = 'I'; break;
		case PM_OUTPUT:			mode = 'O'; break;
		case PM_INPUT_PULLUP:	mode = 'P'; break; 
	}
	
	auto it = pins.find(pin);
	if ( it != pins.end() )
		it->second.mode = mode;
}
////////////////////////////////////////////////////////////////////
void AE::ArduinoData::digitalWrite(unsigned int pin, bool pl) {
////////////////////////////////////////////////////////////////////
	if ( isPin(pin ) == false )
		return;

	PinLevel x = pl ? PL_HIGH : PL_LOW;
	digitalWrite((PinName)pin, x);
}
////////////////////////////////////////////////////////////////////
void AE::ArduinoData::digitalWrite(AE::PinName pin, AE::PinLevel pl) {
////////////////////////////////////////////////////////////////////
	auto it = pins.find(pin);
	if ( it != pins.end() )
		it->second.value = pl == PL_LOW ? 0 : 1;
}
////////////////////////////////////////////////////////////////////
AE::PinLevel AE::ArduinoData::digitalRead(AE::PinName pin) {
////////////////////////////////////////////////////////////////////
	auto it = pins.find(pin);
	if ( it != pins.end() )
		return it->second.value == 0 ? PL_LOW : PL_HIGH;

	return PL_LOW;
}
////////////////////////////////////////////////////////////////////
uint16_t AE::ArduinoData::analogRead(PinName pin) {
////////////////////////////////////////////////////////////////////
	if ( isAnalogPin(pin) == true ) {
		auto it = pins.find(pin);
		if ( it != pins.end() ) { 
			const int val = it->second.value;
			return val >=0 && val <=1023 ? val : 0;
		}
	}
	
	return 0;
}
////////////////////////////////////////////////////////////////////
void AE::ArduinoData::analogWrite(PinName pin, int value) {
////////////////////////////////////////////////////////////////////
	if ( isAnalogPin(pin) == true ) {
		auto it = pins.find(pin);
		if ( it != pins.end() )
			it->second.value = value;
	}
}
////////////////////////////////////////////////////////////////////
void AE::ArduinoData::fillTransferData(TransferData& td) {
////////////////////////////////////////////////////////////////////
	unsigned int counter = 0;
	for ( auto it = pins.begin(); it != pins.end(); ++it ) {
		td.pins[counter++] = it->second;
	}
	
	td.stepperDirX = traceInfo.stepperDirX;
	td.stepperDirY = traceInfo.stepperDirY;
	td.stepperDirZ = traceInfo.stepperDirZ;
	
	td.stepperPosX = traceInfo.stepperPosX;
	td.stepperPosY = traceInfo.stepperPosY;
	td.stepperPosZ = traceInfo.stepperPosZ;
}
////////////////////////////////////////////////////////////////////
bool AE::ArduinoData::isPin(unsigned int pin) {
////////////////////////////////////////////////////////////////////
	return ( pin >= MinPinNameValue && pin <= MaxPinNameValue );
}
////////////////////////////////////////////////////////////////////
bool AE::ArduinoData::isAnalogPin(AE::PinName pin) {
////////////////////////////////////////////////////////////////////
	switch ( pin ) {
		case PN_A0:
		case PN_A1:
		case PN_A2:
		case PN_A3:
		case PN_A4:
		case PN_A5:	return true;
		
		#if defined(ARDUINO_AVR_MEGA2560)
			case PN_A6:
			case PN_A7:
			case PN_A8:
			case PN_A9:
			case PN_A10:
			case PN_A11:
			case PN_A12:
			case PN_A13:
			case PN_A14:
			case PN_A15:	return true;
		#endif
		
		default:	;
	}

	return false;
}
////////////////////////////////////////////////////////////////////
bool AE::ArduinoData::isDigitalPin(AE::PinName pin) {
////////////////////////////////////////////////////////////////////
	if ( isPin(pin) == false )
		return false;
		
	return isAnalogPin(pin) == false;
}
////////////////////////////////////////////////////////////////////
AE::PinLevel AE::ArduinoData::convertPinLevel(bool state) {
////////////////////////////////////////////////////////////////////
	return state == false ? PL_LOW : PL_HIGH;
}
////////////////////////////////////////////////////////////////////
AE::PinName	AE::ArduinoData::convertPinName(unsigned char pin) {
////////////////////////////////////////////////////////////////////
	if ( isPin(pin) == false )
		return PN_NULL;

	return (PinName)pin;
}
////////////////////////////////////////////////////////////////////
AE::PinName AE::ArduinoData::convertPinName(const char type, int name) {
////////////////////////////////////////////////////////////////////
	if ( type != 'D' && type != 'A' )
		return PN_NULL;
		
	if ( type == 'D' ) 
		return name >= minDPinIndex && name <= maxDPinIndex ? (PinName)name : PN_NULL;
	
	const int idx = name + minAPinIndex;
	return idx >= minAPinIndex && idx <= maxAPinIndex ? (PinName)idx : PN_NULL;
}
////////////////////////////////////////////////////////////////////
wxString AE::ArduinoData::buildDislpayName(PinName pin) {
////////////////////////////////////////////////////////////////////
	if ( isPin(pin) == false )
		return "";
		
	if ( isAnalogPin(pin) ) return buildDislpayName('A', ((int)pin) - minDPinIndex );
	else 					return buildDislpayName('D', (int)pin);
	
	return "";
}
////////////////////////////////////////////////////////////////////
wxString AE::ArduinoData::buildDislpayName(const char type, int name) {
////////////////////////////////////////////////////////////////////
	if ( type != 'A' && type != 'D' )
		return "";
	
	if ( type == 'A' && ( name < minAPinIndex - minAPinIndex || name > maxAPinIndex - minAPinIndex) )
		return "";

	if ( type == 'D' && ( name < minDPinIndex || name > maxDPinIndex ) )
		return "";
		
	return wxString::Format("%c%02d", type, name);
}





