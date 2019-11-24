#ifndef UNITTESTS_H
#define UNITTESTS_H

#include "Tests/Test_Base.h"
#include "StdStreamRedirector.h"
#include "wxcrafter.h"

//////////////////////////////////////////////////////////////////
class UnitTests : public CncUnitTestsBase {
	
	public:
		UnitTests(wxWindow* parent, int initialTestIdx=-1, bool autoRun=false);
		virtual ~UnitTests();
		
		void runTest();
		void callbackFromTest(FrameworkCallbackInfo& fci);
		
		static const wxString& formatName(const wxString name);
		
	protected:
		virtual void onShow(wxShowEvent& event);
		virtual void runTest(wxCommandEvent& event);
		virtual void selectTest(wxCommandEvent& event);
		virtual void onStartupTimer(wxTimerEvent& event);
		virtual void clearView(wxCommandEvent& event);
		
	private:
		TestStore testStore;
		CncTextCtrl* testResultStream;
		
		int initialTestIdx;
		bool autorun;
		StdStreamRedirector* redirector;
		
		void enableControls(bool state);
		
};
#endif // UNITTESTS_H
