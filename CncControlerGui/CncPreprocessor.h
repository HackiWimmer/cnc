#ifndef CNCPREPROCESSOR_H
#define CNCPREPROCESSOR_H

#include "wxCrafterMotionMonitor.h"
#include "CncMoveSequence.h"
#include "CncPathListEntry.h"

class CncPathListEntryListCtrl;
class CncMoveSequenceListCtrl;
class CncMoveSequenceOverviewListCtrl;

class CncPreprocessor : public CncPreprocessorBase {
	
	private:
		CncPathListEntryListCtrl* 			pathListEntries;
		CncMoveSequenceOverviewListCtrl* 	moveSequenceOverview;
		CncMoveSequenceListCtrl* 			moveSequence;

		void updatePathListContent();
		void updateMoveSequenceListContent(bool force = false);
		
	public:
		enum ListType{ LT_PATH_LIST, LT_MOVE_SEQ_OVERVIEW, LT_MOVE_SEQ_CONTENT};

		CncPreprocessor(wxWindow* parent);
		virtual ~CncPreprocessor();
		
		void freeze();
		void thaw();
		
		void enablePathListEntries(bool state);
		void enableMoveSequences(bool state);
		
		void clearAll();
		
		void clearPathListEntries();
		void addPathListEntry(const CncPathListEntry& cpe);
		
		void clearMoveSequences();
		void addMoveSequence(const CncMoveSequence& seq);
		
		void selectClientId(long id, CncPreprocessor::ListType lt);
		
		void updateContent();

	protected:
		
		virtual void clearMoveSequences(wxCommandEvent& event);
		virtual void clearPathListEntries(wxCommandEvent& event);
		virtual void connectMoveSequences(wxCommandEvent& event);
		virtual void connectPathListEntries(wxCommandEvent& event);
		
		void onIndividualCommand(wxCommandEvent& event);
		
		wxDECLARE_NO_COPY_CLASS(CncPreprocessor);
		wxDECLARE_EVENT_TABLE();
};

#endif // CNCPREPROCESSOR_H
