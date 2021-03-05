#include <wx/gdicmn.h>
#include <wx/richtooltip.h>
#include "MainFrame.h"
#include "CncAutoProgressDialog.h"
#include "CncTouchBlockDetector.h"
#include "CncTouchblockManagement.h"

///////////////////////////////////////////////////////////////////
CncTouchblockManagement::CncTouchblockManagement(wxWindow* parent)
: CncTouchblockManagementBase	(parent)
///////////////////////////////////////////////////////////////////
{
	wxImage img1 = m_btTouchModeA->GetBitmap().ConvertToImage();
	m_btTouchModeB->SetBitmap(img1.Rotate90(false));
	
	wxImage img2 = m_btTouchModeB->GetBitmap().ConvertToImage();
	m_btTouchModeC->SetBitmap(img2.Rotate90(false));
	
	wxImage img3 = m_btTouchModeC->GetBitmap().ConvertToImage();
	m_btTouchModeD->SetBitmap(img3.Rotate90(false));
	
	setTouchMode(TM_A);
	m_btTakeOver->Enable(false);
	
	wxFloatingPointValidator<float> val(3, NULL, wxNUM_VAL_DEFAULT );//, wxNUM_VAL_ZERO_AS_BLANK);
	val.SetRange(0, 50.0);	val.SetPrecision(3);
	
	m_touchDiameter			->SetValidator(val);
	m_touchBockThickness	->SetValidator(val);
	m_touchXCheekThickness	->SetValidator(val);
	m_touchYCheekThickness	->SetValidator(val);
	
	#warning use config values here
	m_touchDiameter			->SetValue(wxString::Format("%.3lf", 8.000));
	m_touchBockThickness	->SetValue(wxString::Format("%.3lf", 7.400));
	m_touchXCheekThickness	->SetValue(wxString::Format("%.3lf", 9.950));
	m_touchYCheekThickness	->SetValue(wxString::Format("%.3lf", 9.950));
	m_cbPrevTest			->SetValue(false);
}
///////////////////////////////////////////////////////////////////
CncTouchblockManagement::~CncTouchblockManagement() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
const CncTouchblockManagement::TouchMode CncTouchblockManagement::getTouchMode() const {
///////////////////////////////////////////////////////////////////
	if		( m_btTouchModeA->GetValue() ) return TM_A;
	else if ( m_btTouchModeB->GetValue() ) return TM_B;
	else if ( m_btTouchModeC->GetValue() ) return TM_C;
	else if ( m_btTouchModeD->GetValue() ) return TM_D;
	
	return TM_UNKNOWN;
}
///////////////////////////////////////////////////////////////////
void CncTouchblockManagement::setTouchMode(const TouchMode m) {
///////////////////////////////////////////////////////////////////
	m_btTouchModeA->SetValue(m == TM_A);
	m_btTouchModeB->SetValue(m == TM_B);
	m_btTouchModeC->SetValue(m == TM_C);
	m_btTouchModeD->SetValue(m == TM_D);
}
///////////////////////////////////////////////////////////////////
void CncTouchblockManagement::onTouchBlock(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	if		( event.GetEventObject() == m_btTouchModeA ) setTouchMode(TM_A);
	else if ( event.GetEventObject() == m_btTouchModeB ) setTouchMode(TM_B);
	else if ( event.GetEventObject() == m_btTouchModeC ) setTouchMode(TM_C);
	else if ( event.GetEventObject() == m_btTouchModeD ) setTouchMode(TM_D);
}
///////////////////////////////////////////////////////////////////
double CncTouchblockManagement::getValueAsDouble(wxTextCtrl* ctrl, double defaultValue) {
///////////////////////////////////////////////////////////////////
	if ( ctrl == NULL )
		return defaultValue;
		
	double d;
	if ( ctrl->GetValue().ToDouble(&d) )
		return d;
		
	return defaultValue;
}
///////////////////////////////////////////////////////////////////
void CncTouchblockManagement::onTouchTest(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_btTakeOver->Enable(false);
	
	Enable(false);
	CncAutoProgressDialog progressDlg(this);
	progressDlg.Show(true);
		
		wxString msg;
		msg =	"Close the contact manually. " \
				"The test stops after some seconds if no closed contact state was detected.";
		
		m_infobar->ShowMessage(msg);
		
		CncTouchBlockDetector::Parameters para;
		para.touchMode = CncTouchBlockDetector::Parameters::TM_TOUCH_TEST;
		CncTouchBlockDetector::Result result = THE_APP->processTouchTest(para);
		
		m_infobar->Dismiss();
		
	progressDlg.Show(false);
	Enable(true);
	
	// final report
	wxRichToolTip tip("Contact Test:", result.processResult ? "Contact was closed." : "Contact was not closed!");
	tip.SetIcon(result.processResult ? wxICON_INFORMATION : wxICON_ERROR);
	tip.SetTipKind(wxTipKind_TopLeft);
	tip.ShowFor((wxWindow*)event.GetEventObject());
}
///////////////////////////////////////////////////////////////////
void CncTouchblockManagement::onTouchZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	m_btTakeOver->Enable(false);
	
	Enable(false);
	CncAutoProgressDialog progressDlg(this);
	progressDlg.Show(true);

		if ( m_cbPrevTest->GetValue() == true ) {
			
			wxString msg;
			msg =	"Close the contact manually. " \
					"The test stops after some seconds if no closed contact state was detected.";
			
			m_infobar->ShowMessage(msg);
			
			CncTouchBlockDetector::Parameters testPara;
			testPara.touchMode = CncTouchBlockDetector::Parameters::TM_TOUCH_TEST;
			CncTouchBlockDetector::Result result = THE_APP->processTouchTest(testPara);
			
			wxRichToolTip tip("Prior Contact Test:", result.processResult ? "Contact was closed." : "Contact was not closed!");
			tip.SetIcon(result.processResult ? wxICON_INFORMATION : wxICON_ERROR);
			tip.SetTipKind(wxTipKind_TopLeft);
			tip.ShowFor((wxWindow*)event.GetEventObject());
			
			if ( result.processResult == false )
				return;
				
			m_infobar->ShowMessage("The process will wait a second . . . ");
			THE_APP->waitActive(1000);
		}
		
		m_infobar->ShowMessage("Running the Z Touch  . . . ");
		
		CncTouchBlockDetector::Parameters para;
		para.touchMode					= CncTouchBlockDetector::Parameters::TM_TOUCH_Z;
		para.touchDiameter				= getValueAsDouble(m_touchDiameter,			0.0);
		para.touchBlockZThickness		= getValueAsDouble(m_touchBockThickness,	0.0);
		CncTouchBlockDetector::Result result = THE_APP->processZTouch(para);

	progressDlg.Show(false);
	Enable(true);
	
	m_infobar->Dismiss();
	
	wxRichToolTip tip("Z Touch:", result.processResult ? "Z Touch was successful." : "Z Touch was failed!");
	tip.SetIcon(result.processResult ? wxICON_INFORMATION : wxICON_ERROR);
	tip.SetTipKind(wxTipKind_TopLeft);
	tip.ShowFor((wxWindow*)event.GetEventObject());
	
	m_btTakeOver->Enable(result.processResult);
}
///////////////////////////////////////////////////////////////////
void CncTouchblockManagement::onTouchXYZ(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	auto evalTouchCorner = [](const TouchMode tm) {
		switch ( tm ) {
			case TM_D:		return CncTouchBlockDetector::Parameters::TouchCorner::TC_LEFT_TOP;
			case TM_C:		return CncTouchBlockDetector::Parameters::TouchCorner::TC_RIGHT_TOP;
			case TM_B:		return CncTouchBlockDetector::Parameters::TouchCorner::TC_RIGHT_BOTTOM;
			case TM_A:
			default:		return CncTouchBlockDetector::Parameters::TouchCorner::TC_LEFT_BOTTOM;
			
		}
		
		return CncTouchBlockDetector::Parameters::TouchCorner::TC_LEFT_BOTTOM;
	};
	
	m_btTakeOver->Enable(false);
	
	Enable(false);
	CncAutoProgressDialog progressDlg(this);
	progressDlg.Show(true);

		if ( m_cbPrevTest->GetValue() == true ) {
			
			wxString msg;
			msg =	"Close the contact manually. " \
					"The test stops after some seconds if no closed contact state was detected. [ESC] will abort the processing . . .";
			
			m_infobar->ShowMessage(msg);
			
			CncTouchBlockDetector::Parameters testPara;
			testPara.touchMode = CncTouchBlockDetector::Parameters::TM_TOUCH_TEST;
			CncTouchBlockDetector::Result result = THE_APP->processTouchTest(testPara);
			
			wxRichToolTip tip("Prior Contact Test:", result.processResult ? "Contact was closed." : "Contact was not closed!");
			tip.SetIcon(result.processResult ? wxICON_INFORMATION : wxICON_ERROR);
			tip.SetTipKind(wxTipKind_TopLeft);
			tip.ShowFor((wxWindow*)event.GetEventObject());
			
			if ( result.processResult == false )
				return;
				
			m_infobar->ShowMessage("The process will wait a second . . . ");
			THE_APP->waitActive(1000);
		}
		
		m_infobar->ShowMessage("Running the XYZ Touch  . . . ");
		
		CncTouchBlockDetector::Parameters para;
		para.touchMode					= CncTouchBlockDetector::Parameters::TM_TOUCH_XYZ;
		para.touchCorner				= evalTouchCorner(getTouchMode());
		para.touchDiameter				= getValueAsDouble(m_touchDiameter,			0.0);
		para.touchBlockZThickness		= getValueAsDouble(m_touchBockThickness,	0.0);
		para.touchBlockXCheekThickness	= getValueAsDouble(m_touchXCheekThickness,	0.0);
		para.touchBlockYCheekThickness	= getValueAsDouble(m_touchYCheekThickness,	0.0);
		CncTouchBlockDetector::Result result = THE_APP->processZTouch(para);

	progressDlg.Show(false);
	Enable(true);
	
	m_infobar->Dismiss();
	
	wxRichToolTip tip("XYZ Touch:", result.processResult ? "XYZ Touch was successful." : "XYZ Touch was failed!");
	tip.SetIcon(result.processResult ? wxICON_INFORMATION : wxICON_ERROR);
	tip.SetTipKind(wxTipKind_TopLeft);
	tip.ShowFor((wxWindow*)event.GetEventObject());
	
	m_btTakeOver->Enable(result.processResult);
}
///////////////////////////////////////////////////////////////////
void CncTouchblockManagement::onCancel(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	EndModal(wxID_CANCEL);
}
///////////////////////////////////////////////////////////////////
void CncTouchblockManagement::onTakeOver(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	EndModal(wxID_OK);
}
