#include "CncConfig.h"
#include "SvgCncContext.h"

//////////////////////////////////////////////////////////////////
SvgCncContext::SvgCncContext()
: reversePath 			(false)
, currentZDepth			(0.0)
, currentZDepthMode		('Z')
, currentLineNumber		(UNDEFINED_LINE_NUMBER)
, toolPathCorrection			(CncCT_None)
, fillColourDecoder		()
, strokeColourDecoder	()
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
SvgCncContext::~SvgCncContext() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setCurrentLineNumber(int cln) { 
//////////////////////////////////////////////////////////////////
	currentLineNumber = cln; 
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setCurrentZDepth(wxString& flag) {
//////////////////////////////////////////////////////////////////
	//depth="z-1.0" || depth="Z5.0"
	flag.Trim(true).Trim(false);
	
	currentZDepth		= 0.0;
	currentZDepthMode 	= 'Z';
	
	if ( flag.IsEmpty() )
		return;
		
	currentZDepthMode = (char)flag[0];
	if ( currentZDepthMode != 'z' && currentZDepthMode != 'Z' )
		currentZDepthMode = 'Z';
	
	flag = flag.SubString(1, flag.Length()-1);
	flag.ToDouble(&currentZDepth);
	
	//clog << "current z depth " << currentZDepthMode << currentZDepth << endl;
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setReverseFlag(wxString& flag) {
//////////////////////////////////////////////////////////////////
	flag.Trim(true).Trim(false);
	reversePath = (flag.MakeUpper() == "YES");
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setCorrectionType(wxString& type) {
//////////////////////////////////////////////////////////////////
	type.Trim(true).Trim(false);
	if      ( type.Upper() == "INNER")	toolPathCorrection = CncCT_Inner;
	else if ( type.Upper() == "OUTER")	toolPathCorrection = CncCT_Outer;
	else if ( type.Upper() == "NONE")	toolPathCorrection = CncCT_None;
	else								toolPathCorrection = CncCT_Center;
}
//////////////////////////////////////////////////////////////////
const char* SvgCncContext::getCorrectionTypeAsString() {
//////////////////////////////////////////////////////////////////
	switch (toolPathCorrection) {
		case CncCT_Center:	return "Center";
		case CncCT_Inner:	return "Inner";
		case CncCT_Outer:	return "Outer";
		case CncCT_None:	return "None";
	}
	return "unkown";
}
//////////////////////////////////////////////////////////////////
const char* SvgCncContext::getZDeptAsString() {
//////////////////////////////////////////////////////////////////
	static wxString s;
	
	s = wxString::Format("%c%.3lf", currentZDepthMode, currentZDepth);
	return s;
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::getParameterList(DcmItemList& rows) {
//////////////////////////////////////////////////////////////////
	DataControlModel::addKeyValueRow(rows, "SVG CNC-Parameters", 	"");
	DataControlModel::addKeyValueRow(rows, "  Reverse Path", 			reversePath ? "Yes" : "No");
	DataControlModel::addKeyValueRow(rows, "  Correction Type", 		getCorrectionTypeAsString());
	DataControlModel::addKeyValueRow(rows, "  Z Depth", 				getZDeptAsString());
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setFillColour(const wxColour & col) {
//////////////////////////////////////////////////////////////////
	fillColourDecoder.setColour(col);
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setStrokeColour(const wxColour & col) {
//////////////////////////////////////////////////////////////////
	strokeColourDecoder.setColour(col);
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::determineColourEffects() {
//////////////////////////////////////////////////////////////////
	// reset
	toolPathCorrection	= CncCT_Center;
	guidePath			= false;
	
	if ( THE_CONFIG->getSvgUseColourScheme() == true ) {
		
		// ----------------------------------------------------------
		if ( strokeColourDecoder.matchesBlack() ) {
			if ( fillColourDecoder.matchesWhite() ) {
				// interior cut
				toolPathCorrection = CncCT_Inner;
			}
			else if ( fillColourDecoder.matchesBlack() ) {
				// exterior cut
				toolPathCorrection = CncCT_Outer;
			}
			else if ( fillColourDecoder.matchesBlue() ) {
				// guide
				toolPathCorrection	= CncCT_None;
				guidePath			= true;
			}
		}
		// ----------------------------------------------------------
		else if ( strokeColourDecoder.matchesGray() ) {
			if ( fillColourDecoder.matchesWhite() ) {
				// one line cut
				toolPathCorrection = CncCT_Center;
			}
			else if ( fillColourDecoder.matchesGray() ) {
				// pocketing cut
				toolPathCorrection = CncCT_Inner;
			}
			else if ( fillColourDecoder.matchesBlue() ) {
				// guide
				toolPathCorrection	= CncCT_None;
				guidePath			= true;
			}
		}
		// ----------------------------------------------------------
		else if ( strokeColourDecoder.matchesBlue() ) {
			// guide
				toolPathCorrection	= CncCT_None;
				guidePath			= true;
		}
	}
}
