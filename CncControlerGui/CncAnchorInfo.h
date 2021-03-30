#ifndef CNC_ANCHOR_INFO_H
#define CNC_ANCHOR_INFO_H

#include <map>
#include <wx/string.h>
#include "CncPosition.h"

struct CncAnchorInfo {
	
	bool				show		= true; 
	bool				fixed		= false;
	bool				absolute	= false;
	wxString			name		= "";
	wxString			type		= "XYZ";
	CncDoublePosition	pos			= { 0.0, 0.0, 0.0 };
	
	bool hasX() const { return type.Contains("X"); }
	bool hasY() const { return type.Contains("Y"); }
	bool hasZ() const { return type.Contains("Z"); }
};

class CncAnchorMap : public std::map<wxString, CncAnchorInfo> {
	
	public:
		CncAnchorMap()
		//: std::map()
		{}
		
		~CncAnchorMap()
		{}
};

#endif
