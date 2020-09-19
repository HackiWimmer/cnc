#ifndef CNC_GAMEPAD_CMD_HISTORY_CTRL_H
#define CNC_GAMEPAD_CMD_HISTORY_CTRL_H

#include <vector>
#include "CncLargeScaleListCtrl.h"

class CncGamepadCommadHistoryListCtrl : public CncLargeScaledListCtrl {
		
	private:
		
		struct CmdInfo {
			int 		type;
			int			result;
			wxString	command;
			wxString	parameter;
		};
		
		typedef std::vector<CmdInfo> CmdList;
		CmdList 		commands;
		wxListItemAttr	defaultItemAttr;
		wxListItemAttr	errorItemAttr;
		
		virtual wxString OnGetItemText(long item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);
		
	protected:
		virtual bool isItemValid(long item) const;
		void addCommand(int type, int result, const wxString& cmd, const wxString& para);

	public:
		
		static const int COL_TYP 					=  0;
		static const int COL_RESULT					=  1;
		static const int COL_COMMAND				=  2;
		static const int COL_PARAMETER				=  3;
		static const int TOTAL_COL_COUNT			=  4;
		
		CncGamepadCommadHistoryListCtrl(wxWindow *parent, long style);
		virtual ~CncGamepadCommadHistoryListCtrl();
		
		void addStartCommand(bool result);
		void addStopCommand(bool result);
		void addUpdateCommand(bool result, const wxString& para);
		
		void clearAll();
		
		wxDECLARE_NO_COPY_CLASS(CncGamepadCommadHistoryListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif