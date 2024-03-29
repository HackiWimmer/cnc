#ifndef CNCFILEPREVIEW_H
#define CNCFILEPREVIEW_H

#include "3D/CncGCodePreview.h"
#include "CncSvgControl.h"
#include "CncPosition.h"
#include "wxCrafterFileView.h"

class CncFilePreview : public CncFilePreviewBase
{
	public:
		const unsigned int SVG_TAB_PAGE 	= 0;
		const unsigned int GCODE_TAB_PAGE 	= 1;
		
		CncFilePreview(wxWindow* parent, wxString name);
		virtual ~CncFilePreview();
		
		bool selectEmptyPreview();
		bool selectPreview(const wxString& fileName);
		void setTitle(const wxString& title)			{ m_previewTitle->SetLabel(title); }
		
		void normalizeView();
		
		friend class CncFilePreviewWnd;
		
	private:
		CncTemplateFormat	tplFormat;
		CncGCodePreview* 	gcodePreview;
		CncSvgViewer* 		svgPreview;
		wxString			previewName;
		wxFileName			lastFileName;
		
	protected:
		
		bool loadFile();
		
		bool selectEmptyPreviewIntern();
		bool selectSVGPreview();
		bool selectGCodePreview();
		bool selectBinaryPreview(const wxString& fileName);
		
		virtual void onMouse(wxMouseEvent& event);
		virtual void onPreviewBookChanged(wxNotebookEvent& event);
		
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
		
		bool evaluateMetricBoundaries() const;
};

#endif // CNCFILEPREVIEW_H
