#ifndef CNCPREPROCESSOR_H
#define CNCPREPROCESSOR_H

#include "wxcrafter.h"
#include "CncMoveSequence.h"
#include "CncPathListEntry.h"

class CncPathListEntryListCtrl;
class CncMoveSequenceListCtrl;

class CncPreprocessor : public CncPreprocessorBase {
	
	private:
		CncPathListEntryListCtrl* pathListEntries;
		CncMoveSequenceListCtrl* moveSequences;

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
		void addMoveSequenceStart(const CncMoveSequence& seq);
		void addMoveSequence(const CncMoveSequence& seq);
		
		void selectClientId(long id, ListType lt);
		
	protected:
		virtual void clearMoveSequences(wxCommandEvent& event);
		virtual void clearPathListEntries(wxCommandEvent& event);
		virtual void connectMoveSequences(wxCommandEvent& event);
		virtual void connectPathListEntries(wxCommandEvent& event);
};
#endif // CNCPREPROCESSOR_H
