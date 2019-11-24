#ifndef CNCFILEVIEW_H
#define CNCFILEVIEW_H

#include <vector>
#include <wx/dir.h>
#include "wxCrafterFileView.h"

typedef std::vector<wxString> ExtFilterList;

class CncFileView : public CncFileViewBase, public wxDirTraverser {
	
	protected:
		virtual void selectOpenTemplate(wxCommandEvent& event);
		virtual void selectNewTemplate(wxCommandEvent& event);
		virtual void fileListLeave(wxMouseEvent& event);
		virtual void selectFilter(wxCommandEvent& event);
		virtual void aFolderUp(wxCommandEvent& event);
		virtual void refresh(wxCommandEvent& event);
		virtual void selectDefault(wxCommandEvent& event);
		virtual void fileListActivated(wxListEvent& event);
		virtual void fileListSelected(wxListEvent& event);
	
	public:
		CncFileView(wxWindow* parent);
		virtual ~CncFileView();
		
		virtual wxDirTraverseResult OnFile(const wxString& filename);
		virtual wxDirTraverseResult OnDir(const wxString& dirname);
		
		bool openDirectory(const wxString& dirName);
		bool selectFileInList(const wxString& fileName);
		
		void setDefaultPath(const wxString& dirname);
		void selectDefaultPath();
		
		void update();
		
	protected:
		wxString defaultPath;
		ExtFilterList filterList;

		bool makePathValid(wxString& p);
		bool openFile(const wxString& fileName);
		bool previewFile(const wxString& fileName);
		
	private:
		bool avoidSelectListEvent;
		wxString lastSelection;
};

#endif // CNCFILEVIEW_H
