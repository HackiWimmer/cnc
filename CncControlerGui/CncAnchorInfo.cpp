#include "CncConfig.h"
#include "CncContext.h"
#include "CncBoundarySpace.h"
#include "CncAnchorInfo.h"

const wxString CncAnchorName::ANCHOR_TOOL_CHANGE	= "Anchor_ToolChange";
const wxString CncAnchorName::ANCHOR_ZTOUCH			= "Anchor_ZTouch";
const wxString CncAnchorName::ANCHOR_PARKING		= "Anchor_Parking";

#define EVALUATE_PHY_ANCHOR(name) \
	if ( THE_BOUNDS &&  THE_BOUNDS->getHardwareOffset().isValid() ) \
	{ \
		auto it = find(name); \
		if ( it != end() ) \
		{ \
			const CncAnchorInfo& ai = it->second; \
			const CncDoublePosition pLog(ai.pos); \
			if ( ai.isLogically() )	pPhy = THE_BOUNDS->getHardwareOffset().transLog2Phy(pLog); \
			else					pPhy = ai.pos; \
		} \
	} \
	else \
	{ \
		CNC_CEX1_FUNCT_A(" Invalid Hardware Reference. Default anchor is used") \
	}

//////////////////////////////////////////////////
CncDoublePosition CncAnchorMap::getPhysicalAnchorToolChange() const {
//////////////////////////////////////////////////
	wxASSERT(THE_BOUNDS);
	CncDoublePosition pPhy(THE_BOUNDS->getMaxDimensionMetricX() / 2.0 , +1.0, -1.0);
	EVALUATE_PHY_ANCHOR(CncAnchorName::ANCHOR_TOOL_CHANGE)
	
	return pPhy;
}
//////////////////////////////////////////////////
CncDoublePosition CncAnchorMap::getPhysicalAnchorZTouch() const {
//////////////////////////////////////////////////
	wxASSERT(THE_BOUNDS);
	CncDoublePosition pPhy(+30.0, THE_BOUNDS->getMaxDimensionMetricY() - 30, -1.0);
	EVALUATE_PHY_ANCHOR(CncAnchorName::ANCHOR_ZTOUCH)
	
	return pPhy;
}
//////////////////////////////////////////////////
CncDoublePosition CncAnchorMap::getPhysicalAnchorParking() const {
//////////////////////////////////////////////////
	wxASSERT(THE_BOUNDS);
	CncDoublePosition pPhy(THE_BOUNDS->getMaxDimensionMetricX() / 4.0, +1.0, -1.0);
	EVALUATE_PHY_ANCHOR(CncAnchorName::ANCHOR_PARKING)
	
	return pPhy;
}
//////////////////////////////////////////////////
CncDoublePosition CncAnchorMap::getLogicalAnchorToolChange() const {
//////////////////////////////////////////////////
	wxASSERT(THE_BOUNDS);
	return THE_BOUNDS->getHardwareOffset().transPhy2Log(getPhysicalAnchorToolChange());
}
//////////////////////////////////////////////////
CncDoublePosition CncAnchorMap::getLogicalAnchorZTouch() const {
//////////////////////////////////////////////////
	wxASSERT(THE_BOUNDS);
	return THE_BOUNDS->getHardwareOffset().transPhy2Log(getPhysicalAnchorZTouch());
}
//////////////////////////////////////////////////
CncDoublePosition CncAnchorMap::getLogicalAnchorParking() const {
//////////////////////////////////////////////////
	wxASSERT(THE_BOUNDS);
	return THE_BOUNDS->getHardwareOffset().transPhy2Log(getPhysicalAnchorParking());
}


