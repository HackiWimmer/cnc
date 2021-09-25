#ifndef CNCFILEVIEW_H
#define CNCFILEVIEW_H

#include <vector>
#include <wx/dir.h>
#include "CncFileViewCallback.h"
#include "wxCrafterFileView.h"

typedef std::vector<wxString> ExtFilterList;

class CncFileViewListCtrl;
class CncFileView	: public CncFileViewBase
					, public wxDirTraverser
					, public CncFileViewCallback::Interface
{
	
	protected:
		virtual void selectOpenTemplate(wxCommandEvent& event);
		virtual void selectNewTemplate(wxCommandEvent& event);
		virtual void fileListLeave(wxMouseEvent& event);
		virtual void selectFilter(wxCommandEvent& event);
		virtual void aFolderUp(wxCommandEvent& event);
		virtual void refresh(wxCommandEvent& event);
		virtual void selectDefault(wxCommandEvent& event);
		virtual void fileListActivated(long item);
		virtual void fileListSelected(long item);
		
		friend CncFileViewListCtrl;
	
	public:
		CncFileView(wxWindow* parent, bool staticDir);
		virtual ~CncFileView();
		
		virtual wxDirTraverseResult OnFile(const wxString& filename);
		virtual wxDirTraverseResult OnDir(const wxString& dirname);
		
		virtual bool Enable(bool enable=true);
		
		const wxString getCurrentDirectory() const	{ return m_currentDirectory->GetValue(); }
		CncFileViewListCtrl* getFileView() const	{ return fileList; }

		bool openDirectory(const wxString& dirName);
		bool selectFileInList(const wxString& fileName);
		
		void setDefaultPath(const wxString& dirname);
		void selectDefaultPath();
		
		void update();
		
	protected:
		wxString 				defaultPath;
		CncFileViewListCtrl*	fileList;
		ExtFilterList 			filterList;
		bool					staticDir;

		bool makePathValid(wxString& p);
		bool openFile(const wxString& fileName);
		bool previewFile(const wxString& fileName);
		
	private:
		
		bool avoidSelectListEvent;
		wxString lastSelection;
};

class CncTransferFileView : public CncFileView {
	
	public:
		CncTransferFileView(wxWindow* parent, bool staticDir);
		virtual ~CncTransferFileView();
};

#endif // CNCFILEVIEW_H
