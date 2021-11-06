#ifndef CNC_SECURE_PORT_LIST_CTRL_H
#define CNC_SECURE_PORT_LIST_CTRL_H

#include "CncLargeScaleListCtrl.h"

class CncSecurePortListCtrl : public CncLargeScaledListCtrl {
	
	public:
		enum PortListImage {
			PTI_CONNECTED 		=  0,
			PTI_AVAILABLE  		=  1,
			PTI_ACCESS_DENIED 	=  2,
			PTI_UNKNOWN			=  3,
			PTI_EMPTY			= -1
		};
		
		const PortListImage getImgaeIndex(const wxString& name);
		
		struct PortEntry {
			PortListImage	imageIdx;
			wxString		portName;
			
			PortEntry(const wxString& n, PortListImage i) 
			: imageIdx(i)
			, portName(n)
			{}
		};
		
		typedef std::vector<PortEntry> PortEntryList;
	
	private:
		
		PortEntryList		portEntries;
		CncSecureCtrlPanel*	securePanel;
		
		wxListItemAttr		defaultItemAttr;
		wxListItemAttr		selectedItemAttr;
		
		virtual wxString OnGetItemText(long item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		void onSize(wxSizeEvent& event);
		void onActivateListItem(wxListEvent& event);
		
	protected:
	
		// --------------------------------------------------------
		virtual bool isItemValid(long item) const {
			
			wxASSERT( GetItemCount() == portEntries.size() );
			
			if( item < 0 || item > (long)(portEntries.size() - 1) )
				return false;
				
			return true;
		}

	public:
		static const int COL_STATE 			= 0;
		static const int COL_NAME 			= 1;
		static const int TOTAL_COL_COUNT	= 2;
		static const int COL_STRECH			= COL_NAME;
		
		CncSecurePortListCtrl(wxWindow *parent, CncSecureCtrlPanel* panel, long style);
		virtual ~CncSecurePortListCtrl();

		void updateColumnWidth();
		void deleteAllEntries();
		void addPortEntry(const wxString& name, PortListImage pii) ;
		void addPortEntry(const wxString& name, const wxString& image);
		bool selectPortInList(const wxString& portName);
		
		wxDECLARE_NO_COPY_CLASS(CncSecurePortListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif