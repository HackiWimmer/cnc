#ifndef PATHGENERATORFRAME_H
#define PATHGENERATORFRAME_H

#include <wx/string.h>
#include "wxcrafter.h"
#include "CncCommon.h"
#include "PathGeneratorStore.h"

///////////////////////////////////////////////////////////////////////////
class PathGeneratorFrame : public PathGeneratorFrameBase
{
	private:
		const int minSizeHeight	= 70;
		int prevSizeHeight;
		
		wxStyledTextCtrl* templateEditor;
		
		long previewHeight;
		long previewWidth;
		
		long viewBoxX;
		long viewBoxY;
		long viewBoxW;
		long viewBoxH;
		
		PathGeneratorStore pathGeneratorStore;
		
		static wxString globaGeneratedPath;
		
		///////////////////////////////////////////////////////////////////
		void initControls();
		void closeWindow();
		void updatePreview();
		int getPathSelection();
		void decorateSizeButton();
		void enableControls(bool state);
		void setupPathSelector();
		void displayGrid(std::fstream& fs);
		
		///////////////////////////////////////////////////////////////////
		void setHelpInfos();
		void clearParameters();
		void selectPathSelector(int id);
		void clearView();
		void setupProperty(wxPGProperty* property, bool show);
		void setupProperty(wxPGProperty* property, bool show, wxVariant value);
		void setupCommonValues(const PathGeneratorBase::CommonValues& cv);
		void setupCncParameterValues(const PathGeneratorBase::CncParameterValues& cpv);
		
		///////////////////////////////////////////////////////////////////
		void updateParameterValues(const PathGeneratorBase::ParameterMap& pMap);
		void updateCommonValues(const PathGeneratorBase::CommonValues& cv);
		void updateTransformValues(const PathGeneratorBase::TransformValues& tv);
		void updateCncParameterValues(const PathGeneratorBase::CncParameterValues& cpv);
		
		///////////////////////////////////////////////////////////////////
		void evaluateCommonValues(int id);
		void evaluateSvgParameterValues(int id);
		void evaluateCncParameterValues(int id);
		void evaluatePathParameterValues(int id);
		
		///////////////////////////////////////////////////////////////////
		void generatePath();
		
		
		
		
		// obsolete
									bool generateSymetricCirclePoints(std::vector<wxRealPoint>& pl, double xOffset, double yOffset, double sections, double radius);
									void generatePolygon();
									void generateKnob();
									void setPath(wxString path);
		
	protected:

		virtual void onActivate(wxActivateEvent& event);
		virtual void relaceEditControlSelection(wxCommandEvent& event);
		virtual void onSize(wxSizeEvent& event);
		virtual void toogleSize(wxCommandEvent& event);
		virtual void propertyChanged(wxPropertyGridEvent& event);
		virtual void selectPathSelector(wxCommandEvent& event);
		virtual void clearView(wxCommandEvent& event);
		virtual void onCloseWindowFromButton(wxCommandEvent& event);
		virtual void onCloseWindow(wxCloseEvent& event);
		virtual void copyPath(wxCommandEvent& event);
		virtual void generatePath(wxCommandEvent& event);
		
	public:
		PathGeneratorFrame(wxWindow* parent, wxStyledTextCtrl* tplEditor);
		virtual ~PathGeneratorFrame();
		
		///////////////////////////////////////////////////////////////////
		virtual bool Show(bool show=true);
		
		///////////////////////////////////////////////////////////////////
		void updateEditControlCanReplaceState(bool state) {
			m_btPGReplaceEdit->Enable(state);
		}
		
		///////////////////////////////////////////////////////////////////
		wxTextCtrl* getPathTrace() { return m_generatedPreview; }
		
		///////////////////////////////////////////////////////////////////
		static const wxString& getCurrentGeneratedPath() {
			return globaGeneratedPath;
		}
		
		///////////////////////////////////////////////////////////////////
		bool regenerateSvgBlock(PathGeneratorStore::RegenerateParameter & rp);
};


#endif 
