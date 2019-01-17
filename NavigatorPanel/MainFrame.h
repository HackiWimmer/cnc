#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "CncNavigatorPanel.h"
#include "CncMouseRemoteControl.h"

#include "wxcrafter.h"

class MainFrame : public MainFrameBaseClass
{
public:
    MainFrame(wxWindow* parent);
    virtual ~MainFrame();

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	
	void onNavigatorPanel(CncNavigatorPanelEvent& event);
	void onRemoteControl(CncMouseRemoteControlEvent& event);
	
	void globalKeyDownHook(wxKeyEvent& event);

protected:

};
#endif // MAINFRAME_H
