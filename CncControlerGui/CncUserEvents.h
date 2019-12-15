#include <wx/event.h>
#include <wx/any.h>

class IndividualCommandEvent;

//---------------------------------------------------------------------------------------------
	wxDECLARE_EVENT(wxEVT_INDIVIDUAL_CTRL_COMMAND, 			IndividualCommandEvent);
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
class IndividualCommandEvent : public wxCommandEvent {
	
	public:
		// define here individual event ids
		enum EvtPreprocessor 	{ UpdateSelectedClientIds };
		enum EvtMainFrame		{ WaitActive, DispatchAll, DistpatchNext, EnableControls, ExtViewBoxChange };

	public:

		enum ValueName { VAL1=0, VAL2=1, VAL3=2, VAL4=3, VAL5=4, VAL6=5, VAL7=6, VAL8=7 };
		enum Value     { MAX=VAL8 + 1 };

		IndividualCommandEvent(int id = 0)
		: wxCommandEvent(wxEVT_INDIVIDUAL_CTRL_COMMAND, id)
		, values()
		{}

		explicit IndividualCommandEvent(const IndividualCommandEvent& event)
		: wxCommandEvent(event)
		, values(event.values)
		{}

		virtual wxEvent *Clone() const {
			return new IndividualCommandEvent(*this);
		}
		
		bool hasValue(ValueName name)					{ return flags[name]; }

		template <class T>
		const T getValue(ValueName name)				{ return values[name].As<T>(); }

		template <class T>
		void setValue(ValueName name, T value)			{ values[name] = value; flags[name] = true; }

	protected:

		wxAny 	values[Value::MAX];
		bool 	flags[Value::MAX];

};

