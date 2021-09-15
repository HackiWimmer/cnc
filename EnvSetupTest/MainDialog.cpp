#include <iostream>
#include <sstream>
#include <wx/version.h>
#include <boost/version.hpp>
#include <CncWxSvgVersionInfo.h>
#include "wxSVG/svgctrl.h"
#include "MainDialog.h"

///////////////////////////////////////////////////////////////////
MainDialog::MainDialog(wxWindow* parent)
: MainDialogBaseClass	(parent)
, svgCtrl				(NULL)
///////////////////////////////////////////////////////////////////
{
	SetTitle("Development Environment Setup Test . . . ");
}
///////////////////////////////////////////////////////////////////
MainDialog::~MainDialog() {
///////////////////////////////////////////////////////////////////

	wxDELETE( svgCtrl );
}
///////////////////////////////////////////////////////////////////
void MainDialog::traceWxWidgetsVersion(std::ostream& out) {
///////////////////////////////////////////////////////////////////
	out << wxMAJOR_VERSION
		<< "."
		<< wxMINOR_VERSION
		<< "."
		<< wxRELEASE_NUMBER;
		
		if ( wxSUBRELEASE_NUMBER > 0 ) {
			out << " - "
			    << wxSUBRELEASE_NUMBER;
		}
		
		out << std::endl;
}
///////////////////////////////////////////////////////////////////
void MainDialog::traceBoostVersion(std::ostream& out) {
///////////////////////////////////////////////////////////////////
	out << BOOST_VERSION / 100000
		<< "."
		<< BOOST_VERSION / 100 % 1000
		<< "."
		<< BOOST_VERSION % 100 
		<< std::endl;
}
///////////////////////////////////////////////////////////////////
void MainDialog::traceGccVersion(std::ostream& out) {
///////////////////////////////////////////////////////////////////
	out << __GNUC__
		<< "."
		<< __GNUC_MINOR__
		<< "."
		<< __GNUC_PATCHLEVEL__
		<< std::endl;
}
///////////////////////////////////////////////////////////////////
void MainDialog::traceWxSvgVersion(std::ostream& out) {
///////////////////////////////////////////////////////////////////
	out << CNC_WX_SVG_VERSIONFO
		<< std::endl;
}
///////////////////////////////////////////////////////////////////
void MainDialog::onInitDialog(wxInitDialogEvent& event) {
///////////////////////////////////////////////////////////////////
	{
		std::stringstream ss;
		traceWxWidgetsVersion(ss);
		m_wxVersionInfo->SetLabel(ss.str().c_str());
	}
	{
		std::stringstream ss;
		traceBoostVersion(ss);
		m_boostVersionInfo->SetLabel(ss.str().c_str());
	}
	{
		std::stringstream ss;
		traceGccVersion(ss);
		m_gccVersionInfo->SetLabel(ss.str().c_str());
	}
	{
		std::stringstream ss;
		traceWxSvgVersion(ss);
		m_wxSvgVersionInfo->SetLabel(ss.str().c_str());
	}
	
	svgCtrl = new wxSVGCtrl(m_wxSvgParent);
	svgCtrl->SetFitToFrame(true);
	svgCtrl->Load("C:\\@Development\\@Projekte\\c++\\CNCGuiController\\EnvSetupTest\\car.svg");
}
