#include <wx/evtloop.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/choicdlg.h>
#include "GlobalFunctions.h"
#include "CncLoggerProxy.h"
#include "Tests/Test_FrameworkCallback.h"
#include "Tests/Test_SVGNodeParser.h"
#include "Tests/Test_CncPathListManager.h"
#include "MainFrame.h"
#include "UnitTestFrame.h"

/////////////////////////////////////////////////////////////////////////////
UnitTests::UnitTests(wxWindow* parent, int iti, bool ar)
: CncUnitTestsBase(parent)
, testResultStream(new CncTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_testResultStreamPlaceholder->GetWindowStyle()))
, initialTestIdx(iti)
, autorun(ar)
, redirector(NULL)
{
/////////////////////////////////////////////////////////////////////////////
	GblFunc::cloneAttributes(m_testResultStreamPlaceholder, 	testResultStream);
	GblFunc::replaceControl(m_testResultStreamPlaceholder, 		testResultStream);
	redirector = new StdStreamRedirector(testResultStream);
	
	// install tests
	testStore.push_back(new TEST_FRAMEWORK_CALLBACK(this));
	testStore.push_back(new TEST_CNC_PATH_LIST_MANAGER(this));
	testStore.push_back(new TEST_SVG_NODE_PARSER(this));
	// ...

	unsigned int cnt = 0;
	for (auto it = testStore.begin(); it != testStore.end(); ++it) {
		wxString item(wxString::Format("[%04d] %s", cnt, UnitTests::formatName((*it)->name())));
		m_unitTestSelector->Append(item);
		cnt++;
	}
	
	// do this definitly here, for more information please see UnitTests::enableControls()
	m_btUnitTestRun->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(UnitTests::runTest), NULL, this);
}
/////////////////////////////////////////////////////////////////////////////
UnitTests::~UnitTests() {
/////////////////////////////////////////////////////////////////////////////
	if ( redirector )
		delete redirector;
		
	testStore.clear();
	
	// do this definitly here, for more information please see UnitTests::enableControls()
	m_btUnitTestRun->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(UnitTests::runTest), NULL, this);
}
/////////////////////////////////////////////////////////////////////////////
const wxString& UnitTests::formatName(const wxString name) {
/////////////////////////////////////////////////////////////////////////////
	static wxString s;
	
	s.clear();
	wxStringTokenizer st(name, "_");
	while ( st.HasMoreTokens() ) {
		s.append( st.GetNextToken().MakeCapitalized() );
	}
	
	return s;
}
/////////////////////////////////////////////////////////////////////////////
void UnitTests::runTest(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////////////
	runTest();
}
/////////////////////////////////////////////////////////////////////////////
void UnitTests::selectTest(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////////////
	if ( m_unitTestAutoRun->IsChecked() )
		runTest();
}
/////////////////////////////////////////////////////////////////////////////
void UnitTests::onShow(wxShowEvent& event) {
/////////////////////////////////////////////////////////////////////////////
	// curently nothing to do
}
/////////////////////////////////////////////////////////////////////////////
void UnitTests::runTest() {
/////////////////////////////////////////////////////////////////////////////
	testResultStream->Clear();
	testResultStream->Refresh();
	testResultStream->Update();
	
	// to start scrolling immediately
	for ( unsigned int i=0; i<50; i++ )
		std::cout << std::endl;

	enableControls(false);
	m_unitTestStatus->SetLabel("Test is running . . .");
	
	unsigned int sel = m_unitTestSelector->GetSelection();
	if ( sel >=0 && sel < testStore.size() ) {
		bool ret = testStore[sel]->run();
		
		wxString message(( ret == true ? "Test was successful . . ." : "Test was failed . . ."));
		m_unitTestStatus->SetLabel(message);
	}
		
	enableControls(true);
}
/////////////////////////////////////////////////////////////////////////////
void UnitTests::onStartupTimer(wxTimerEvent& event) {
/////////////////////////////////////////////////////////////////////////////
	if ( initialTestIdx >= 0 && initialTestIdx < (int)m_unitTestSelector->GetCount() )
		m_unitTestSelector->Select(initialTestIdx);
		
	if ( autorun == true )
		runTest();
}
/////////////////////////////////////////////////////////////////////////////
void UnitTests::clearView(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////////////////////
	testResultStream->Clear();
}
/////////////////////////////////////////////////////////////////////////////
void UnitTests::enableControls(bool state) {
/////////////////////////////////////////////////////////////////////////////
	m_btUnitTestClear->Enable(state);
	m_unitTestSelector->Enable(state);
	m_unitTestFreezeOutput->Enable(state);
	
	if ( m_unitTestFreezeOutput->IsChecked() ) {
		if ( state == false ) 	testResultStream->Freeze();
		else					testResultStream->Thaw();
	}
	
	// connect and disconnect runTest here because m_btUnitTestRun->Enable(state)
	// causes a not wanted horizontal scrolling of the test result stream
	if ( state == true ) {
		// fetch existing events before reconnecting
		THE_APP->dispatchAll();
			
		m_btUnitTestRun->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(UnitTests::runTest), NULL, this);
	} else {
		m_btUnitTestRun->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(UnitTests::runTest), NULL, this);
	}
	
	m_btUnitTestRun->Refresh();
	m_btUnitTestRun->Update();
	
	state == true ? wxSetCursor(wxNullCursor) : wxSetCursor(wxCURSOR_WAIT);
}
/////////////////////////////////////////////////////////////////////////////
void UnitTests::callbackFromTest(FrameworkCallbackInfo& fci) {
/////////////////////////////////////////////////////////////////////////////
	switch ( fci.type ) {
		case FrameworkCallbackInfo::CBType::TEXT_MESSAGE: {
			wxMessageDialog dlg(this, fci.tm.in.message, fci.tm.in.headline, wxOK | fci.tm.in.icon);
			dlg.ShowModal();
			break;
		}
		
		case FrameworkCallbackInfo::CBType::YES_NO_QUESTION: {
			wxMessageDialog dlg(this, fci.ynq.in.message, fci.ynq.in.headline, wxYES | wxNO | wxICON_QUESTION);
			fci.ynq.out.result = dlg.ShowModal();
			break;
		}
		
		case FrameworkCallbackInfo::CBType::USER_VALUE: {
			wxTextEntryDialog dlg(this, fci.uv.in.message, fci.uv.in.headline, fci.uv.in.defaultValue);
			
			fci.uv.out.result = dlg.ShowModal();
			if ( fci.uv.out.result == wxID_OK ) 	fci.uv.out.value = dlg.GetValue();
			else 									fci.uv.out.value = "";
			
			break;
		}
		
		case FrameworkCallbackInfo::CBType::SINGLE_CHOICE: {
			wxSingleChoiceDialog dlg(this, fci.sc.in.message, fci.sc.in.headline, fci.sc.in.items);
			dlg.SetSelection(fci.sc.in.defaultSelection);
			
			if ( dlg.ShowModal() == wxID_OK ) 	fci.sc.out.result = dlg.GetSelection();
			else 								fci.sc.out.result = -1;

			break;
		}
		
		default: {
			wxMessageDialog dlg(this, wxString::Format("Reveived Callback Type: %d", fci.type), "Unknown Callback Type . . . ", wxOK | wxICON_ERROR);
			dlg.ShowModal();
		}
	}
}
