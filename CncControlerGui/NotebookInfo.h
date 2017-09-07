#ifndef NOTBOOK_INFO_H 
#define NOTBOOK_INFO_H

#include <vector>
#include <wx/notebook.h>

struct PageInfo {
	wxIcon logo;
	wxString lable 	= "";
};

typedef std::vector<PageInfo> PageInfoList;


class NotebookInfo {
	
	private:
		wxNotebook* notebook;
		PageInfoList pageInfoList;
		
		///////////////////////////////////////////////////////////////////
		void evaluate() {
			if ( notebook == NULL )
				return;
			
			pageInfoList.clear();
			for ( unsigned int i=0; i<notebook->GetPageCount(); i++ ) {
				PageInfo pi;
				pi.lable = notebook->GetPageText(i);
				unsigned int idx = notebook->GetPageImage(i);
				if ( idx > 0 ) {
					wxImageList* il = notebook->GetImageList();
					if ( il != NULL ) 
						pi.logo = il->GetIcon(idx);
				}
				pageInfoList.push_back(pi);
			}
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		NotebookInfo(wxNotebook* nb)
		: notebook (nb)
		{
			evaluate();
		}
		
		///////////////////////////////////////////////////////////////////
		~NotebookInfo() {
			pageInfoList.clear();
		}
		
		///////////////////////////////////////////////////////////////////
		void decorate(unsigned int idx) {
			if ( notebook == NULL )
				return;
				
			if ( idx > notebook->GetPageCount() )
				return;
				
			PageInfo pi = pageInfoList.at(idx);
			
			unsigned int ii = notebook->GetImageList()->Add(pi.logo);
			notebook->SetPageImage(idx, ii);
			notebook->SetPageText(idx, pi.lable);
		}
};

#endif