#ifndef CNCOPENDIALOGLRULIST_H_
#define CNCOPENDIALOGLRULIST_H_

#include <vector>
#include <wx/listctrl.h>
#include <wx/filedlg.h>

class CncFileDialog;

///////////////////////////////////////////////////////////////////////////
class CncFileDialogLRUList : public wxListCtrl {

	private:
		long lastSelection;

		static const int COL_FILE			=  0;
		static const int TOTAL_COL_COUNT	=  1;

		void closeParentDialog(int code);
		bool processSelectedItem();
		
		CncFileDialog* getFileDialog();

	public:

		CncFileDialogLRUList(wxWindow *parent);
		~CncFileDialogLRUList();

		void onSelectListItem(wxListEvent& event);
		void onLeftDClick(wxMouseEvent& event);
		void onKeyDown(wxKeyEvent& event);

		void addFileName(const wxString& fn);

		wxDECLARE_NO_COPY_CLASS(CncFileDialogLRUList);
		wxDECLARE_EVENT_TABLE();
};

///////////////////////////////////////////////////////////////////////////
class CncFileDialogExtraPanel : public wxPanel {

	public:
		CncFileDialogExtraPanel(wxWindow *parent);
		virtual ~CncFileDialogExtraPanel();
		
		void addFileName(const wxString& fn);
		
	private:
		CncFileDialogLRUList* lruListCtrl;
};


///////////////////////////////////////////////////////////////////////////
class CncFileDialog : public wxFileDialog {
	
	public:
	
		typedef std::vector<wxString> LruList;
	
		CncFileDialog (
			wxWindow *		  	parent,
			const wxString &  	message = wxFileSelectorPromptStr,
			const wxString &  	defaultDir = wxEmptyString,
			const wxString &  	defaultFile = wxEmptyString,
			const wxString &  	wildcard = wxFileSelectorDefaultWildcardStr,
			long  				style = wxFD_DEFAULT_STYLE,
			const wxPoint &  	pos = wxDefaultPosition,
			const wxSize &  	size = wxDefaultSize,
			const wxString &  	name = wxFileDialogNameStr 
		);
		
		virtual ~CncFileDialog();
		virtual int ShowModal();
		
		void addLruFilename(const wxString& fn);
		
		unsigned int getLruFileCount() 				{ return lruList.size(); }
		const LruList& getLruListContent() 			{ return lruList; }
		
		static wxWindow* createExtraPanelForFileDialog(wxWindow* parent);
		
	private:
		
		LruList lruList;
		
};

#endif 
