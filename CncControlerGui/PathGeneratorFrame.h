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
		const int minSizeHeight	= 74;
		int prevSizeHeight;
		
		int leftSplitterWidth;
		bool leftSplitterMinimized;
		
		bool hasErrorInfo;
		
		wxStyledTextCtrl* templateEditor;
		
		long previewHeight;
		long previewWidth;
		
		long viewBoxX;
		long viewBoxY;
		long viewBoxW;
		long viewBoxH;
		
		int lastSearchIndex;
		
		TreeIndex treeIndex;
		PathGeneratorStore pathGeneratorStore;
		
		static wxString globaGeneratedPath;
		
		///////////////////////////////////////////////////////////////////
		void selectProcessInfo();
		void selectPreview();
		void selectResult();
		
		///////////////////////////////////////////////////////////////////
		void clearProcessInfo();
		void appendInfoMessage(const wxString& m);
		void appendWarningMessage(const wxString& m);
		void appendErrorMessage(const wxString& m);
		
		///////////////////////////////////////////////////////////////////
		void initControls();
		void closeWindow();
		void updatePreview();
		int getPathSelection();
		int getPathSelection(const wxString& item);
		void decorateSizeButton();
		void decorateTreeSizeButton();
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
		void evaluateValues();
		void evaluateCommonValues(int id);
		void evaluateSvgParameterValues(int id);
		void evaluateCncParameterValues(int id);
		void evaluatePathParameterValues(int id);
		
		///////////////////////////////////////////////////////////////////
		void setupPreDefinfedValues(int id, const wxString& name);
		
		///////////////////////////////////////////////////////////////////
		wxTreeItemId searchFirstTreeItem(wxTreeItemId parent, const wxString& label, bool fullMatch = true);
		
		///////////////////////////////////////////////////////////////////
		int searchAndSelectFirstTreeItem(const wxString& search = _T(""));
		int searchAndSelectNextTreeItem(const wxString& search = _T(""));
		int searchAndSelectTreeItem(const wxString& search, const int startIndex, bool fullMatch=true, const unsigned int minCharMatchCount=3);
		
		///////////////////////////////////////////////////////////////////
		void generatePath();
		
	protected:
    virtual void saveTemplate(wxCommandEvent& event);
   

		// user events
		virtual void updatePreview(wxCommandEvent& event);
		virtual void selectEditMode(wxCommandEvent& event);
		virtual void selectDisplayMode(wxCommandEvent& event);
		virtual void onShow(wxShowEvent& event);
		virtual void checkAutoGenerate(wxCommandEvent& event);
		virtual void pgMainBookChanged(wxNotebookEvent& event);
		virtual void propertyChanging(wxPropertyGridEvent& event);
		virtual void keyDownTreeSearch(wxKeyEvent& event);
		virtual void onMaximize(wxMaximizeEvent& event);
		virtual void treeSearch(wxCommandEvent& event);
		virtual void updateTreeSearch(wxCommandEvent& event);
		virtual void mainShashPositionChanged( wxSplitterEvent& event);
		virtual void mainShashPositionChanging( wxSplitterEvent& event);
		virtual void toogleTreeSize(wxCommandEvent& event);
		virtual void selectTemplateTree(wxTreeEvent& event);
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
