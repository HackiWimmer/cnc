#ifndef CNC_LARGE_SCALED_LIST_CTRL_H
#define CNC_LARGE_SCALED_LIST_CTRL_H

#include <vector>
#include <wx/arrstr.h>
#include <wx/listctrl.h>

//////////////////////////////////////////////////////////////////////
class CncColumContainer {
	
	public:
		/////////////////////////////////////////////////////////////
		CncColumContainer()
		: as()
		{
		}

		/////////////////////////////////////////////////////////////
		CncColumContainer(unsigned int cc)
		: as()
		{
			as.Add("", cc);
		}
		
		/////////////////////////////////////////////////////////////
		CncColumContainer(const CncColumContainer& cc)
		: as(cc.getStringArray())
		{
		}
		
		/////////////////////////////////////////////////////////////
		virtual ~CncColumContainer() {
			
		}
		
		/////////////////////////////////////////////////////////////
		unsigned int getCount() const { return as.GetCount(); }
		
		/////////////////////////////////////////////////////////////
		bool updateItem(unsigned int index, const wxString& data) {
			if ( index > getCount() - 1 )
				return false;
				
			as[index].assign(data);
			return true;
		}
		
		/////////////////////////////////////////////////////////////
		const wxString& getItem(unsigned int index) const {
			if ( index > getCount() - 1 )
				return _("");
				
			return as[index];
		}
		
		/////////////////////////////////////////////////////////////
		const wxArrayString& getStringArray() const {
			return as;
		}
		
		/////////////////////////////////////////////////////////////
		void trace(wxString& out) const {
			for ( unsigned int i=0; i<getCount(); i++ ) {
				out.append(as[i]);
				
				if ( i == getCount() -1 )
					break;
					
				out.append(", ");
			}
			out.append('\n');
		}
		
	private:
		wxArrayString as;
};

//////////////////////////////////////////////////////////////////////
class CncLargeScaledListCtrl : public wxListCtrl {
	
	public:
		enum ListType { NORMAL = 0, REVERSE = 1 };
		
		CncLargeScaledListCtrl(wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		                       long style=wxLC_REPORT | wxLC_VIRTUAL, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxListCtrlNameStr);
		
		CncLargeScaledListCtrl(wxWindow *parent, long style);
		
		virtual ~CncLargeScaledListCtrl();
		
		// interface
		bool clear();
		bool appendItem(const CncColumContainer& cc);
		bool appendItems(unsigned int nsize, const CncColumContainer* cc);
		
		long getItemCount() { return rows.size(); }
		
		const CncColumContainer& getRow(long item);
		
		void setListType(CncLargeScaledListCtrl::ListType lt) 	{ listType = lt; }
		CncLargeScaledListCtrl::ListType getListType() 			{ return listType; }
		
		long getLastSelection() { return lastSelection; }
		bool setLastSelection(long item);
		bool selectItem(long item, bool ensureVisible=true);
		
		bool goForwardUnitlColumnChange(long item, long column);
		bool goBackwardUnitlColumnChange(long item, long column);
		
	private:
		
		typedef std::vector<CncColumContainer> 	RowContainer;
		
		virtual wxString OnGetItemText(long	item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr *OnGetItemAttr(long item) const;
		
		inline bool isItemValid(long item) const;
		inline bool isColumnValid(long column) const;
		inline long translateItem(long item) const;
		
		bool goForwardUnitlColumnChangeIntern(long item, long column);
		bool goBackwardUnitlColumnChangeIntern(long item, long column);
		
		CncColumContainer* createDummyRow(long item);
		
		
		CncColumContainer* dummyRow;
		RowContainer rows;
		ListType listType;
		long lastSelection;
};

#endif
