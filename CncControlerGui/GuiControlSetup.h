#ifndef GUI_CONTROL_SETUP_H_INCLUDED
#define GUI_CONTROL_SETUP_H_INCLUDED

class wxTextCtrl;
class wxStaticText;
class wxStaticBitmap;
class wxWindow;
class wxPanel;
class wxCheckBox;
class wxSlider;
class wxDataViewListCtrl;
class CncMotionMonitor;
class CncZView;
class CncSpeedView;
class MainFrame;
class wxToggleButton;
class wxBitmapToggleButton;

struct GuiControlSetup {
	
	MainFrame* mainFrame						= NULL;
	
	CncMotionMonitor* motionMonitor				= NULL;
	
	wxTextCtrl* passingTrace					= NULL;
	wxTextCtrl* configuredFeedSpeed				= NULL;
	
	wxStaticBitmap* heartbeatState				= NULL;
	
	wxStaticBitmap* toolState					= NULL;
	wxMenuItem* motorState						= NULL;
	wxBitmapToggleButton* probeModeState		= NULL;
	
	wxStaticText* xMinLimit						= NULL;
	wxStaticText* xMaxLimit						= NULL;
	wxStaticText* yMinLimit						= NULL;
	wxStaticText* yMaxLimit						= NULL;
	wxStaticText* zMinLimit						= NULL;
	wxStaticText* zMaxLimit						= NULL;
	
	wxDataViewListCtrl* controllerConfig		= NULL;
	wxDataViewListCtrl* controllerPinReport		= NULL;
	
	wxToggleButton* testToggleTool   			= NULL;
};

#endif