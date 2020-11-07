#ifndef CNC_SETTER_LIST_CTRL_H
#define CNC_SETTER_LIST_CTRL_H

#include <vector>
#include "CncArduino.h"
#include "CncCommon.h"
#include "CncLargeScaleListCtrl.h"

class CncSetterListCtrl : public CncLargeScaledListCtrl {
	
	private:
	
		struct SetterEntry {
			unsigned char 			pid	= PID_UNKNOWN;
			cnc::SetterValueList	values;
			wxDateTime 				time;
			
			SetterEntry(unsigned char p, const cnc::SetterValueList& v) 
			: pid(p)
			, values(v)
			, time(wxDateTime::UNow())
			{}
		};
		
		typedef std::vector<SetterEntry> SetterEntries;
		SetterEntries			setterEntries;
		
		wxTextCtrl*				selSetterNum;
		wxTextCtrl*				selSetterPid;
		wxTextCtrl*				selSetterKey;
		wxTextCtrl*				selSetterValue;
		wxStaticText*			selSetterUnit;
		
		wxListItemAttr			defaultItemAttr;
		wxListItemAttr			separatorRunItemAttr;
		wxListItemAttr			separatorResetItemAttr;
		wxListItemAttr			separatorSetupItemAttr;
		wxTimer 				displayTimer;
		int						displayTimerInterval;
		
		long					translateItem(long item) const;
		virtual bool			isItemValid(long item) const;
		
		virtual wxString		OnGetItemText(long item, long column) const;
		virtual int 			OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
		void					onPaint(wxPaintEvent& event);
		void					onSize(wxSizeEvent& event);
		void					onDisplayTimer(wxTimerEvent& event);
		void 					onSelectListItem(wxListEvent& event);
		void 					onActivateListItem(wxListEvent& event);

		
	public:
		static const int COL_TYPE 		= 0;
		static const int COL_NUM 		= 1;
		static const int COL_PID 		= 2;
		static const int COL_KEY 		= 3;
		static const int COL_VAL 		= 4;
		static const int COL_UNIT 		= 5;
		
		static const int TOTAL_COL_COUNT	= 6;
		
		CncSetterListCtrl(wxWindow *parent, long style);
		virtual ~CncSetterListCtrl();
		
		void addSetter(unsigned char pid, const cnc::SetterValueList& v);
		void clearAll();

		void setSelNumCtrl  (wxTextCtrl* c)		{ selSetterNum		= c; }
		void setSelPidCtrl  (wxTextCtrl* c)		{ selSetterPid		= c; }
		void setSelKeyCtrl  (wxTextCtrl* c)		{ selSetterKey		= c; }
		void setSelValCtrl  (wxTextCtrl* c)		{ selSetterValue	= c; }
		void setSelUnitCtrl (wxStaticText* c)	{ selSetterUnit		= c; }
		
		void updateColumnWidth();
		
		wxDECLARE_NO_COPY_CLASS(CncSetterListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif