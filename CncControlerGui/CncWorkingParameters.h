#ifndef CncWorkingParameters_H
#define CncWorkingParameters_H

#include "CncCommon.h"
#include "DataControlModel.h"

struct CncWorkingParameters {
	
	private:
	
		bool reversePath 				= false;
		CncToolCorretionType correction = CncCT_None;
		double currentZDepth			= 0.0;
		char currentZDepthMode			= 'Z';
	
	public:
		int currentLineNumber			= UNDEFINED_LINE_NUMBER;
		
		///////////////////////////////////////////////////////////
		bool getReverseFlag() 						{ return reversePath; }
		CncToolCorretionType getCorrectionType() 	{ return correction; }
		double getCurrentZDepth() 					{ return currentZDepth; }
		bool isCurrentZDepthAbs()					{ return currentZDepthMode == 'Z'; }
		bool isCurrentZDepthRel()					{ return !isCurrentZDepthAbs(); }
		char getCurrentZDepthMode()					{ return currentZDepthMode; }
		
		///////////////////////////////////////////////////////////
		void setCurrentZDepth(wxString& flag) {
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
		}
		
		///////////////////////////////////////////////////////////
		void setReverseFlag(wxString& flag) {
			flag.Trim(true).Trim(false);
			reversePath = (flag.MakeUpper() == "YES");
		}
		
		///////////////////////////////////////////////////////////
		void setCorrectionType(wxString& type) {
			type.Trim(true).Trim(false);
			if      ( type.Upper() == "INNER")	correction = CncCT_Inner;
			else if ( type.Upper() == "OUTER")	correction = CncCT_Outer;
			else if ( type.Upper() == "NONE")	correction = CncCT_None;
			else								correction = CncCT_Center;
		}
		
		///////////////////////////////////////////////////////////
		const char* getCorrectionTypeAsString() {
			switch (correction) {
				case CncCT_Center:	return "Center";
				case CncCT_Inner:	return "Inner";
				case CncCT_Outer:	return "Outer";
				case CncCT_None:	return "None";
			}
			return "unkown";
		}
		
		///////////////////////////////////////////////////////////
		const char* getZDeptAsString() {
			static wxString s;
			
			s = wxString::Format("%c%.3lf", currentZDepthMode, currentZDepth);
			return s;
		}
		
		///////////////////////////////////////////////////////////
		void getParameterList(DcmItemList& rows) {
			DataControlModel::addKeyValueRow(rows, "Cnc Working Parameters", 	"");
			DataControlModel::addKeyValueRow(rows, "  Reverse Path", 			reversePath ? "Yes" : "No");
			DataControlModel::addKeyValueRow(rows, "  Correction Type", 		getCorrectionTypeAsString());
			DataControlModel::addKeyValueRow(rows, "  Z Depth", 				getZDeptAsString());
		}
};

#endif