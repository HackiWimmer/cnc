#ifndef CNCFILEPREVIEW_H
#define CNCFILEPREVIEW_H

#include "3D/CncGCodePreview.h"
#include "wxcrafter.h"

class CncFilePreview : public CncFilePreviewBase
{
	public:
		CncFilePreview(wxWindow* parent);
		virtual ~CncFilePreview();
		
		bool selectSVGPreview(const wxString& fileName);
		bool selectGCodePreview(const wxString& fileName);
		
		bool selectPreview(const wxString& fileName);
		
	private:
		CncGCodePreview* gcodePreview;
		
		void installContent();
 
};

#endif // CNCFILEPREVIEW_H
