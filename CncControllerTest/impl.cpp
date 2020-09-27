#include <iostream>
#include <cmath> 
#include "impl.h"
#include <wx/string.h>
#include <wx/datetime.h>

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
/*
virtual bool readMinLmtPin()                                  { return READ_LMT_PIN_X_MIN; }  
    virtual bool readMaxLmtPin()                                  { return READ_LMT_PIN_X_MAX; }  
    virtual bool readLmtPins()                                    { return READ_LMT_PIN_X;     }  
    virtual void writeDirPin(bool value)                          { WRITE_DIR_PIN_X            }
    virtual void writeStpPin(bool value)                          { WRITE_STP_PIN_X            }  

  public:
    CncAxisX(const StepperSetup& ss) 
    : CncArduinoStepper(ss)                                       {}
    virtual ~CncAxisX()                                           {}
    virtual char              getAxisId()   
*/
//template <bool o, typename T> 

class Base {
	
	typedef bool 			(*readPin_funct) 	(void);
	typedef void 			(*writePin_funct) 	(void);
	
	protected:
		
		char axis;
		
		readPin_funct	readLmtPins;
		readPin_funct	readMinLmtPin;
		readPin_funct	readMaxLmtPin;
		writePin_funct	writeDirPin;
		writePin_funct	writeStpPin;
		
	protected:
		Base(
			char 			aId, 
			readPin_funct 	fMinMax,
			readPin_funct 	fMin,
			readPin_funct 	fMax
		)
		: axis				(aId)
		, readLmtPins		(fMinMax)
		, readMinLmtPin		(fMin)
		, readMaxLmtPin		(fMax)
		{}
		
	public:
		
		void test () {
			std::cout << readLmtPins() << std::endl;
			std::cout << readMinLmtPin() << std::endl;
			std::cout << readMaxLmtPin() << std::endl;
		}
};

#undef READ_LMT_PIN_X_MIN
#undef READ_LMT_PIN_X_MAX
#undef READ_LMT_PIN_X

#define READ_LMT_PIN_X_MIN	true
#define READ_LMT_PIN_X_MAX	true
#define READ_LMT_PIN_X		false

class X : public Base {

	public:
		static bool readLmtPins()       { return READ_LMT_PIN_X;     }  
		static bool readMinLmtPin()     { return READ_LMT_PIN_X_MIN; }  
		static bool readMaxLmtPin()     { return READ_LMT_PIN_X_MAX; }  
		
		X() 
		: Base(
			'X',
			&X::readLmtPins,
			&X::readMinLmtPin,
			&X::readMaxLmtPin
		)
		{}
		
	
};

////////////////////////////////////////////////////
void Implementation::run() {
////////////////////////////////////////////////////
	std::cout << "Start . . ." << std::endl;
	
	
	std::cout << wxDateTime::Now().Format("%Y-%m-%d.%H-%M-%S").c_str() << std::endl;
	
	return;
	
	X x;
	x.test();
	
	return;
	
	
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
