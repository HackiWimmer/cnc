#ifndef GUI_CONTROL_SETUP_H_INCLUDED
#define GUI_CONTROL_SETUP_H_INCLUDED

class wxTextCtrl;
class wxStaticText;
class wxStaticBitmap;
class wxWindow;
class wxPanel;
class wxSlider;
class wxDataViewListCtrl;
class CncOpenGLDrawPane;
class CncZView;
class CncSpeedView;
class MainFrame;

struct GuiControlSetup {
	
	MainFrame* mainWnd			= NULL;
	
	CncOpenGLDrawPane* drawPane3D	= NULL;
	
	wxTextCtrl* xAxis 			= NULL; 
	wxTextCtrl* yAxis 			= NULL;
	wxTextCtrl* zAxis 			= NULL;
	
	wxTextCtrl* xAxisCtl		= NULL; 
	wxTextCtrl* yAxisCtl		= NULL;
	wxTextCtrl* zAxisCtl		= NULL;
	
	wxTextCtrl* cmdCount 		= NULL;
	wxTextCtrl* cmdDuration		= NULL;
	
	wxTextCtrl* passingTrace	= NULL;
	
	wxStaticBitmap* toolState	= NULL;
	
	wxStaticText* xMinLimit		= NULL;
	wxStaticText* xMaxLimit		= NULL;
	wxStaticText* yMinLimit		= NULL;
	wxStaticText* yMaxLimit		= NULL;
	wxStaticText* zMinLimit		= NULL;
	wxStaticText* zMaxLimit		= NULL;
	
	wxMenuItem* motorState		= NULL;
	
	CncZView* zView				= NULL;
	CncSpeedView* speedView     = NULL;
	
	wxPanel* xAxisMarkerTop		= NULL;
	wxPanel* xAxisMarkerBottom	= NULL;
	wxPanel* yAxisMarker		= NULL;

	wxDataViewListCtrl* staticCncConfig 		= NULL;
	wxDataViewListCtrl* dynamicCncConfig 		= NULL;
	wxDataViewListCtrl* setterValues			= NULL;
	wxDataViewListCtrl* processedSetters		= NULL;
	wxDataViewListCtrl* controllerConfig		= NULL;
	wxDataViewListCtrl* controllerPinReport		= NULL;
	wxDataViewListCtrl* controllerErrorInfo		= NULL;

};

#endif