#include "MainFrame.h"
#include <wx/aboutdlg.h>

#include <vector>
#include <iostream>
#include <sstream>

//#include <boost/stacktrace.hpp>
#include <wxSVG/svgctrl.h>

MainFrame::MainFrame(wxWindow* parent)
    : MainFrameBaseClass(parent)
{
	
	
	wxSVGCtrl* m_svgCtrl = new wxSVGCtrl(m_panel55);
	m_svgCtrl->Load(wxT("tiger.svg")); 
	
	
	std::vector<int> ccc;
}

MainFrame::~MainFrame()
{
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Close();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
	std::stringstream ss;
	ss << "Start\n";
	//ss << boost::stacktrace::stacktrace();
	ss << "End\n";


    wxUnusedVar(event);
    wxAboutDialogInfo info;
    info.SetCopyright(_("My MainFrame"));
    info.SetLicence(_("GPL v2 or later"));
    info.SetDescription(ss.str().c_str());
    ::wxAboutBox(info); 
}
