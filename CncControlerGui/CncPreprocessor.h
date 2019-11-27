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
		enum ListType{ LT_PATH_LIST, LT_MOVE_SEQUENCE};

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
		void addMoveSequenceStart(const CncMoveSequence& seq, double value_MM_MIN, char mode);
		void addMoveSequence(const CncMoveSequence& seq);
		
		void selectClientId(long id, CncPreprocessor::ListType lt);
		
		void updateContent();

	protected:
	
		virtual void clearMoveSequences(wxCommandEvent& event);
		virtual void clearPathListEntries(wxCommandEvent& event);
		virtual void connectMoveSequences(wxCommandEvent& event);
		virtual void connectPathListEntries(wxCommandEvent& event);
};
#endif // CNCPREPROCESSOR_H
