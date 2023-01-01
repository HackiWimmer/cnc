#ifndef CNC_ANCHOR_INFO_H
#define CNC_ANCHOR_INFO_H

#include <map>
#include <wx/string.h>
#include "CncPosition.h"

struct CncAnchorName
{
	static const wxString ANCHOR_TOOL_CHANGE;
	static const wxString ANCHOR_ZTOUCH;
	static const wxString ANCHOR_PARKING;
};

struct CncAnchorInfo 
{
	bool				show		= true; 
	bool				fixed		= true;
	wxString			name		= "";
	wxString			type		= "P";
	CncDoublePosition	pos			= { 0.0, 0.0, 0.0 };
	
	bool isPhysically() const { return type.IsSameAs("P") == true;  }
	bool isLogically()  const { return type.IsSameAs("P") == false; }
};

class CncAnchorMap : public std::map<wxString, CncAnchorInfo> 
{
	public:
		CncAnchorMap()
		//: std::map()
		{}
		
		~CncAnchorMap()
		{}
		
		CncDoublePosition getLogicalAnchorToolChange() const;
		CncDoublePosition getLogicalAnchorZTouch() const;
		CncDoublePosition getLogicalAnchorParking() const;
		
		CncDoublePosition getPhysicalAnchorToolChange() const;
		CncDoublePosition getPhysicalAnchorZTouch() const;
		CncDoublePosition getPhysicalAnchorParking() const;
};

#endif
