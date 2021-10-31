#ifndef CNC_EXCEPTIONS_H
#define CNC_EXCEPTIONS_H

#include <exception>
#include <wx/string.h>

class CncInterruption : public std::runtime_error {
	
	public:
		
		enum Type 
		{ 
			CNC_EX_INTERRUPTION, 
			CNC_EX_CONNECTION_ERROR, 
			CNC_EX_READ_ERROR, 
			CNC_EX_WRITE_ERROR 
		};
		
		explicit CncInterruption (const char* what_arg, Type t = CNC_EX_INTERRUPTION) 
		: runtime_error	(wxString::Format("CncInterruption: %s", what_arg ? what_arg : "")) 
		, type			(t)
		, catchedAt		()
		{}
		
		explicit CncInterruption (const CncInterruption& e)
		: runtime_error	(e.what())
		, type			(e.type)
		, catchedAt		(e.catchedAt)
		{}
		
		void addCatchLocation(const wxString& context)
		{
			if ( catchedAt.IsAscii() )	catchedAt.append(context);
			else						catchedAt.append(wxString::Format("\n%s", context));
		}
		
		const Type			getTypeInfo()			const { return type; }
		const wxString&		getCatchLocation()		const { return catchedAt; }
		
		const char*			getTypeInfoAsString()	const 
		{ 
			switch ( type )
			{
				case CNC_EX_INTERRUPTION:		return "Interruption";
				case CNC_EX_CONNECTION_ERROR:	return "Connection Error";
				case CNC_EX_READ_ERROR:			return "Read Error"; 
				case CNC_EX_WRITE_ERROR:		return "Write Error"; 
			}
			
			return "Unknown CncInterruptException Type"; 
		}
		
	protected:
		Type		type;
		wxString	catchedAt;
		
};

#endif