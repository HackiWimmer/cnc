#include <wx/event.h>

class IndividualCommandEvent;

//---------------------------------------------------------------------------------------------
	wxDECLARE_EVENT(wxEVT_INDIVIDUAL_CTRL_COMMAND, 			IndividualCommandEvent);
//---------------------------------------------------------------------------------------------

namespace EvtPreprocessor {
	const int UpdateSelectedClientIds	= 1;
};

//---------------------------------------------------------------------------------------------
class IndividualCommandEvent : public wxCommandEvent {
	
	public:
		IndividualCommandEvent(int id = 0)
		: wxCommandEvent(wxEVT_INDIVIDUAL_CTRL_COMMAND, id)
		{}

		virtual wxEvent *Clone() const {
			return new IndividualCommandEvent(*this);
		}
		
};