#ifndef CNC_SERIAL_SPY_LIST_CTRL_H
#define CNC_SERIAL_SPY_LIST_CTRL_H

#include <wx/event.h>
#include <wx/timer.h>
#include "OSD/CncTimeFunctions.h"
#include "CncLargeScaleListCtrl.h"

wxDECLARE_EVENT(wxEVT_SERIAL_TIMER, wxTimerEvent);

class CncSerialSpyListCtrl : public CncLargeScaledListCtrl {
	
	private:
		static const int refreshInterval = 1000;
				
		wxListItemAttr itemAttrDefault;
		wxListItemAttr itemAttrResultOk;
		wxListItemAttr itemAttrResultMore;
		wxListItemAttr itemAttrResultError;
		wxListItemAttr itemAttrResultWarning;
		wxListItemAttr itemAttrResultLimit;
		wxListItemAttr itemAttrResultHalt;
		wxListItemAttr itemAttrResultQuit;
		wxListItemAttr itemAttrResultInterrupt;
		wxListItemAttr itemAttrResultMarker;
		wxListItemAttr itemAttrResultEnable;
		wxListItemAttr itemAttrResultDisable;
		wxListItemAttr itemAttrResultCommand;
		
		wxTimer serialTimer;
		long lastItemCount;
		CncMilliTimestamp tsLast;
		 
		void refreshList();
		void startRefreshInterval() { serialTimer.Start(refreshInterval); }
		void decodeSerialSpyLine(const wxString& line, bool displayInfo = true); 
		
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		 
	public:
		static const int COL_TYPE 			=  0;
		static const int COL_NUM 			=  1;
		static const int COL_LINE 			=  2;
		static const int TOTAL_COL_COUNT	=  3;
		
		enum LineType {	LT_ResultOk			=  1,
						LT_ResultMore		=  2,
						LT_ResultError		=  3,
						LT_ResultWarning	=  4,
						LT_ResultLimit		=  5,
						LT_ResultHalt		=  6,
						LT_ResultQuit		=  7,
						LT_ResultInterrupt	=  8,
						
						LT_Separator		= 10,
						LT_Info				= 11,
						LT_Marker			= 12,
						LT_Enable			= 13,
						LT_Disable			= 14,
						LT_Time				= 15,
						LT_Command			= 16,
						
						LT_Default 			=  0
		};
		
		CncSerialSpyListCtrl(wxWindow *parent, long style);
		virtual ~CncSerialSpyListCtrl();
		
		void flush();
		
		void addLine(const wxString& line, const LineType lt = LT_Default);
		void clearDetails();
		void updateColumnWidth();
		
		void onSize(wxSizeEvent& event);
		void onTimer(wxTimerEvent& event);
		void onSelectList(wxListEvent& event);
		
		wxDECLARE_NO_COPY_CLASS(CncSerialSpyListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif
