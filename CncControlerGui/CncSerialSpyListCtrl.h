#ifndef CNC_SERIAL_SPY_LIST_CTRL_H
#define CNC_SERIAL_SPY_LIST_CTRL_H

#include <wx/event.h>
#include <wx/timer.h>
#include <wx/anybutton.h>
#include "HexDecoder.h"
#include "OSD/CncTimeFunctions.h"
#include "CncLargeScaleListCtrl.h"

wxDECLARE_EVENT(wxEVT_SPY_DISPLAY_TIMER, wxTimerEvent);

class CncSerialSpyListCtrl : public CncLargeScaledListCtrl {
	
	public:
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
						LT_DebugEntry		= 17,
						
						LT_Default 			=  0
		};
		
	private:
		
		// ---------------------------------------------------
		struct LineInfo {
			enum Type { LIT_UNKNOWN = 0, LIT_INBOUND = 1, LIT_OUTBOUND = 2 };
			
			Type		type;
			wxString	context;
			wxString	hexString;
			
			// ------------------------------------------
			static Type decodeType(const wxString& s) {
				//0123456789
				// Serial::< 
				switch ( (char)s[9] ) {
					case '>': return LineInfo::Type::LIT_OUTBOUND;
					case '<': return LineInfo::Type::LIT_INBOUND;
				}
				
				return LIT_UNKNOWN;
			}
		};
		
		const CncSerialSpyListCtrl::LineInfo& decodeLineInfo(const wxString& line, CncSerialSpyListCtrl::LineInfo& ret) const;
		
		// ---------------------------------------------------
		struct Entry {
			wxString line; 
			wxString appendix; 
			LineType lt;
			 
			Entry(const wxString& l, const LineType t)
			: line		(l)
			, appendix	("")
			, lt		(t)
			{}
			
			Entry(const wxString& l, const wxString& a, const LineType t)
			: line		(l)
			, appendix	(a)
			, lt		(t)
			{}
		};
		
		typedef std::vector<Entry> Entries;
		
		wxListItemAttr		itemAttrDefault;
		wxListItemAttr		itemAttrInbound;
		wxListItemAttr		itemAttrOutbound;
		wxListItemAttr		itemAttrResultOk;
		wxListItemAttr		itemAttrResultMore;
		wxListItemAttr		itemAttrResultError;
		wxListItemAttr		itemAttrResultWarning;
		wxListItemAttr		itemAttrResultDebug;
		wxListItemAttr		itemAttrResultLimit;
		wxListItemAttr		itemAttrResultHalt;
		wxListItemAttr		itemAttrResultQuit;
		wxListItemAttr		itemAttrResultInterrupt;
		wxListItemAttr		itemAttrResultMarker;
		wxListItemAttr		itemAttrResultEnable;
		wxListItemAttr		itemAttrResultDisable;
		wxListItemAttr		itemAttrResultCommand;
		
		Entries				entries;
		wxTimer				spyDisplaylTimer;
		int 				refreshInterval;
		wxAnyButton *		openDetails;
		bool				liveDecoding;
		bool				considerDebug;
		bool				autoScrolling;
		bool				autoColumnSizing;
		CncMilliTimestamp	tsLast;
		 
		void					refreshList();
		void					startRefreshInterval() { spyDisplaylTimer.Start(refreshInterval); }
		
		bool 					decodeSerialSpyLineIntern(long item, SpyHexDecoder::Details& details) const ;
		
		
		
		virtual bool 			isItemValid(long item) const;
		virtual int 			OnGetItemColumnImage(long item, long column) const;
		virtual wxString 		OnGetItemText(long item, long column) const;
		virtual wxListItemAttr*	OnGetItemAttr(long item) const;
		 
		void					onSize(wxSizeEvent& event);
		void					onTimer(wxTimerEvent& event);
		void					onSelectListItem(wxListEvent& event);
		void					onActivateListItem(wxListEvent& event);
		void					onEndDragList(wxListEvent& event);
		void					onKeyDown(wxKeyEvent& event);
		
	public:
		static const int COL_TYPE 			=  0;
		static const int COL_NUM 			=  1;
		static const int COL_LINE 			=  2;
		static const int COL_DECODED		=  3;
		static const int TOTAL_COL_COUNT	=  4;
		
		CncSerialSpyListCtrl(wxWindow *parent, long style);
		virtual ~CncSerialSpyListCtrl();
		
		void flush();
		
		const wxString 			getLine(long item) const;
		const wxString 			getSelectedLine() const;
		long 		 			getSelectedItem() const;
		
		void 					setRefreshInterval(int v)			{ refreshInterval = v; }
		
		void 					installOpenDetails(wxAnyButton* b)	{ openDetails = b; }
		void					enableLiveDecoding(bool state);
		void					enableAutoScrolling(bool state);
		void					enableAutoColumnSizing(bool state);
		void					enableDebugEntries(bool state);
		
		void 					addLine(const wxString& line, const LineType lt = LT_Default);
		void 					addLine(const wxString& line, const wxString& appendix, const LineType lt = LT_Default);
		
		void 					clearAll();
		void 					updateColumnWidth();
		bool 					copyToClipboard(bool allRows=false);
		
		const wxString&			decodeSerialSpyLine(long item, wxString& ret) const; 
		const wxString&			decodeSelectedSpyLine(wxString& ret) const; 
		
		const wxColor&			getSelectedSpyLineBgColour(wxColor& ret) const; 
		const wxColor&			getSelectedSpyLineFgColour(wxColor& ret) const; 
		const wxFont&			getSelectedSpyLineFont(wxFont& ret) const;
		bool					fitsTextIntoCell(long item, long col);
		bool					fitsDecodedTextForSelectedItem();
			
		wxDECLARE_NO_COPY_CLASS(CncSerialSpyListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif
