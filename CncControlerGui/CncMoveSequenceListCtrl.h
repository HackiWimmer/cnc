#ifndef CNC_MOVE_SEQUENCE_LIST_CTRL_H
#define CNC_MOVE_SEQUENCE_LIST_CTRL_H

#include <vector>
#include "OSD/CncTimeFunctions.h"
#include "CncMoveSequence.h"
#include "CncLargeScaleListCtrl.h"

class wxStaticText;

//////////////////////////////////////////////////////////////////
class CncMoveSequenceListCtrl : public CncLargeScaledListCtrl {

	public:

		struct SpeedInfo {
			char mode 		= '-';
			double value	= 0.0;
		};

	private:

	/*
		struct RowInfo  {
			enum Type { RIT_NULL = 0, RIT_ID = 1, RIT_SPEED = 2, RIT_POS = 3 };

			Type 			type;
			CncMoveSequence seq;
			SpeedInfo 		speed;

			RowInfo(Type t)
			: type(t)
			, seq()
			, speed()
			{}

			void appendTo(CncMoveSequenceListCtrl* ctrl);
			const wxString& onGetColumnText(wxString& ret, long column) const;
		};
	*/
	
		bool active;
		const CncMoveSequence* moveSequence;
		
		wxListItemAttr defaultItemAttr;
		wxListItemAttr initialItemAttr;
		wxListItemAttr clientIdItemAttr;
		wxListItemAttr speedItemAttr;
		
		wxStaticText* infoText;

		virtual wxString OnGetItemText(long item, long column) const;
		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
		void updateColumnWidth();

		void onPaint(wxPaintEvent& event);
		void onSize(wxSizeEvent& event);
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);

	public:

		static const int COL_TYPE 			=  0;
		static const int COL_SPEED			=  1;
		static const int COL_CLD_ID			=  2;
		static const int COL_DISTANCE_X		=  3;
		static const int COL_DISTANCE_Y		=  4;
		static const int COL_DISTANCE_Z		=  5;

		static const int TOTAL_COL_COUNT	=  6;
		static const int COL_SEARCH			= COL_CLD_ID;
		static const int COL_STRECH			= COL_CLD_ID;

		CncMoveSequenceListCtrl(wxWindow *parent, long style);
		virtual ~CncMoveSequenceListCtrl();

		void addMoveSequence(const CncMoveSequence* seq, const SpeedInfo& si);
		void setActive(bool state) { active = state; }

		void clearAll();

		bool searchReference(const wxString& what);
		bool searchReferenceById(const long id);

		wxDECLARE_NO_COPY_CLASS(CncMoveSequenceListCtrl);
		wxDECLARE_EVENT_TABLE();
};

//////////////////////////////////////////////////////////////////
class CncMoveSequenceOverviewListCtrl : public CncLargeScaledListCtrl {

	private:
		typedef std::vector<CncMoveSequence> MoveSequences;
		MoveSequences 				moveSequences;
		CncMoveSequenceListCtrl* 	slaveSequenceList;
		wxStaticText*				contentLabel;
		
		virtual wxString OnGetItemText(long item, long column) const;
		virtual bool isItemValid(long item) const;
		
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);

 	public:
		static const int COL_NUM 			=  0;
		static const int COL_CNT 			=  1;
		static const int COL_REF 			=  2;
		static const int COL_CLD_ID			=  3;
		
		static const int TOTAL_COL_COUNT	=  4;
		
		CncMoveSequenceOverviewListCtrl(wxWindow *parent, long style, CncMoveSequenceListCtrl* slave, wxStaticText*	label);
		virtual ~CncMoveSequenceOverviewListCtrl();
		
		void clearAll();
		void addMoveSequence(const CncMoveSequence& seq);
		
		wxDECLARE_NO_COPY_CLASS(CncMoveSequenceOverviewListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif
