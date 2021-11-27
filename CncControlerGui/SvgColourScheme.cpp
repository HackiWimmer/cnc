#include <wx/tokenzr.h>
#include "SvgColourScheme.h"

//////////////////////////////////////////////////////////////////
SvgColourDecoder::SvgColourDecoder() 
: currentTest		(wxNullColour)
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
SvgColourDecoder::SvgColourDecoder(const wxColour& col) 
: currentTest		(col)
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
SvgColourDecoder::~SvgColourDecoder() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void SvgColourDecoder::trace(std::ostream &ostr) const {
//////////////////////////////////////////////////////////////////
	ostr	<< wxString::Format("Valid=%d, rgb(%d,%d,%d)", 
								valid(), 
								getRed(),
								getGreen(),
								getBlue())
			;
}
//////////////////////////////////////////////////////////////////
bool SvgColourDecoder::matchesWhite() const {
//////////////////////////////////////////////////////////////////
	const bool r = currentTest.Red()	> 240;
	const bool g = currentTest.Green()	> 240;
	const bool b = currentTest.Blue()	> 240;
	
	return ( r && g && b );
}

//////////////////////////////////////////////////////////////////
bool SvgColourDecoder::matchesBlack() const {
//////////////////////////////////////////////////////////////////
	const bool r = currentTest.Red()	<  16;
	const bool g = currentTest.Green()	<  16;
	const bool b = currentTest.Blue()	<  16;
	
	return ( r && g && b );
}
//////////////////////////////////////////////////////////////////
bool SvgColourDecoder::matchesRed() const {
//////////////////////////////////////////////////////////////////
	const bool r = currentTest.Red();
	const bool g = currentTest.Green();
	const bool b = currentTest.Blue();
	
	return ( r > b && b == g);
}
//////////////////////////////////////////////////////////////////
bool SvgColourDecoder::matchesBlue() const {
//////////////////////////////////////////////////////////////////
	const bool r = currentTest.Red();
	const bool g = currentTest.Green();
	const bool b = currentTest.Blue();
	
	return ( b > r && r == g);
}
//////////////////////////////////////////////////////////////////
bool SvgColourDecoder::matchesGray() const {
//////////////////////////////////////////////////////////////////
	if ( matchesBlack() )
		return false;
	
	if ( matchesWhite() )
		return false;
	
	const bool r = currentTest.Red();
	const bool g = currentTest.Green();
	const bool b = currentTest.Blue();
	
	return ( r == g && g == b );
}
//////////////////////////////////////////////////////////////////
bool SvgColourDecoder::setColour(const wxColour& col) {
//////////////////////////////////////////////////////////////////
	currentTest = col;
	return currentTest.IsOk();
}
//////////////////////////////////////////////////////////////////
bool SvgColourDecoder::setColour(const wxString& str) {
//////////////////////////////////////////////////////////////////
	currentTest = wxNullColour;
	
	if ( str.IsEmpty() )
		return false;
		
	// ----------------------------------------------------------
	if ( str.StartsWith("#") ) {
		// #d70000 or #80d380
		unsigned int r, g, b;
		if ( sscanf(str.AfterFirst('#').SubString(0, 6), "%02x%02x%02x", &r, &g, &b) == 3 )
			currentTest = wxColour(r, g, b);
	}
	// ----------------------------------------------------------
	else if ( str.StartsWith("rgb") ) {
		// rgb(153,68,0)
		unsigned int r, g, b;
		if ( sscanf(str.AfterFirst('(').BeforeLast(')'), "%d,%d,%d", &r, &g, &b) == 3 )
			currentTest = wxColour(r, g, b);
	}
	// ----------------------------------------------------------
	else if ( str.StartsWith("hsl") ) {
		// hsl(240,100%,50%)
		// ------------------------------------------------------
		auto decodeToken = [](const wxString& t) {
			unsigned int ret = 0;
			
			if ( t.IsEmpty() == false ) {
				if ( t.Contains("%") )	{ ret = (float)(atoi(t))/100.0 * 255; }
				else 					{ ret = atoi(t); }
			}
			
			return ret;
		};
		
		unsigned int r, g, b;
		unsigned int counter = 0;
		
		wxStringTokenizer values(str.AfterFirst('(').BeforeLast(')'), ",");
		while ( values.HasMoreTokens() ) {
			const wxString token(values.GetNextToken());
			
			switch ( counter ) {
				case 0:	r = decodeToken(token); counter++;	break;
				case 1:	g = decodeToken(token); counter++;	break;
				case 2:	b = decodeToken(token); counter++;	break;
				default: 									break;
			}
		}
		
		if ( counter == 3 )
			currentTest = wxColour(r, g, b);
	}
	// ----------------------------------------------------------
	else {
		// searching as name
		currentTest = wxTheColourDatabase->Find(str);
	}
	
	return currentTest.IsOk();
}


//////////////////////////////////////////////////////////////////
SvgStyleAttributeDecoder::SvgStyleAttributeDecoder() 
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
SvgStyleAttributeDecoder::~SvgStyleAttributeDecoder() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
unsigned int SvgStyleAttributeDecoder::decode(const wxString& attrStr) {
//////////////////////////////////////////////////////////////////
	//style="fill:black;stroke:#e48080;stroke-width:0.1;stroke-linecap:square"
	unsigned int counter = 0;
	wxStringTokenizer values(attrStr, ";");
	
	while ( values.HasMoreTokens() ) {
		const wxString token(values.GetNextToken());
		const wxString key(token.BeforeFirst(':'));
		const wxString val(token.AfterFirst(':'));
		
		counter++;
		
		if ( notifyNextParameter(key, val) == false )
			break;
	}
	
	return counter;
}


//////////////////////////////////////////////////////////////////
void SvgColourAttributeDecoder::reset() {
//////////////////////////////////////////////////////////////////
	fillFound	= false;
	strokeFound	= false;
	fillCol		= defFillCol;
	strokeCol	= defStrokeCol;
}
//////////////////////////////////////////////////////////////////
bool SvgColourAttributeDecoder::notifyNextParameter(const wxString& key, const wxString& val) {
//////////////////////////////////////////////////////////////////
	if ( key.IsSameAs("fill") ) {
		fillFound = true;
		
		decoder.setColour(val);
		fillCol = decoder.getColour();
	}
	else if (key.IsSameAs("stroke") ) {
		strokeFound = true;
		
		decoder.setColour(val);
		strokeCol = decoder.getColour();
	}
	
	// break if both  values found
	return !(fillFound == true && strokeFound == true);
}



