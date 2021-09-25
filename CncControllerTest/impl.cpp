#include <iostream>
#include <cmath> 
#include <algorithm>
#include <numeric>
#include <iostream>
#include <utility>
#include <vector>

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

template<> bool Foo<PESIMISTIC>::f()
{
  std::cout << "Foo<false>::f()\n";
  return PESIMISTIC;
}

template<> bool Foo<OPTIMISTIC>::f()
{
  std::cout << "Foo<true>::f()\n";
  return OPTIMISTIC;
}

class MyInt{
	
	public:
		MyInt(int i_)
		:i(i_)
		{
			for ( unsigned int k = 0; k< 10; k++)
				values[k] = 0.1 * i;
		}
		
		// copy semantic
		MyInt(const MyInt&)= delete;
		MyInt& operator= (const MyInt&)= delete;
		
		// move semantic
		MyInt(MyInt&&)= default;
		MyInt& operator= (MyInt&&)= default;
		
		int getVal() const {
			return i;
		}
		
		double getDouble(unsigned int k) const {
			return values[k];
		}
		
	private:
		int i;
		
		double values[10];
};

class string
{
    // ...
	public:
		// copy constructor
		string(const string& rhs);
		// move constructor
		string(string&& rhs) noexcept;
		// copy assignment operator
		auto operator=(const string& rhs) & -> string&;
		// move assignment operator
		auto operator=(string&& rhs) & noexcept -> string&;
};

//#include <sdkddkver.h>

//#include <stdio.h>
//#include <tchar.h>
//#include <atlbase.h>
//#include <Windows.h>
#include <sapi.h>

bool xxx() 
{
	
	
		
	
		
	
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
	
	HKEY hKey;
	
	TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD    cbName;                   // size of name string 
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 

    TCHAR achValue[MAX_VALUE_NAME]; 
    DWORD cchValue = MAX_VALUE_NAME; 

	DWORD i, test;
	DWORD dwType = REG_SZ;
	DWORD dwSize = MAX_PATH; 
	char value[MAX_PATH];

		if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Enum\\USB"),0, KEY_READ, &hKey) == ERROR_SUCCESS)	
		{
			RegQueryInfoKey(
				hKey,                    // key handle 
				achClass,                // buffer for class name 
				&cchClassName,           // size of class string 
				NULL,                    // reserved 
				&cSubKeys,               // number of subkeys 
				&cbMaxSubKey,            // longest subkey size 
				&cchMaxClass,            // longest class string 
				&cValues,                // number of values for this key 
				&cchMaxValue,            // longest value name 
				&cbMaxValueData,         // longest value data 
				&cbSecurityDescriptor,   // security descriptor 
				&ftLastWriteTime);       // last write time 
			

			for	(i=0; i<cSubKeys; i++)	//enumerate subkeys until end
			{
				cbName = MAX_KEY_LENGTH;
				
					wchar_t buf[255];
					DWORD dwBufSize = sizeof(buf);
					if(RegQueryValueExW(hKey,L"MachineGuid",0, 0, (BYTE*)buf, &dwBufSize) == ERROR_SUCCESS)
						{
							std::cout << L"Registry GUID value: " << buf << std::endl;
						}
		
					if (RegEnumKeyEx(hKey, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime)== ERROR_SUCCESS)
					{
						std::cout << cbName << std::endl; //hir werden mir teilweise andere werte angezeigt 
	
						/*
								***doesn´t work***
						
						if (RegQueryValueEx(hKey, "DisplayName", NULL, &dwType, (LPBYTE)value, &dwSize)==ERROR_SUCCESS)
							{
								cout << "success" << endl;
							}

						else 
							{ 
								cout << "RegQueryValueEx failed" << endl;
							}														
							
								***doesn´t work***
							*/
					}

					else
					{
						std::cout <<"RegEnumKeyEx failed" << std::endl;
					}
			}
									
		}
		
		else
		{
			std::cout << "RegOpenKeyEx failed";
		}
	
	RegCloseKey(hKey);
	
}


////////////////////////////////////////////////////
void Implementation::run() {
////////////////////////////////////////////////////
	std::cout << "Start . . ." << std::endl;
	
	
	
	
	xxx();
	
	
	
	
	
	
	
	return;
	
	
	#define STR_HELPER(x) #x
	#define STR(x) STR_HELPER(x)

	#define VERSION_MAJOR_NUM 0
	#define VERSION_MINOR_NUM 8
	#define VERSION_INDEX_NUM 9
	#define VERSION_STRING "" STR(VERSION_MAJOR_NUM) "." STR(VERSION_MINOR_NUM) "." STR(VERSION_INDEX_NUM) "\0"

	#define VER_FILEVERSION             VERSION_MAJOR_NUM,VERSION_MINOR_NUM,VERSION_INDEX_NUM,0
	#define VER_FILEVERSION_STR         VERSION_STRING 
	
	
	std::cout <<  wxString::FormatVER_FILEVERSION << std::endl;
	
	return;
	
    ISpVoice * pVoice = NULL;

    if (FAILED(::CoInitialize(NULL)))
        return;

    HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
    if( SUCCEEDED( hr ) )
    {
        hr = pVoice->Speak(L"Geschwindigkeit 1500 Millimeter pro Minute. fastest ", 0, NULL);
        pVoice->Release();
        pVoice = NULL;
    }

    ::CoUninitialize();
	
	
	return;
	
	
	MyInt a(5);
	MyInt b(10);

	b = std::move(a);
	
	std::cout << a.getVal() <<", "<< b.getVal() << std::endl; 
	
	return;
	
	
	
    std::vector<MyInt> vecMyInt;
    
	for (auto i= 1; i <= 10; ++i) {
        vecMyInt.push_back(std::move(MyInt(i)));
    }
    
    std::for_each(vecMyInt.begin(), vecMyInt.end(), [](MyInt& myInt)
	{ 
		std::cout << myInt.getVal() << " ["; 
		for ( unsigned int k = 0; k< 10; k++)
			std::cout << myInt.getDouble(k) << " "; 
			
		std::cout << "]\n";
	});
    
    std::cout << std::endl;
	
	for ( auto it = vecMyInt.begin(); it != vecMyInt.end(); ++it ) {
		
	}
    
    //auto myInt= MyInt(std::accumulate(vecMyInt.begin(), vecMyInt.end(),MyInt(1),[](MyInt& f, MyInt& s){ return f.getVal() * s.getVal(); }));
    
    //std::cout << "myInt.getVal(): " << myInt.getVal() << std::endl;
    
    std::cout << std::endl;

	
	return;
	
	Foo<true> foo;
	foo.f();
	
	
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
