#ifndef CNCPREPROCESSOR_H
#define CNCPREPROCESSOR_H

#include "wxCrafterMotionMonitor.h"
#include "CncMoveSequence.h"
#include "CncPathListEntry.h"

class CncPathListEntryListCtrl;
class CncMoveSequenceListCtrl;
class CncMoveSequenceOverviewListCtrl;
class CncOperatingTrace;

class CncPreprocessor : public CncPreprocessorBase {
	
	private:
		bool								useOperatingTrace;
		
		CncPathListEntryListCtrl* 			pathListEntries;
		CncMoveSequenceOverviewListCtrl* 	moveSequenceOverview;
		CncMoveSequenceListCtrl* 			moveSequence;
		CncOperatingTrace*					operatingTrace;
		
		void updatePathListContent();
		void updateMoveSequenceListContent(bool force = false);
		
	public:
		enum ListType{ LT_PATH_LIST, LT_MOVE_SEQ_OVERVIEW, LT_MOVE_SEQ_CONTENT};

		CncPreprocessor(wxWindow* parent);
		virtual ~CncPreprocessor();
		
		void popProcessMode();
		void pushUpdateMode();
		
		void enablePathListEntries		(bool state);
		void enableMoveSequences		(bool state);
		void enableOperatingTrace		(bool state);
		
		void clearAll();
		
		void clearPathListEntries();
		void addPathListEntry(const CncPathListEntry& cpe);
		
		void clearMoveSequences();
		void addMoveSequence(const CncMoveSequence& seq);
		
		void clearOperatingTrace();
		void addOperatingTrace			(const wxString& s);
		void addOperatingTrace			(const std::stringstream& s);
		void addOperatingTraceMovSeqSep	(const wxString& s);
		void addOperatingTracePthLstSep	(const wxString& s);
		void addOperatingTraceSeparator	(const wxString& s);
		void addOperatingTraceDebugEntry(const wxString& s);
		void addOperatingTraceWarnEntry	(const wxString& s);
		void addOperatingTraceErrorEntry(const wxString& s);

		
		void selectClientId(long id, CncPreprocessor::ListType lt);
		
		void updateContent();

	protected:
		virtual void copyOperatingTrace(wxCommandEvent& event);
		virtual void saveOperatingTrace(wxCommandEvent& event);
		virtual void clearOperatingTrace(wxCommandEvent& event);
		virtual void connectOperatingTrace(wxCommandEvent& event);
		virtual void clearMoveSequences(wxCommandEvent& event);
		virtual void clearPathListEntries(wxCommandEvent& event);
		virtual void connectMoveSequences(wxCommandEvent& event);
		virtual void connectPathListEntries(wxCommandEvent& event);
		
		void onIndividualCommand(wxCommandEvent& event);
		
		wxDECLARE_NO_COPY_CLASS(CncPreprocessor);
		wxDECLARE_EVENT_TABLE();
};

#endif // CNCPREPROCESSOR_H
