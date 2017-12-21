#ifndef CNC_NUMBER_FORMATTER_H_INCLUDED
#define CNC_NUMBER_FORMATTER_H_INCLUDED

#include <wx/numformatter.h>

///////////////////////////////////////////////////////////////////////////////
// This class can used to generate human readable output for numbers as strings
// without unsing wxLocale. The locale specific informations are handels by this
// class
//
// Please note: wxLocale isn't used - generally in the whole app - with respect to 
// the resulting overhead or perfromance reducement in most of the used conversion 
// functions like e. g. ToLong, etc. or the wxNumberFormatter itself

class CncNumberFormatter {
	
	private:
		static char decimalSpeparator;
		static char thousandsSpeparator;

		//////////////////////////////////////////////////////////////////////////
		static void addThousandsSeparators(wxString& s);
		static void removeTrailingZeroes(wxString& s);
		static void removeThousandsSeparators(wxString& s);
		
	public:
		
		//////////////////////////////////////////////////////////////////////////
		static const char getDecimalSeparator()          { return CncNumberFormatter::decimalSpeparator; }
		static const char getThousandsSeparator()        { return CncNumberFormatter::thousandsSpeparator; }
		
		static void setDecimalSeparator(char c)          { decimalSpeparator = c; }
		static void setThousandsSeparator(char c)        { thousandsSpeparator = c; }
		
		
		//////////////////////////////////////////////////////////////////////////
		static wxString toString (long val, int flags=wxNumberFormatter::Style_WithThousandsSep);
		static wxString toString (double val, int precision, int flags=wxNumberFormatter::Style_WithThousandsSep);
		
		//////////////////////////////////////////////////////////////////////////
		static bool fromString(wxString s, long *val);
		static bool fromString(wxString s, double *val);

};

#endif
