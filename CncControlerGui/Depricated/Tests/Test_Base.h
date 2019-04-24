#ifndef TEST_BASE_H
#define TEST_BASE_H

#include <iostream>
#include <vector>
#include <exception>
#include <stdexcept>
#include <wx/filename.h>

// forward declaration
class UnitTests;

////////////////////////////////////////////////////////////////////////
struct FrameworkCallbackInfo {
	enum CBType {UNDEFINED, TEXT_MESSAGE, YES_NO_QUESTION, USER_VALUE, SINGLE_CHOICE};
	
	CBType type = UNDEFINED;
	
	struct TextMessage {
		struct In {
			int icon = wxICON_INFORMATION;
			wxString message;
			wxString headline;
		} in;
	} tm;
	
	struct YesNoQuestion {
		struct In {
			wxString message;
			wxString headline;
		} in;
		
		struct Out {
			int result = 0;
		} out;
	} ynq;
	
	struct UserValue {
		struct In {
			wxString message;
			wxString headline;
			wxString defaultValue;
		} in;
		
		struct Out {
			int result;
			wxString value;
		} out;
	} uv;
	
	struct SingleChoice {
		struct In {
			wxString message;
			wxString headline;
			wxArrayString items;
			unsigned int defaultSelection = 0;
		} in;
		
		struct Out {
			int result = 0;
		} out;
	} sc;
};

////////////////////////////////////////////////////////////////////////
class TEST_BASE {
	
	protected:
		
		UnitTests* unitTestFrame;
		
		////////////////////////////////////////////////////////////////
		void FRAMEWORK_CALLBACK(FrameworkCallbackInfo& fci);
		
		////////////////////////////////////////////////////////////////
		void CHECK_RESULT(bool result, const wxString& message, const char* file, int line) {
			if ( result == false ) {
				wxFileName f(file);
				throw std::runtime_error(wxString::Format("\n%s:[%d]: %s", f.GetFullName(), line, message).c_str());
			}
		}
		
		////////////////////////////////////////////////////////////////
		void HLINE() {
			HLINE(std::cout);
		}
		
		////////////////////////////////////////////////////////////////
		void HLINE(std::ostream &ostr, char c='*') {
			wxString s(c, 80);
			ostr << s << std::endl;
		}
		
		////////////////////////////////////////////////////////////////
		void PARAGRAPH() {
			PARAGRAPH(std::cout);
		}
		
		////////////////////////////////////////////////////////////////
		void PARAGRAPH(std::ostream &ostr) {
			ostr << "#\n";
		}
		
		////////////////////////////////////////////////////////////////
		virtual bool runTest() = 0;
	
	public:
		
		////////////////////////////////////////////////////////////////
		TEST_BASE(UnitTests* frame)
		: unitTestFrame(frame) 
		{}
		
		////////////////////////////////////////////////////////////////
		virtual ~TEST_BASE() {}
		
		////////////////////////////////////////////////////////////////
		virtual const char* name() = 0;
		
		////////////////////////////////////////////////////////////////
		bool run() {
			bool ret = false;
			try {
				HLINE(std::clog, '+'); std::clog << "Test Start . . ."<< std::endl; HLINE(std::clog, '+');
				
				ret = runTest();
				
				HLINE(std::clog, '-'); std::clog << "Test End . . ."<< std::endl; HLINE(std::clog, '-');
				
			} catch (std::runtime_error& e) {
				std::cerr << "TEST_BASE::run failed: Details:" << std::endl;
				std::cerr << e.what() << std::endl;
				ret = false;
				
			} catch (std::exception& e) {
				std::cerr << "TEST_BASE::run failed: Details:" << std::endl;
				std::cerr << e.what() << std::endl;
				ret = false;
				
			} catch (...) {
				std::cerr << "TEST_BASE::run failed: Unknown error . . ." << std::endl;
				ret = false;
			}
			
			return ret;
		}
		

		
};

////////////////////////////////////////////////////////////////
typedef std::vector<TEST_BASE*> TestStore;

#endif