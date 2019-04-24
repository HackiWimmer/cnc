#ifndef TEST_FRAMEWORK_CALLBACK_H
#define TEST_FRAMEWORK_CALLBACK_H

#include "Test_Base.h"
#include "SVGPathAssistant.h"

////////////////////////////////////////////////////////////////////////////
class TEST_FRAMEWORK_CALLBACK : public TEST_BASE {
	
	public:
		////////////////////////////////////////////////////////////////////
		TEST_FRAMEWORK_CALLBACK(UnitTests* frame)
		: TEST_BASE(frame)
		{}
		
		////////////////////////////////////////////////////////////////////
		virtual ~TEST_FRAMEWORK_CALLBACK() {}
		
		////////////////////////////////////////////////////////////////////
		virtual const char* name() { return "TEST_FRAMEWORK_CALLBACK"; }
		
	protected:
		////////////////////////////////////////////////////////////////////
		virtual bool runTest() {
	
			FrameworkCallbackInfo fci;
			fci.type = FrameworkCallbackInfo::CBType::SINGLE_CHOICE;
			fci.sc.in.headline = "Callback";
			fci.sc.in.message  = "What callback type should be tested?";
			fci.sc.in.items.clear();
			fci.sc.in.items.Add("All types . . . ");
			fci.sc.in.items.Add("Text Message");
			fci.sc.in.items.Add("Yes/No Question");
			fci.sc.in.items.Add("User Input");
			FRAMEWORK_CALLBACK(fci);
			
			std::clog << "Received '" << fci.sc.out.result << "' from choice dialog" << std::endl;
			
			if ( fci.sc.out.result < 0 ) {
				std::clog << "Test cancled" << std::endl;
				return true;
			}
			
			// switch choice
			switch ( fci.sc.out.result ) {
				case 0:		return allTypes();
				case 1:		return textMessage();
				case 2: 	return yesNoQuestion();
				case 3: 	return userValue();
				
				default:	 std::clog << "Received not known test type '" << fci.sc.out.result << "' from choice dialog" << std::endl;
							return false;
			}
			
			return false;
		}
		
		////////////////////////////////////////////////////////////////////
		bool allTypes() {
			textMessage();
			yesNoQuestion();
			userValue();
			
			return true;
		}
		
		////////////////////////////////////////////////////////////////////
		bool textMessage() {
			PARAGRAPH();
			std::clog << "::Text Message" << std::endl;
			
			FrameworkCallbackInfo fci;
			fci.type = FrameworkCallbackInfo::CBType::TEXT_MESSAGE;
			fci.tm.in.headline = "Callback";
			fci.tm.in.message  = "Hallo";
			FRAMEWORK_CALLBACK(fci);
			
			return true;
		}
		
		////////////////////////////////////////////////////////////////////
		bool userValue() {
			PARAGRAPH();
			std::clog << "::User Value" << std::endl;

			FrameworkCallbackInfo fci;
			fci.type = FrameworkCallbackInfo::CBType::USER_VALUE;
			fci.uv.in.headline 		= "Callback";
			fci.uv.in.message  		= "Choice . . .";
			fci.uv.in.defaultValue 	= "Hallo";
			FRAMEWORK_CALLBACK(fci);
			
			std::clog << "Received '" << fci.uv.out.value << "' from user input" << std::endl;
			
			return true;
		}
		
		////////////////////////////////////////////////////////////////////
		bool yesNoQuestion() {
			PARAGRAPH();
			std::clog << "::Yes/no Question" << std::endl;

			FrameworkCallbackInfo fci;
			fci.type = FrameworkCallbackInfo::CBType::YES_NO_QUESTION;
			fci.ynq.in.headline = "Callback";
			fci.ynq.in.message  = "Choice . . .";
			FRAMEWORK_CALLBACK(fci);
			
			std::clog << "Received '" << ( fci.ynq.out.result == wxID_YES ? "YES" : "NO") << "' from yes/no question" << std::endl;
			
			return true;
		}
};

#endif