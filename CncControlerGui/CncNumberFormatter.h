#ifndef CNC_NUMBER_FORMATTER_H_INCLUDED
#define CNC_NUMBER_FORMATTER_H_INCLUDED

#include <wx/numformatter.h>

class CncNumberFormatter {
	
	public:
		
		//////////////////////////////////////////////////////////////////////////
		static const wxString toString (long val, int flags=wxNumberFormatter::Style_WithThousandsSep) {
			wxLocale local(wxLANGUAGE_DEFAULT);
			
			return wxNumberFormatter::ToString(val, flags);
		}
		
		//////////////////////////////////////////////////////////////////////////
		static const wxString toString (double val, int precision, int flags=wxNumberFormatter::Style_WithThousandsSep) {
			wxLocale local(wxLANGUAGE_DEFAULT);
			
			return wxNumberFormatter::ToString(val, precision, flags);
		}

};

#endif