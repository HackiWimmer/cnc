#ifndef CNCFILEPREVIEWWND_H
#define CNCFILEPREVIEWWND_H

#include "CncFilePreview.h"

class CncFilePreviewWnd : public CncFilePreviewWndBase
{
	public:
		enum PreviewType { PT_AUTO, PT_SVG, PT_GCODE};
		
		CncFilePreviewWnd(wxWindow* parent);
		virtual ~CncFilePreviewWnd();
		
		bool previewFile(const wxString& fileName, PreviewType pt=PT_AUTO);
		
	private:
		CncFilePreview* preview;
		
		void installContent();

	protected:
		virtual void onClose(wxCloseEvent& event);
};

#endif // CNCFILEPREVIEWWND_H
