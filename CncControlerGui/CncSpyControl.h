#ifndef CNC_SPY_CONTROL
#define CNC_SPY_CONTROL

#include <wx/textctrl.h>

class CncSpyControl : public wxTextCtrl {
	
	protected:
		bool sytleChanged;
		wxTextCtrl* detailCtrl;
	
		void OnMouse(wxMouseEvent& event);
		void decodeSerialSpyLine(const wxString& line);
				
	public:
		CncSpyControl(wxWindow *parent, wxWindowID id, wxTextCtrl* dc);
		virtual ~CncSpyControl();
		
		virtual bool SetDefaultStyle(const wxTextAttr &style);
		virtual void AppendText(const wxString & text);
		virtual void AppendText(const wxChar & c);
		
		virtual void Clear();
		
		// is used from global kex down hook
		void OnKeyDown(wxKeyEvent& event);
		
		wxDECLARE_EVENT_TABLE();
};

#endif