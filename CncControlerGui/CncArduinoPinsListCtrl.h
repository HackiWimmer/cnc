#ifndef CNC_ARDUINO_PINS_LIST_CTRL_H
#define CNC_ARDUINO_PINS_LIST_CTRL_H

#include <map>
#include <vector>
#include <wx/anybutton.h>
#include "ArduinoData.h"
#include "CncLargeScaleListCtrl.h"

class CncArduinoPinsListCtrl : public CncLargeScaledListCtrl {
		
	private:
		
		typedef std::map<wxString, unsigned int> PinIndex;
		PinIndex pinIndex;
		
		typedef std::vector<AE::PinData> PinList;
		PinList pins;
		
		virtual wxString OnGetItemText(long item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);
		
	protected:
		virtual bool isItemValid(long item) const;

	public:
		
		static const int COL_TYP 					=  0;
		static const int COL_NUM 					=  1;
		static const int COL_LABEL 					=  2;
		static const int COL_VAL 					=  3;
		static const int TOTAL_COL_COUNT			=  4;
		
		CncArduinoPinsListCtrl(wxWindow *parent, long style);
		virtual ~CncArduinoPinsListCtrl();
		
		void sort(wxAnyButton* sortButton);
		void updatePinValue(const char type, unsigned int name, const char mode, int value);
		
		wxDECLARE_NO_COPY_CLASS(CncArduinoPinsListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif