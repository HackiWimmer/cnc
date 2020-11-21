#include "CncConfig.h"
#include "SvgCncContext.h"

//////////////////////////////////////////////////////////////////
SvgCncContext::SvgCncContext()
: reversePath 				(false)
, currentZDepth				(0.0)
, currentZDepthMode			('Z')
, currentLineNumber			(UNDEFINED_LINE_NUMBER)
, toolPathCorrection		(CncCT_None)
, fillColourDecoder			()
, strokeColourDecoder		()
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
SvgCncContext::~SvgCncContext() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
bool SvgCncContext::hasSomethingToCorrect() const {
//////////////////////////////////////////////////////////////////
	const bool b1 = hasToolCorrection();
	const bool b2 = hasReverseCorrection();
	
	return b1 || b2;
}
//////////////////////////////////////////////////////////////////
bool SvgCncContext::hasToolCorrection() const {
//////////////////////////////////////////////////////////////////
	switch ( toolPathCorrection ) {
		case CncCT_Pocket:
		case CncCT_Inner:
		case CncCT_Outer:	return true;
		
		case CncCT_Center:
		case CncCT_None:	return false;
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setCurrentLineNumber(int cln) { 
//////////////////////////////////////////////////////////////////
	currentLineNumber = cln; 
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setCurrentZDepth(const wxString& flag) {
//////////////////////////////////////////////////////////////////
	//depth="z-1.0" || depth="Z5.0"
	wxString f(flag);
	f.Trim(true).Trim(false);
	
	currentZDepth		= 0.0;
	currentZDepthMode 	= 'Z';
	
	if ( f.IsEmpty() )
		return;
		
	currentZDepthMode = (char)f[0];
	if ( currentZDepthMode != 'z' && currentZDepthMode != 'Z' )
		currentZDepthMode = 'Z';
	
	f = f.SubString(1, f.Length()-1);
	f.ToDouble(&currentZDepth);
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setReverseFlag(const wxString& flag) {
//////////////////////////////////////////////////////////////////
	wxString f(flag);
	f.Trim(true).Trim(false);
	reversePath = f.IsSameAs("YES") ;
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::setToolCorrectionType(const wxString& type) {
//////////////////////////////////////////////////////////////////
	wxString t(type);
	t.Trim(true).Trim(false);
	
	if      ( t.IsSameAs("INNER")  )	toolPathCorrection = CncCT_Inner;
	else if ( t.IsSameAs("OUTER")  )	toolPathCorrection = CncCT_Outer;
	else if ( t.IsSameAs("CENTER")   )	toolPathCorrection = CncCT_Center;
	else if ( t.IsSameAs("POCKET") )	toolPathCorrection = CncCT_Pocket;
	else if ( t.IsSameAs("NONE")   )	toolPathCorrection = CncCT_None;
	else								toolPathCorrection = CncCT_None;
}
//////////////////////////////////////////////////////////////////
const char* SvgCncContext::getToolCorrectionTypeAsStr() const {
//////////////////////////////////////////////////////////////////
	switch (toolPathCorrection) {
		case CncCT_Pocket:	return "Pocket";
		case CncCT_Center:	return "Center";
		case CncCT_Inner:	return "Inner";
		case CncCT_Outer:	return "Outer";
		case CncCT_None:	return "None";
	}
	
	return wxString::Format("%s: unkown", CNC_LOG_FUNCT);
}
//////////////////////////////////////////////////////////////////
const char* SvgCncContext::getZDeptAsStr() const {
//////////////////////////////////////////////////////////////////
	return wxString::Format("%c%.3lf", currentZDepthMode, currentZDepth);
}
//////////////////////////////////////////////////////////////////
void SvgCncContext::getParameterList(DcmItemList& rows) const {
//////////////////////////////////////////////////////////////////
	DataControlModel::addKeyValueRow(rows, "SVG CNC-Parameters", 	"");
	DataControlModel::addKeyValueRow(rows, "  Reverse Path", 			reversePath ? "Yes" : "No");
	DataControlModel::addKeyValueRow(rows, "  Correction Type", 		getToolCorrectionTypeAsStr());
	DataControlModel::addKeyValueRow(rows, "  Z Depth", 				getZDeptAsStr());
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
				// on line cut
				toolPathCorrection = CncCT_Center;
			}
			else if ( fillColourDecoder.matchesGray() ) {
				// pocketing cut
				toolPathCorrection = CncCT_Pocket;
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
