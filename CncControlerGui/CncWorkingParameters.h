#ifndef CncWorkingParameters_H
#define CncWorkingParameters_H

#include "CncCommon.h"
#include "DataControlModel.h"

struct CncWorkingParameters {
	
	private:
		bool reversePath 				= false;
		CncToolCorretionType correction = CncCT_None;
	
	public:
		int currentLineNumber			= UNDEFINED_LINE_NUMBER;
		
		bool getReverseFlag() { return reversePath; }
		CncToolCorretionType getCorrectionType() { return correction; }
		
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
		void getParameterList(DcmItemList& rows) {
			DataControlModel::addKeyValueRow(rows, "Cnc Working Parameters", 	"");
			DataControlModel::addKeyValueRow(rows, "  Reverse Path", 			reversePath ? "Yes" : "No");
			DataControlModel::addKeyValueRow(rows, "  Coorection type", 		getCorrectionTypeAsString());
		}
};

#endif