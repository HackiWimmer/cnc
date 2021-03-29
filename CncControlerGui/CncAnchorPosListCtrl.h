#ifndef CNC_ANCHOR_POS_LIST_H
#define CNC_ANCHOR_POS_LIST_H

#include <map>
#include "CncAnchorInfo.h"
#include "CncLargeScaleListCtrl.h"

class CncAnchorPosListCtrl : public CncLargeScaledListCtrl {
	
	public:
	
		struct AnchorInfo : public CncAnchorInfo {
			
			protected:
				int					index		= -1;
				
			friend CncAnchorPosListCtrl;
		};
		
		typedef std::map<wxString, AnchorInfo> AnchorMap;
		AnchorMap anchorMap;
		
		class Interface {
			
			public:
				Interface() 
				{}
				
				virtual ~Interface() 
				{}
				
				virtual void notifySelection(const AnchorInfo& ai) = 0;
				virtual void notifyActivation(const AnchorInfo& ai) = 0;
		};
		
	private:
		
		typedef std::vector<wxString> AnchorIndex;
		AnchorIndex anchorIdx;
		
		Interface* caller;
		
		wxListItemAttr defaultItemAttr;
		wxListItemAttr defaultItemAttrSelected;
		
		virtual wxString OnGetItemText(long item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);
		
		void updateIndex();
		
	protected:
		
		virtual bool isItemValid(long item) const;
		
	public:
		
		static const int COL_MODE 					=  0;
		static const int COL_NAME 					=  1;
		static const int COL_SHOW	 				=  2;
		static const int COL_TYPE	 				=  3;
		static const int COL_X						=  4;
		static const int COL_Y 						=  5;
		static const int COL_Z	 					=  6;
		static const int TOTAL_COL_COUNT			=  7;
		
		CncAnchorPosListCtrl(wxWindow *parent, long style);
		virtual ~CncAnchorPosListCtrl();
		
		bool hasAnchor(const wxString& name) const;
		bool isAnchorFixed(const wxString& name) const;

		void addAnchor(const AnchorInfo& ai);
		void modAnchor(const AnchorInfo& ai);
		void delAnchor(const wxString& name);
		
		void selAnchor(const wxString& name);
		
		void setObserver(Interface* o) { caller = o; }
		
		const AnchorMap& getAnchorMap() const { return anchorMap; }
		
		wxDECLARE_NO_COPY_CLASS(CncAnchorPosListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif