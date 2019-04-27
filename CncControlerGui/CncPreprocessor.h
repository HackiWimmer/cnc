#ifndef CNCPREPROCESSOR_H
#define CNCPREPROCESSOR_H

#include "wxcrafter.h"
#include "CncPathListEntry.h"
#include "CncPathListEntryListCtrl.h"

class CncPreprocessor : public CncPreprocessorBase {
	
	private:
		CncPathListEntryListCtrl* pathListEntries;

	public:
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
		//addMoveSequence(...)
		
		void selectClientId(long id);
		
	protected:
		virtual void clearMoveSequences(wxCommandEvent& event);
		virtual void clearPathListEntries(wxCommandEvent& event);
		virtual void connectMoveSequences(wxCommandEvent& event);
		virtual void connectPathListEntries(wxCommandEvent& event);
};
#endif // CNCPREPROCESSOR_H
