#ifndef MAINDIALOG_H
#define MAINDIALOG_H
#include "wxcrafter.h"

class wxSVGCtrl;

class MainDialog : public MainDialogBaseClass
{
	public:
		MainDialog(wxWindow* parent);
		virtual ~MainDialog();
	
	private:
		void traceWxWidgetsVersion(std::ostream& out);
		void traceBoostVersion(std::ostream& out);
		void traceGccVersion(std::ostream& out);
		void traceWxSvgVersion(std::ostream& out);
		
	protected:
		virtual void onInitDialog(wxInitDialogEvent& event);
		
		wxSVGCtrl* svgCtrl;
};
#endif // MAINDIALOG_H
