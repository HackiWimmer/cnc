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
		
		void initControls();
		
		bool generateSymetricCirclePoints(std::vector<wxRealPoint>& pl, double xOffset, double yOffset, double sections, double radius);
		
		void generatePolygon();
		void generateKnob();
		
		void setPath(wxString path);
		
	public:
		PathGeneratorFrame(wxWindow* parent);
		virtual ~PathGeneratorFrame();
		
	protected:
		virtual void onCloseWindowFromButton(wxCommandEvent& event);
		virtual void onCloseWindow(wxCloseEvent& event);
		virtual void copyPath(wxCommandEvent& event);
		virtual void generatePath(wxCommandEvent& event);
};
#endif 
