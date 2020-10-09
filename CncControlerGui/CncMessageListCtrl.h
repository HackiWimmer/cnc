#ifndef CNC_MESSAGE_LIST_CTRL_H
#define CNC_MESSAGE_LIST_CTRL_H

#include <wx/datetime.h>
#include <wx/timer.h>
#include "CncLargeScaleListCtrl.h"

class CncMessageListCtrl : public CncLargeScaledListCtrl {
	
	private:
		
		struct Message {
			
			char type = '\0';
			wxDateTime ts;
			wxString message;
			wxString context;
			
			Message(const char t, const wxString& m) {
				ts   = wxDateTime::UNow();
				type = t;
				message.assign(m);
				context.assign("");
			}

			Message(const char t, const wxString& m, const wxString& c) {
				ts   = wxDateTime::UNow();
				type = t;
				message.assign(m);
				context.assign(c);
			}
		};
		
		typedef std::vector<Message> MesageList;
		MesageList messages;
		
		int 			updateInterval;
		wxTimer			displayTimer;
		wxListItemAttr	itemAttrInfo;
		wxListItemAttr	itemAttrWarning;
		wxListItemAttr	itemAttrError;
		wxListItemAttr	itemAttrDebug;
		wxListItemAttr	itemAttrSeparator;
		
		virtual wxString OnGetItemText(long item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr *OnGetItemAttr(long item) const;

		void onDisplayTimer(wxTimerEvent& event);
		void onSize(wxSizeEvent& event);
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);
		
	protected:
		
		virtual bool isItemValid(long item) const;
		void updateColumnWidth();
	
	public:
		
		static const int COL_TIM			= 0;
		static const int COL_TYP			= 1;
		static const int COL_MSG			= 2;
		static const int COL_CTX			= 3;
		
		static const int TOTAL_COL_COUNT	= 4;
		static const int COL_STRECH			= COL_MSG;
		
		CncMessageListCtrl(wxWindow *parent, long style);
		virtual ~CncMessageListCtrl();
		
		void setUpdateInterval(int value);
		void appendMessage(const char type, const wxString& message);
		void appendMessage(const char type, const wxString& message, const wxString& context);
		void clear();
		
		wxDECLARE_NO_COPY_CLASS(CncMessageListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif
