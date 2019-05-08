#ifndef CNC_MOVE_SEQUENCE_LIST_CTRL_H
#define CNC_MOVE_SEQUENCE_LIST_CTRL_H

#include <vector>
#include "OSD/CncTimeFunctions.h"
#include "CncMoveSequence.h"
#include "CncLargeScaleListCtrl.h"

class CncMoveSequenceListCtrl : public CncLargeScaledListCtrl {

	public:

		struct SpeedInfo {
			char mode 		= '-';
			double value	= 0.0;
		};

	private:

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
		};


		wxListItemAttr defaultItemAttr;
		wxListItemAttr initialItemAttr;
		wxListItemAttr clientIdItemAttr;
		wxListItemAttr speedItemAttr;

		typedef std::vector<RowInfo> MoveSequences;
		MoveSequences moveSequences;

		virtual int OnGetItemColumnImage(long item, long column) const;
		virtual wxListItemAttr* OnGetItemAttr(long item) const;
		
		void updateColumnWidth();

		void onSize(wxSizeEvent& event);
		void onSelectListItem(wxListEvent& event);
		void onActivateListItem(wxListEvent& event);

	public:

		static const int COL_TYPE 			=  0;
		static const int COL_REF 			=  1;
		static const int COL_SPEED			=  2;
		static const int COL_CLD_ID			=  3;
		static const int COL_DISTANCE_X		=  4;
		static const int COL_DISTANCE_Y		=  5;
		static const int COL_DISTANCE_Z		=  6;

		static const int TOTAL_COL_COUNT	=  7;
		static const int COL_SEARCH			= COL_CLD_ID;
		static const int COL_STRECH			= COL_REF;

		CncMoveSequenceListCtrl(wxWindow *parent, long style);
		virtual ~CncMoveSequenceListCtrl();

		void addMoveSequenceClietId(const CncMoveSequence& seq, const SpeedInfo& si);
		void addMoveSequencePositions(const CncMoveSequence& seq);

		void clearAll();
		void updateContent(bool force = false);

		bool searchReference(const wxString& what);
		bool searchReferenceById(const long id);

		wxDECLARE_NO_COPY_CLASS(CncMoveSequenceListCtrl);
		wxDECLARE_EVENT_TABLE();
};

#endif
