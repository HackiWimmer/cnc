#ifndef CNC_POS_SPY_LIST_CTRL_H
#define CNC_POS_SPY_LIST_CTRL_H

#include <vector>
#include "CncCommon.h"
#include "CncArduino.h"
#include "CncPosition.h"
#include "CncArduino.h"
#include "CncLargeScaleListCtrl.h"

class CncPosSpyListCtrl : public CncLargeScaledListCtrl {
	
	public:
		enum PosType { PT_APP, PT_CTL };
		
	private:
		
		struct PositionEntry {
			unsigned char pid				= PID_UNKNOWN;
			long id							= -1;
			char speedMode					= cnc::USER_DEFIND_SPEED_CHAR;
			double configuredSpeedValue		= 0.0;
			double currentSpeedValue		= 0.0;
			CncLongPosition pos				= {0, 0, 0};
			
			PositionEntry(unsigned char p, long i, char sm, double cfgSpeedValue, double curSpeedValue, const CncLongPosition& pos)
			: pid					(p)
			, id					(i)
			, speedMode 			(sm)
			, configuredSpeedValue	(cfgSpeedValue)
			, currentSpeedValue		(curSpeedValue)
			, pos					(pos)
			{}
			
		};
		
		typedef std::vector<PositionEntry> PositionEntries;
		
		PosType				posType;
		CncUnit				cncUnit;
		PositionEntries 	appPositionEntries;
		PositionEntries 	ctlPositionEntries;
		double 				displayFactX;
		double 				displayFactY;
		double 				displayFactZ;
		wxString			posFormat;
		wxString 			majorPosPid;
		wxListItemAttr 		majorPosItemAttr;
		wxTimer 			displayTimer;
		int					displayTimerInterval;
		
		long translateItem(long item) const;
		virtual bool isItemValid(long item) const;

		virtual wxString OnGetItemText(long item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr *OnGetItemAttr(long item) const;
		
		void onPaint(wxPaintEvent& event);
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);
		void onDisplayTimer(wxTimerEvent& event);
		
	public:
	
		static const int COL_PID			= 0;
		static const int COL_REF			= 1;
		static const int COL_T				= 2;
		static const int COL_F				= 3;
		static const int COL_X				= 4;
		static const int COL_Y				= 5;
		static const int COL_Z				= 6;
		
		static const int TOTAL_COL_COUNT	= 7;
		
		static const int COL_SEARCH			= COL_REF;
		
		CncPosSpyListCtrl(wxWindow *parent, long style);
		virtual ~CncPosSpyListCtrl();
		
		void setUnit(CncUnit u);
		void setPositionType(CncPosSpyListCtrl::PosType pt);
		void addAppPosition(unsigned char pid, long id, char speedMode, double cfgSpeedValue, double curSpeedValue, const CncLongPosition& pos);
		void addCtlPosition(unsigned char pid, long id, char speedMode, double cfgSpeedValue, double curSpeedValue, const CncLongPosition& pos);
		void clearAll();
		
		bool searchReference(const wxString& what);
		bool searchReferenceById(const long id);
		
		wxDECLARE_NO_COPY_CLASS(CncPosSpyListCtrl);
		wxDECLARE_EVENT_TABLE();

};

#endif