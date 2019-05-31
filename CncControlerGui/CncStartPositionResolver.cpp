#include "CncConfig.h"
#include "MainFrame.h"
#include "CncStartPositionResolver.h"

const CncDoublePosition CncStartPositionResolver::ReferencePosition	= CncDoublePosition(0.0, 0.0, 0.0);

///////////////////////////////////////////////////////////////////
CncStartPositionResolver::CncStartPositionResolver(wxWindow* parent)
: CncStartPositionResolverBase(parent)
, distance(0.0, 0.0, 0.0)
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
	
	const CncDoublePosition cp 	= THE_APP->getCncControl()->getCurAppPosMetric();
	const CncDoublePosition rf 	= CncStartPositionResolver::ReferencePosition;
	distance					= rf - cp;
	
	format(cp,			m_currentPositionX, 	m_currentPositionY, 	m_currentPositionZ);
	format(rf,			m_referencePositionX,	m_referencePositionY,	m_referencePositionZ);
	format(distance, 	m_distanceX, 			m_distanceY, 			m_distanceZ);

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
	
	wxStringTokenizer tokenizer(sel, "-> ");
	while ( tokenizer.HasMoreTokens() ) {
		wxString token = tokenizer.GetNextToken();
		
		if ( token.IsEmpty() == false ) {
			//std::cout << token << std::endl;
			bool ret = true;
			if 		( token == "dx") { ret = THE_APP->getCncControl()->moveRelLinearMetricXYZ(distance.getX(), 	0.0, 				0.0				, false); }
			else if ( token == "dy") { ret = THE_APP->getCncControl()->moveRelLinearMetricXYZ(0.0, 				distance.getY(), 	0.0				, false); }
			else if ( token == "dz") { ret = THE_APP->getCncControl()->moveRelLinearMetricXYZ(0.0, 				0.0, 				distance.getZ()	, false); }
			else					 { ret = false; }
			
			if ( ret == false ) {
				std::cerr << "CncStartPositionResolver::onOk(): Error while resolve '" << token << "'" << std::endl;
				return wxID_ABORT;
			}
		}
	}
	
	return wxID_OK;
}
