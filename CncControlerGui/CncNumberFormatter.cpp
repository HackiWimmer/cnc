#include "CncNumberFormatter.h" 

//////////////////////////////////////////////////////////////////////////	
char CncNumberFormatter::decimalSpeparator		= '.';
char CncNumberFormatter::thousandsSpeparator	= '\'';

//////////////////////////////////////////////////////////////////////////	
void CncNumberFormatter::removeThousandsSeparators(wxString& s) {
//////////////////////////////////////////////////////////////////////////
    wxChar thousandsSep = getThousandsSeparator();
    s.Replace(wxString(thousandsSep), wxString());
}
//////////////////////////////////////////////////////////////////////////
bool CncNumberFormatter::fromString(wxString s, long *val) {
//////////////////////////////////////////////////////////////////////////
    removeThousandsSeparators(s);
    return s.ToLong(val);
}
//////////////////////////////////////////////////////////////////////////
bool CncNumberFormatter::fromString(wxString s, double *val) {
//////////////////////////////////////////////////////////////////////////
    removeThousandsSeparators(s);
    return s.ToDouble(val);
}
//////////////////////////////////////////////////////////////////////////
wxString CncNumberFormatter::toString(long val, int style) {
//////////////////////////////////////////////////////////////////////////
    wxString s = wxString::Format("%ld", val);

    if ( style & wxNumberFormatter::Style_WithThousandsSep )
        addThousandsSeparators(s);

    return s;
}
//////////////////////////////////////////////////////////////////////////
wxString CncNumberFormatter::toString(double val, int precision, int style) {
//////////////////////////////////////////////////////////////////////////
    wxString s = wxString::FromCDouble(val,precision);

    if ( style & wxNumberFormatter::Style_WithThousandsSep )
        addThousandsSeparators(s);

    if ( style & wxNumberFormatter::Style_NoTrailingZeroes )
        removeTrailingZeroes(s);

    return s;
}
//////////////////////////////////////////////////////////////////////////
void CncNumberFormatter::removeTrailingZeroes(wxString& s) {
//////////////////////////////////////////////////////////////////////////
    // If number is in scientific format, trailing zeroes belong to the exponent and cannot be removed.
    if ( s.find_first_of("eE") != wxString::npos )
        return;

    const size_t posDecSep = s.find(getDecimalSeparator());
    // No decimal point => removing trailing zeroes irrelevant for integer number.
    if ( posDecSep == wxString::npos )
        return;
    wxCHECK_RET( posDecSep, "Can't start with decimal separator" );

    // Find the last character to keep.
    size_t posLastNonZero = s.find_last_not_of("0");

    // If it's the decimal separator itself, don't keep it neither.
    if ( posLastNonZero == posDecSep )
        posLastNonZero--;

    s.erase(posLastNonZero + 1);
    // Remove sign from orphaned zero.
    if ( s.compare("-0") == 0 )
        s = "0";
}
//////////////////////////////////////////////////////////////////////////
void CncNumberFormatter::addThousandsSeparators(wxString& s) {
//////////////////////////////////////////////////////////////////////////
    // Thousands separators for numbers in scientific format are not relevant.
    if ( s.find_first_of("eE") != wxString::npos )
        return;

    wxChar thousandsSep = CncNumberFormatter::getThousandsSeparator();

    size_t pos = s.find(CncNumberFormatter::getDecimalSeparator());
    if ( pos == wxString::npos ) {
        // Start grouping at the end of an integer number.
        pos = s.length();
    }

    // End grouping at the beginning of the digits -- there could be at a sign
    // before their start.
    const size_t start = s.find_first_of("0123456789");

    // We currently group digits by 3 independently of the locale. This is not
    // the right thing to do and we should use lconv::grouping (under POSIX)
    // and GetLocaleInfo(LOCALE_SGROUPING) (under MSW) to get information about
    // the correct grouping to use. This is something that needs to be done at
    // wxLocale level first and then used here in the future (TODO).
    const size_t GROUP_LEN = 3;

    while ( pos > start + GROUP_LEN ) {
        pos -= GROUP_LEN;
        s.insert(pos, thousandsSep);
    }
}