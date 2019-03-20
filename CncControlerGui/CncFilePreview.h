#ifndef CNCFILEPREVIEW_H
#define CNCFILEPREVIEW_H

#include "3D/CncGCodePreview.h"
#include "CncSvgControl.h"
#include "wxcrafter.h"

class CncFilePreview : public CncFilePreviewBase
{
	public:
		CncFilePreview(wxWindow* parent);
		virtual ~CncFilePreview();
		
		bool selectEmptyPreview();
		bool selectSVGPreview(const wxString& fileName);
		bool selectGCodePreview(const wxString& fileName);
		bool selectBinaryPreview(const wxString& fileName);
		
		bool selectPreview(const wxString& fileName);
		
	private:
		CncGCodePreview* 	gcodePreview;
		CncSvgViewer* 		svgPreview;
		
		void installContent();
 
	protected:
		
		virtual void show3D(wxCommandEvent& event);
		virtual void showFromBottom3D(wxCommandEvent& event);
		virtual void showFromFront3D(wxCommandEvent& event);
		virtual void showFromLeft3D(wxCommandEvent& event);
		virtual void showFromRear3D(wxCommandEvent& event);
		virtual void showFromRight3D(wxCommandEvent& event);
		virtual void showFromTop3D(wxCommandEvent& event);
		
		const char* getBlankHtmlPage();
		const char* getErrorHtmlPage(const wxString& errorInfo);
		
		void activate3DPerspectiveButton(wxButton* bt);
};

#endif // CNCFILEPREVIEW_H
