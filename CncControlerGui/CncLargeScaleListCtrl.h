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
		: initialized(false)
		, columns()
		{
		}
		
		/////////////////////////////////////////////////////////////
		CncColumContainer(unsigned int cc)
		: initialized(true)
		, columns()
		{
			columns.Add("", cc);
		}
		
		/////////////////////////////////////////////////////////////
		CncColumContainer(const CncColumContainer& cc)
		: initialized(cc.isInitialized())
		, columns(cc.getStringArray())
		{
		}
		
		/////////////////////////////////////////////////////////////
		virtual ~CncColumContainer() {
			columns.Clear();
		}
		
		/////////////////////////////////////////////////////////////
		unsigned int initColumnCount(unsigned int cc) {
			if ( isInitialized() == false )
				columns.Add("", cc);
				
			return columns.GetCount();
		}
		
		/////////////////////////////////////////////////////////////
		bool isInitialized() const { return initialized; }
		
		/////////////////////////////////////////////////////////////
		unsigned int getCount() const { return columns.GetCount(); }
		
		/////////////////////////////////////////////////////////////
		bool updateItem(unsigned int index, const wxString& data) {
			if ( index > getCount() - 1 )
				return false;
				
			columns[index].assign(data);
			return true;
		}
		
		/////////////////////////////////////////////////////////////
		const wxString& getItem(unsigned int index) const {
			if ( index > getCount() - 1 )
				return _("");
				
			return columns[index];
		}
		
		/////////////////////////////////////////////////////////////
		const wxArrayString& getStringArray() const {
			return columns;
		}
		
		/////////////////////////////////////////////////////////////
		void trace(wxString& out) const {
			for ( unsigned int i=0; i<getCount(); i++ ) {
				out.append(columns[i]);
				
				if ( i == getCount() -1 )
					break;
					
				out.append(", ");
			}
			out.append('\n');
		}
		
	private:
		bool initialized;
		wxArrayString columns;
		
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
		
		void freeze();
		void thaw();
		
		bool clear();
		bool appendItem(const CncColumContainer& cc);
		bool appendItems(unsigned int nsize, const CncColumContainer* cc);
		
		bool updateItem(long item, const CncColumContainer& cc);
		bool updateItem(long item, long column, const wxString& value);

		long getItemCount() { return rows.size(); }
		
		const CncColumContainer& getRow(long item);
		
		void setListType(CncLargeScaledListCtrl::ListType lt) 	{ listType = lt; }
		CncLargeScaledListCtrl::ListType getListType() 			{ return listType; }
		
		long getLastSelection() { return lastSelection; }
		bool setLastSelection(long item);
		bool selectItem(long item, bool ensureVisible=true);
		void deselectAll();
		
		bool goForwardUnitlColumnChange(long item, long column);
		bool goBackwardUnitlColumnChange(long item, long column);
		
	protected:
		
		bool blockSelectionEvent;
		class SelectEventBlocker {
			private:
				CncLargeScaledListCtrl* list;
				
			public:
				SelectEventBlocker(CncLargeScaledListCtrl* l) : list(l) { if ( list ) list->blockSelectionEvent = true; }
				~SelectEventBlocker() 									{ if ( list ) list->blockSelectionEvent = false; }
		};
		
		wxString getItemText(long item, long column) const;
		long searchRow(const wxString& what, int searchColumn);
		
		virtual bool isItemValid(long item) const;
	
	private:
		
		typedef std::vector<CncColumContainer> 	RowContainer;
			
		virtual wxString OnGetItemText(long	item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		virtual wxListItemAttr* OnGetItemColumnAttr(long item, long column ) const;
		
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
