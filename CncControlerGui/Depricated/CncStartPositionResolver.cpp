#include "CncConfig.h"
#include "CncControl.h"
#include "MainFrameProxy.h"
#include "CncMotionMonitor.h"
#include "CncStartPositionResolver.h"

const CncDoublePosition CncStartPositionResolver::ReferencePosition	= CncDoublePosition(0.0, 0.0, 0.0);
const char* UnchgStr = "<Leave Unchanged>";

///////////////////////////////////////////////////////////////////
CncStartPositionResolver::CncStartPositionResolver(wxWindow* parent)
: CncStartPositionResolverBase	(parent)
, distance						(0.0, 0.0, 0.0)
///////////////////////////////////////////////////////////////////
{
	auto format = [&](const CncDoublePosition& p, wxTextCtrl* X, wxTextCtrl* Y, wxTextCtrl* Z) {
		if ( X == NULL )	return;
		if ( Y == NULL )	return;
		if ( Z == NULL )	return;
			
		X->ChangeValue(wxString::Format("%+6.3lf", p.getX()));
		Y->ChangeValue(wxString::Format("%+6.3lf", p.getY()));
		Z->ChangeValue(wxString::Format("%+6.3lf", p.getZ()));
	};
	
	const CncDoublePosition cp 	= APP_PROXY::getCncControl()->getCurAppPosMetric();
	const CncDoublePosition rf 	= CncStartPositionResolver::ReferencePosition;
	distance					= rf - cp;
	
	format(cp,			m_currentPositionX, 	m_currentPositionY, 	m_currentPositionZ);
	format(rf,			m_referencePositionX,	m_referencePositionY,	m_referencePositionZ);
	format(distance, 	m_distanceX, 			m_distanceY, 			m_distanceZ);
	
	wxArrayString items;
	items.Add("Mode 01:              M[xy]");
	items.Add("Mode 02:              U[z]  ->  M[xy]");
	items.Add("Mode 03:              U[z]  ->  M[x]  -> M[y]");
	
	items.Add(wxString::Format("Mode 99:              %s",UnchgStr));
	
	m_sequenceList->Clear();
	m_sequenceList->InsertItems(items, 0);
	m_sequenceList->Select(0);
	m_sequenceList->SetFocus();
}
///////////////////////////////////////////////////////////////////
CncStartPositionResolver::~CncStartPositionResolver() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncStartPositionResolver::onCancel(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	EndModal(wxID_CANCEL);
}
///////////////////////////////////////////////////////////////////
void CncStartPositionResolver::onOk(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	EndModal(resolve());
}
///////////////////////////////////////////////////////////////////
int CncStartPositionResolver::resolve() {
///////////////////////////////////////////////////////////////////
	const wxString sel(m_sequenceList->GetStringSelection().AfterFirst(':').Trim(false).Trim(true));
	return resolve(sel);
}
///////////////////////////////////////////////////////////////////
int CncStartPositionResolver::resolve(const wxString& cmd) {
///////////////////////////////////////////////////////////////////
	wxStringTokenizer tokenizer(cmd, "-> ");
	while ( tokenizer.HasMoreTokens() ) 
	{
		const wxString token = tokenizer.GetNextToken();
		
		if ( token.IsEmpty() == false ) 
		{
			//std::cout << token << std::endl;
			
			CncControl* cnc = APP_PROXY::getCncControl();
			const double dx = -distance.getX();
			const double dy = -distance.getY();
			const double dz = -distance.getZ();// + THE_CONFIG->getWorkpieceOffset();
			
			APP_PROXY::getMotionMonitor()->pushInteractiveProcessMode();
			
			bool ret = true;
			if 		( token == "M[x]")		{ ret = cnc->moveRelLinearMetricXYZ( dx,	0.0,	0.0, false); }
			else if ( token == "M[y]")		{ ret = cnc->moveRelLinearMetricXYZ(0.0,	 dy,	0.0, false); }
			else if ( token == "M[z]")		{ ret = cnc->moveRelLinearMetricXYZ(0.0,	0.0,	 dz, false); }
			else if ( token == "M[xy]")		{ ret = cnc->moveRelLinearMetricXYZ( dx,	 dy,	0.0, false); }
			else if ( token == "M[xyz]")	{ ret = cnc->moveRelLinearMetricXYZ( dx,	 dy,	 dz, false); }
			else if ( token == "U[z]")		{ ret = cnc->moveZToMaxLimit(); }
			else if ( token == UnchgStr )	{ ret = true; }
			else							{ ret = false; }
			
			APP_PROXY::getMotionMonitor()->popInteractiveProcessMode();
			
			if ( ret == false )
			{
				CNC_CERR_FUNCT_A("\nError while resolve '%s'\n"
								 " distance = %.3lf, %.3lf, %.3lf; dx = %.3lf, dy = %.3lf, dz = %.3lf", 
									token, 
									distance.getX(),
									distance.getY(),
									distance.getZ(),
									dx, 
									dy, 
									dz
				)
				
				return wxID_ABORT;
			}
		}
	}
	
	return wxID_OK;
}
