#ifndef CNC_SPY_CONTROL
#define CNC_SPY_CONTROL

#include <wx/textctrl.h>

class CncSpyControl : public wxTextCtrl {
	
	protected:
		bool sytleChanged;
		
		void OnMouse(wxMouseEvent& event);
		void decodeSerialSpyLine(const wxString& line, bool displayInfo = true);
				
	public:
		CncSpyControl(wxWindow *parent, wxWindowID id);
		virtual ~CncSpyControl();
		
		void addMarker(const wxString& mt);
		
		virtual bool SetDefaultStyle(const wxTextAttr &style);
		virtual void AppendText(const wxString & text);
		virtual void AppendText(const wxChar & c);
		
		virtual void Clear();
		
		void clearDetails();
		
		// is used from global kex down hook
		void OnKeyDown(wxKeyEvent& event);
		
		wxDECLARE_EVENT_TABLE();
};

#endif