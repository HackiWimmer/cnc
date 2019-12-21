#include <cstdint>
#include <iostream>
#include "SerialCircularBuffer.h"

///////////////////////////////////////////////////////////////////
SerialCircularBuffer::SerialCircularBuffer()
: circularBuffer(10 * 1024)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
SerialCircularBuffer::~SerialCircularBuffer() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
int SerialCircularBuffer::peak() {
///////////////////////////////////////////////////////////////////
	byte ret = 0;
	if ( circularBuffer.peak(ret) == false )
		return -1;
		
	return (int)ret;
}
///////////////////////////////////////////////////////////////////
int SerialCircularBuffer::read() {
///////////////////////////////////////////////////////////////////
	byte ret = 0;
	if ( circularBuffer.read(ret) == false )
		return -1;
		
	return (int)ret;
}
///////////////////////////////////////////////////////////////////
int SerialCircularBuffer::read(byte* buffer, int size) {
///////////////////////////////////////////////////////////////////
	return circularBuffer.read(buffer, size);
}
///////////////////////////////////////////////////////////////////
int SerialCircularBuffer::write(byte b) {
///////////////////////////////////////////////////////////////////
	return circularBuffer.write(b) ? 1 : 0;
}
///////////////////////////////////////////////////////////////////
bool SerialCircularBuffer::write(byte* buffer, int size) {
///////////////////////////////////////////////////////////////////
	return circularBuffer.write(buffer, size) > 0;
}


///////////////////////////////////////////////////////////////////
//
// only for testing
//
namespace SerialCircularBuffer_TestEnv {
	
	class ControllerThread;
	ControllerThread*		gThread 	= NULL;
	bool 					gRun		= true;
	SerialCircularBuffer* 	gScb 		= NULL;
	
	class ControllerThread : public wxThread {
		
		public:
			SerialCircularBuffer* 	scb;
			int 					testId;
			
			ControllerThread(int tid) 
			: scb 		( new SerialCircularBuffer() )
			, testId	(tid)
			{
				gScb = scb;
			}
			~ControllerThread()
			{
				gScb = NULL;
				delete scb;
			}
			
			//----------------------------------------------------
			virtual ExitCode Entry() {
				
				switch ( testId ) {
					case 1: testW1(); 	break;
					case 2: testW2(); 	break;
				}
				
				return 0;
			}
			
			
			// ----------------------------------------------------
			void testW1() {
				int c = 1;
				while ( gRun ) {
					
					// write more if the Serial buffer was empty
					if ( scb && scb->size() == 0 ) {
						
						int32_t val = (42 * (c*=2) ) % 1000000 ;
						unsigned char buffer[sizeof(int32_t)];
						memcpy(buffer, &val, sizeof(int32_t));
						
						scb->write(buffer, sizeof(int32_t));
					}
					
					this->Sleep(100);
				}
			}
			
			// ----------------------------------------------------
			void testW2() {

				while ( gRun ) {
					
					// write always
					if ( scb ) {
						
						for ( auto i = 0; i < 3; i++ ) {
							int32_t val = (42 * i) % 1000000 ;
							unsigned char buffer[sizeof(int32_t)];
							memcpy(buffer, &val, sizeof(int32_t));
							
							scb->write(buffer, sizeof(int32_t));
						}
						
					}
					
					this->Sleep(1);
				}
			}
	};
};

using namespace SerialCircularBuffer_TestEnv;
#include <wx/app.h>
#include <wx/string.h>
#include <wx/datetime.h>
#define prefix1(ts)  wxString::Format("%s.%03d: %s: ", ts.FormatISOTime(), ts.GetMillisecond(),  __PRETTY_FUNCTION__) 
#define prefix2(ts)  wxString::Format("%s.%03d: %s  ", ts.FormatISOTime(), ts.GetMillisecond(), "   ") 

///////////////////////////////////////////////////////////////////
void SerialCircularBuffer::test(int testId) { 
///////////////////////////////////////////////////////////////////
	auto init = [&](int testId ) {
		
		gThread = new ControllerThread(testId);
		gThread->Run();

	};
	
	gRun = true;
	
		std::cout << wxString::Format("SerialCircularBuffer::test(%d)", testId) << std::endl << std::flush;
		switch ( testId ) {
			case 1:		init(1); 	SerialCircularBuffer::testR1(); 	break;
			case 2:		init(2); 	SerialCircularBuffer::testR2(); 	break;
			default: 	std::cerr << __PRETTY_FUNCTION__ << ": Invalid test id: " << testId << std::endl;
		}
		
	gRun = false;
}

///////////////////////////////////////////////////////////////////
void SerialCircularBuffer::testR1() { 
///////////////////////////////////////////////////////////////////
	std::cout << prefix1(wxDateTime::UNow()) << "Start . . . " << std::endl << std::flush;
	
		if ( gThread->IsRunning() == false )
			return;
	
		std::cout << prefix2(wxDateTime::UNow()) << "Thread is running . . . " << std::endl << std::flush;
		wxTheApp->Yield();
		
		auto cnt = 0;
		while ( gScb ) {
			
			const int a = gScb->size();
			if ( a >= 4  ) {
				
				unsigned char buffer[sizeof(int32_t)];
				int r = gScb->read(buffer, sizeof(int32_t));
				
				int32_t val = 0;
				memcpy(&val, buffer, sizeof(int32_t));
				std::cout << prefix2(wxDateTime::UNow()) << " avl = " << wxString::Format("%05d", a)  << "; cnt = " << wxString::Format("%04d", cnt) << "; read = " << r << "; val = " << val << std::endl;
				
				cnt++;
			}
			
			wxMilliSleep(1);
			
			if ( cnt > 100 )
				break;
			
			if ( cnt%16 == 0 )
				wxTheApp->Yield();
		}
	
	std::cout << prefix1(wxDateTime::UNow()) << "End . . ." << std::endl;
}

///////////////////////////////////////////////////////////////////
void SerialCircularBuffer::testR2() { 
///////////////////////////////////////////////////////////////////
	std::cout << prefix1(wxDateTime::UNow()) << "Start . . . " << std::endl << std::flush;
	
		if ( gThread->IsRunning() == false )
			return;
	
		std::cout << prefix2(wxDateTime::UNow()) << "Thread is running . . . " << std::endl << std::flush;
		wxTheApp->Yield();
		
		auto cnt = 0;
		unsigned char buffer[sizeof(int32_t)];
		int index = 0;

		while ( gScb ) {
			
			const int a = gScb->size();
			if ( a >= 0  ) {
				
				int c = gScb->read();
				if ( c >= 0 )
					buffer[index++] = (unsigned char)c;
					
				if ( index == sizeof(int32_t)) {
					index = 0;
					
					int32_t val = 0;
					memcpy(&val, buffer, sizeof(int32_t));
					std::cout << prefix2(wxDateTime::UNow()) << " avl = " << wxString::Format("%05d", a)  << "; cnt = " << wxString::Format("%04d", cnt) << "; read = " << 4 << "; val = " << val << std::endl;
					
					cnt++;
				}
			}
			
			//wxMilliSleep(1);
			
			if ( cnt > 100 )
				break;
			
			if ( cnt%16 == 0 )
				wxTheApp->Yield();
		}
	
	std::cout << prefix1(wxDateTime::UNow()) << "End . . ." << std::endl;
}

#undef prefix1
#undef prefix2



