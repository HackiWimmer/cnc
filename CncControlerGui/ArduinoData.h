#ifndef ARDUINO_DATA_H
#define ARDUINO_DATA_H

#include <map>
#include <wx/string.h>
#include "ArduinoEnvWrapper.h"

namespace AE {
	
	static const int minDPinIndex 	= MIN_DPIN;
	static const int maxDPinIndex	= MAX_DPIN;
	
	static const int minAPinIndex 	= MIN_APIN;
	static const int maxAPinIndex 	= MAX_APIN;
	
	static const int minPinIndex 	= minDPinIndex;
	static const int maxPinIndex 	= maxAPinIndex;

	static const int maxPins 		= maxPinIndex + 1;
	
	// -----------------------------------------------------
	struct PinData {
		char 	type  	= '\0';
		int 	name 	=   -1;
		int 	uidx 	=   -1;
		char 	mode  	= '\0';
		int 	value 	=   -1;

		PinData()
		: type('\0')
		, name(-1)
		, uidx(-1)
		, mode('\0')
		, value(-1)
		{}

		PinData(char t, int n, int i, char m='\0', int v=-1)
		: type(t)
		, name(n)
		, uidx(i)
		, mode(m)
		, value(v)
		{}
		
		bool operator == (const PinData& val) const {
			return uidx == val.uidx;
		}
	};

	// -----------------------------------------------------
	struct TransferData {
		
		PinData pins[maxPins];
		
		int32_t		stepperDirX		= DIRECTION_UNKNOWN;
		int32_t		stepperDirY		= DIRECTION_UNKNOWN;
		int32_t		stepperDirZ		= DIRECTION_UNKNOWN;
		int32_t		stepperDirH		= DIRECTION_UNKNOWN;
		
		int32_t		stepperPosX		= 0;
		int32_t		stepperPosY		= 0;
		int32_t		stepperPosZ		= 0;
		int32_t		stepperPosH		= 0;
		
		int32_t		cfgSpeed_MM_SEC	= 0;
		int32_t		msdSpeed_MM_SEC	= 0;
		
		TransferData();
		~TransferData();
		
		bool isSomethingChanged(const TransferData& ref);
	};
	
	// stores all configuraion values
	struct ExtraConfiguration {
		bool traceGetters		= false;
		bool traceSetters		= false;
	};
	
	// stores all configuraion values
	struct TraceInformation {
		int32_t		stepperDirX		= DIRECTION_UNKNOWN;
		int32_t		stepperDirY		= DIRECTION_UNKNOWN;
		int32_t		stepperDirZ		= DIRECTION_UNKNOWN;
		int32_t		stepperDirH		= DIRECTION_UNKNOWN;
		
		int32_t		stepperPosX		= 0;
		int32_t		stepperPosY		= 0;
		int32_t		stepperPosZ		= 0;
		int32_t		stepperPosH		= 0;
		
		int32_t		cfgSpeed_MM_SEC	= 0;
		int32_t		msdSpeed_MM_SEC	= 0;
	};
	
	struct ArduinoData {
		
		typedef std::map<PinName, PinData> 	PinMap;
		
		PinMap  				pins;
		ExtraConfiguration		exterConfig;
		TraceInformation		traceInfo;
		
		void fillTransferData(TransferData& td);
		
		//------------------------------------------------------------
		// initialize setup
		ArduinoData();
		~ArduinoData();
		
		void 				pinMode(PinName pin, PinMode pm);
		void 				digitalWrite(unsigned int pin, bool pl);
		void 				digitalWrite(PinName pin, PinLevel pl);
		PinLevel 			digitalRead(PinName pin);

		uint16_t			analogRead(PinName pin);
		void				analogWrite(PinName pin, int value);

		static bool 		isPin(unsigned int pin);
		static bool 		isDigitalPin(PinName pin);
		static bool 		isAnalogPin(PinName pin);

		static PinLevel 	convertPinLevel(bool state);
		static PinName 		convertPinName(unsigned char pin);
		static PinName		convertPinName(const char type, int name);
		static wxString		buildDisplayName(PinName pin);
		static wxString		buildDisplayName(const char type, int name);
	};
	
};

#endif
