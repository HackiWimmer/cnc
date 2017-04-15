#ifndef PATHGENERATORFRAME_H
#define PATHGENERATORFRAME_H
#include <wx/string.h>
#include "wxcrafter.h"


class PathGeneratorFrame : public PathGeneratorFrameBase
{
	private:
		unsigned int previewHeight;
		unsigned int previewWidth;
		
		wxString errorText;

		void closeWindow();
		void updatePreview();
		
		int getPathSelection();
		
		void initControls();
		void setupPathSelector();
		
		void setupPolygon();
		
		void clearParameters();
		void clearParameter(wxPGProperty* p);
		
		
		bool generateSymetricCirclePoints(std::vector<wxRealPoint>& pl, double xOffset, double yOffset, double sections, double radius);
		
		void generatePolygon();
		void generateKnob();
		void generatePocketWhole();
		
		void setPath(wxString path);
		
	protected:
		virtual void selectPathSelector(wxCommandEvent& event);
		virtual void clearView(wxCommandEvent& event);
		virtual void onCloseWindowFromButton(wxCommandEvent& event);
		virtual void onCloseWindow(wxCloseEvent& event);
		virtual void copyPath(wxCommandEvent& event);
		virtual void generatePath(wxCommandEvent& event);
		
	public:
		PathGeneratorFrame(wxWindow* parent);
		virtual ~PathGeneratorFrame();
		
};
#endif 
