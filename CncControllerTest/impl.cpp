#include <iostream>
#include <cmath> 
#include "impl.h"
#include <wx/string.h>

#define PRINT_DEBUG_VALUE(vName, vValue)    std::cout << std::endl << "*** D1: " << vName << " = " << vValue << std::endl << std::endl;
#define ARDO_DEBUG_MESSAGE(a,b)             std::cout << std::endl << "*** D2: " << a     << " = " << b      << std::endl << std::endl;
#define ARDO_DEBUG_VALUE(a,b)               std::cout << std::endl << "*** D3: " << a     << " = " << b      << std::endl << std::endl;
#define ARDO_TRACE_STEPPER_DIR(a, b)        // to eliminate this within the arduino context
#define ARDO_TRACE_STEPPER_POS(a, b)        // to eliminate this within the arduino context

#include "C:\@Development\@Projekte\c++\CNCGuiController\Arduino\StepperEnvironment\ArdoVal.h"
#include "C:\@Development\@Projekte\c++\CNCGuiController\Arduino\StepperEnvironment\ArdoObj.h"
#include "C:\@Development\@Projekte\c++\CNCGuiController\Arduino\StepperEnvironment\CncAcmr.h"
#include "C:\@Development\@Projekte\c++\CNCGuiController\Arduino\StepperEnvironment\CncAcmr.ino"


#include "C:\@Development\@Projekte\c++\CNCGuiController\CncControlerGui\CncGamePad.h"

/////////////////////////////////////////////////////////////
unsigned long __sqrt(unsigned long x) {
/////////////////////////////////////////////////////////////
	register unsigned long xr;		// result register
	register unsigned long q2;		// scan-bit register
	register unsigned char f;		// flag (one bit)

	xr = 0;							// clear result
	q2 = 0x40000000L;				// higest possible result bit
	do {
		if ( (xr + q2) <= x ) {
		  x -= xr + q2;
		  f = 1;					// set flag
		}
		else {
		  f = 0;					// clear flag
		}
		xr >>= 1;
		
		if ( f ) 
		  xr += q2;					// test flag
		
	} while ( q2 >>= 2 );			// shift twice
	
	// xr + 1: add for rounding
	return xr < x ? xr + 1 : xr;
}

////////////////////////////////////////////////////
void Implementation::run() {
////////////////////////////////////////////////////
	std::cout << "Start . . ." << std::endl;
	
	class MyArduinoAccelManager : public ArduinoAccelManager {
		
		protected:
			virtual void notifyACMStateChange(State s) {}
			virtual void notifyACMInitMove()           {}
	};
	
	
	double A =   0.0;
	double B =   0.9;
	double C =  5.50;
	
	ArduinoAccelManager::Function fA(A, B, C);
	ArduinoAccelManager::Function fD(A, B, C);
	ArduinoAccelManager::Setup(0.03, 0.03, 0.03);
	
	MyArduinoAccelManager aam;
	aam.initialize(fA, fD);
	aam.initSpeed(66.0);
	aam.initMove(101);
	
	std::cout << "ArduinoAccelManager:" << std::endl;
	std::cout << aam << std::endl;
	
	
	//std::cout << GET_AxisSignatureIndex(ASG_XZ) << std::endl;
	//std::cout << AXIS_SIGNATURE_COUNT  << std::endl;
	
	return;
	
	if ( true ) {
		int32_t m = 300;
		aam.initMove(m);
		
		std::cout << "ArduinoAccelManager:" << std::endl;
		std::cout << aam << std::endl;
		
		for (int32_t i = 0; i < m; i++) {
			const uint32_t us = aam.initNextImpulse(ASGI_MASTER);
			std::cout << i << ": " << us << std::endl;
		}
	}
	
	
}
