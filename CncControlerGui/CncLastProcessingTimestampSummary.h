#ifndef CNCLASTPROCESSINGTIMESTAMPSUMMARY_H
#define CNCLASTPROCESSINGTIMESTAMPSUMMARY_H
#include "wxCrafterBaseDialogs.h"

class CncLastProcessingTimestampSummary : public CncLastProcessingTimestampSummaryBase	{
	
	public:
		CncLastProcessingTimestampSummary(wxWindow* parent);
		virtual ~CncLastProcessingTimestampSummary();
		
		void addTimestamp(const wxString& key, const wxString& val);
	
	protected:
		virtual void onInitDilaog(wxInitDialogEvent& event);
		virtual void onClose(wxCommandEvent& event);
};
#endif // CNCLASTPROCESSINGTIMESTAMPSUMMARY_H
