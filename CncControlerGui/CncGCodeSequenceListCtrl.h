#ifndef CNC_GCODE_SEQUENCE_LIST_CTRL_H
#define CNC_GCODE_SEQUENCE_LIST_CTRL_H

#include "GCodeBlock.h"
#include "CncLargeScaleListCtrl.h"

class CncGCodeSequenceListCtrl : public CncLargeScaledListCtrl {
	
	private:
		wxListItemAttr	defaultItemAttr;
		wxListItemAttr	defaultItemAttrSelected;
		
		typedef std::vector<GCodeBlock> GCodes;
		GCodes			gcodes;
		
		virtual bool isItemValid(long item) const;
		
		virtual wxString OnGetItemText(long item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
	public:
		static const int COL_ID				= 0;
		static const int COL_CMD			= 1;
		static const int COL_X				= 2;
		static const int COL_Y				= 3;
		static const int COL_Z				= 4;
		static const int COL_F				= 5;
		static const int COL_S				= 6;
		static const int COL_MORE			= 7;
		
		static const int TOTAL_COL_COUNT	= 8;
		static const int COL_SEARCH			= COL_ID;
		
		CncGCodeSequenceListCtrl(wxWindow *parent, long style);
		virtual ~CncGCodeSequenceListCtrl();
		
		void clearAll();
		
		void addBlock(const GCodeBlock& gcb);
		
		void onPaint(wxPaintEvent& event);
		
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);
		
		bool searchReference(const wxString& what);
		bool searchReferenceById(const long id);

		wxDECLARE_NO_COPY_CLASS(CncGCodeSequenceListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif