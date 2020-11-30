#include "CncConfig.h"
#include "MainFrame.h"
#include "CncBoundarySpace.h"

///////////////////////////////////////////////////////////////////
CncBoundarySpace::HardwareOriginOffset::HardwareOriginOffset() 
: offset		(0, 0, 0)
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
CncBoundarySpace::HardwareOriginOffset::HardwareOriginOffset(const HardwareOriginOffset& hwo) 
: offset		(hwo.getAsSteps())
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
void CncBoundarySpace::HardwareOriginOffset::reset() { 
///////////////////////////////////////////////////////////////////
	*this = HardwareOriginOffset(); 
}
///////////////////////////////////////////////////////////////////
double CncBoundarySpace::HardwareOriginOffset::getAsMetricX() const { 
///////////////////////////////////////////////////////////////////
	return CncConfig::available() ? THE_CONFIG->convertStepsToMetricX(offset.getX()) : 0.0; 
}
///////////////////////////////////////////////////////////////////
double CncBoundarySpace::HardwareOriginOffset::getAsMetricY() const { 
///////////////////////////////////////////////////////////////////
	return CncConfig::available() ? THE_CONFIG->convertStepsToMetricY(offset.getY()) : 0.0; 
}
///////////////////////////////////////////////////////////////////
double CncBoundarySpace::HardwareOriginOffset::getAsMetricZ() const { 
///////////////////////////////////////////////////////////////////
	return CncConfig::available() ? THE_CONFIG->convertStepsToMetricZ(offset.getZ()) : 0.0; 
}
///////////////////////////////////////////////////////////////////
CncDoubleDistance CncBoundarySpace::HardwareOriginOffset::getAsMetric() const { 
///////////////////////////////////////////////////////////////////
	CncDoublePosition ret; 
	
	if ( CncConfig::available() )
		THE_CONFIG->convertStepsToMetric(ret, offset);
		
	return ret;
}
///////////////////////////////////////////////////////////////////
void CncBoundarySpace::HardwareOriginOffset::set(int32_t px, int32_t py, int32_t pz) {
///////////////////////////////////////////////////////////////////
	offset.setXYZ(px, py, pz);
}
///////////////////////////////////////////////////////////////////
void CncBoundarySpace::HardwareOriginOffset::set(double px, double py, double pz) {
///////////////////////////////////////////////////////////////////
	const int32_t PX = THE_CONFIG ? THE_CONFIG->convertMetricToStepsX(px) : 0;
	const int32_t PY = THE_CONFIG ? THE_CONFIG->convertMetricToStepsY(py) : 0;
	const int32_t PZ = THE_CONFIG ? THE_CONFIG->convertMetricToStepsZ(pz) : 0;
	offset.setXYZ(PX, PY, PZ);
}
///////////////////////////////////////////////////////////////////
void CncBoundarySpace::HardwareOriginOffset::set(const CncLongPosition& o) {
///////////////////////////////////////////////////////////////////
	offset = o;
}
///////////////////////////////////////////////////////////////////
void CncBoundarySpace::HardwareOriginOffset::set(const CncDoublePosition& o) {
///////////////////////////////////////////////////////////////////	
	if ( CncConfig::available() )	THE_CONFIG->convertMetricToSteps(offset, o);
	else							offset.zeroXYZ();
}



///////////////////////////////////////////////////////////////////
CncBoundarySpace::CncBoundarySpace()
: hardwareOffset				()
, calculateRefPosition			({0.0, 0.0, 0.0})
, boundLocically				()
, boundPhysically				()
, refPostionMode				(CncRM_Mode1)
, workpieceThickness			(0.0)
, measurePlateThickness			(0.0)
///////////////////////////////////////////////////////////////////
{
	calculate();
}
///////////////////////////////////////////////////////////////////
bool CncBoundarySpace::isValid() const {
///////////////////////////////////////////////////////////////////
	return refPostionMode != CncRM_Unknown;
}
///////////////////////////////////////////////////////////////////
const CncLongPosition CncBoundarySpace::getCalculatedRefPositionSteps() const {
///////////////////////////////////////////////////////////////////
	CncLongPosition ret;
	return THE_CONFIG->convertMetricToSteps(ret, calculateRefPosition);
}
///////////////////////////////////////////////////////////////////
const CncDoublePosition CncBoundarySpace::getCalculatedRefPositionMetric() const {
///////////////////////////////////////////////////////////////////
	return calculateRefPosition;
}
///////////////////////////////////////////////////////////////////
bool CncBoundarySpace::hasHardwareOffset() const {
///////////////////////////////////////////////////////////////////
	return hardwareOffset.getAsSteps().isEqual({0, 0, 0}) == false;
}
///////////////////////////////////////////////////////////////////
bool CncBoundarySpace::isPositionInside(const CncLongPosition& p) const {
///////////////////////////////////////////////////////////////////
	CncDoublePosition ret;
	THE_CONFIG->convertStepsToMetric(ret, p);
	
	return isPositionInside(ret);
}
///////////////////////////////////////////////////////////////////
bool CncBoundarySpace::isPositionInside(const CncDoublePosition& p)	const {
///////////////////////////////////////////////////////////////////
	return boundPhysically.fitsInside(p); 
}
///////////////////////////////////////////////////////////////////
void CncBoundarySpace::calculate() {
//
// Mode		minZ			maxZ					ref X	ref Y	ref Z
//  1		0,00			+maxZDist				0,00	0,00	0,00
//  2		0,00			+maxZDist				0,00	0,00	+mpt
//  3		0,00			+maxZDist				0,00	0,00	+wpt
//  4		0,00			+maxZDist				0,00	0,00	+(wpt + mpt)
//  5		-wpt			+maxZDist-wpt			0,00	0,00	0,00
//  6		-(wpt + mpt)	+maxZDist-(wpt+mpt)		0,00	0,00	+mpt
///////////////////////////////////////////////////////////////////
	
	if ( CncConfig::available() == false )
		return;
	
	const double mdX = THE_CONFIG->getMaxDimensionX();
	const double mdY = THE_CONFIG->getMaxDimensionY();
	const double mdZ = THE_CONFIG->getMaxDimensionZ();
	
	measurePlateThickness = THE_CONFIG->getMeasurePlateThickness();
	
	switch ( refPostionMode ) {
		case CncRM_Unknown:	// only a placeholder, nothing to do . . 
							break;
							
		case CncRM_Mode1:	calculateRefPosition	 = { 0.0, 0.0, 0.0 };
							boundLocically.setMinBound({ 0.0, 0.0, 0.0 });
							boundLocically.setMaxBound({ mdX, mdY, mdZ });
							break;
							
		case CncRM_Mode2:	calculateRefPosition	 = { 0.0, 0.0, 0.0 + fabs(measurePlateThickness) };
							boundLocically.setMinBound({ 0.0, 0.0, 0.0 });
							boundLocically.setMaxBound({ mdX, mdY, mdZ });
							break;
							
		case CncRM_Mode3:	calculateRefPosition	 = { 0.0, 0.0, 0.0 + fabs(workpieceThickness) };
							boundLocically.setMinBound({ 0.0, 0.0, 0.0 });
							boundLocically.setMaxBound({ mdX, mdY, mdZ });
							break;
							
		case CncRM_Mode4:	calculateRefPosition	 = { 0.0, 0.0, 0.0 + fabs(workpieceThickness) + fabs(measurePlateThickness) };
							boundLocically.setMinBound({ 0.0, 0.0, 0.0 });
							boundLocically.setMaxBound({ mdX, mdY, mdZ });
							break;
							
		case CncRM_Mode5:	calculateRefPosition	 = { 0.0, 0.0, 0.0 };
							boundLocically.setMinBound({ 0.0, 0.0, 0.0 - fabs(workpieceThickness) });
							boundLocically.setMaxBound({ mdX, mdY, mdZ - fabs(workpieceThickness) });
							break;
							
		case CncRM_Mode6:	calculateRefPosition	 = { 0.0, 0.0, 0.0 + fabs(measurePlateThickness)};
							boundLocically.setMinBound({ 0.0, 0.0, 0.0 - ( fabs(workpieceThickness) + fabs(measurePlateThickness) ) });
							boundLocically.setMaxBound({ mdX, mdY, mdZ - ( fabs(workpieceThickness) + fabs(measurePlateThickness) ) });
							break;
	}
	
	if ( isValid() == false ) {
		std::cerr << CNC_LOG_FUNCT_A(" Invalid state!\n");
		return;
	}
	
	// normalize physically values
	CncDoublePosition hwo(hardwareOffset.getAsMetric());
	boundPhysically = boundLocically;
	boundPhysically.shift(hwo);
	
	if ( THE_APP != NULL ) {
		std::stringstream ss; traceTo(ss, 4);
		THE_APP->GetBounderiesSummary()->ChangeValue(ss.str());
	}
}
///////////////////////////////////////////////////////////////////
void CncBoundarySpace::traceTo(std::ostream& o, unsigned int indent) const{
///////////////////////////////////////////////////////////////////
	const wxString prefix(' ', indent);
	
	o	<< "Current Boundary Space:"																		<< std::endl
																											<< std::endl
		<< prefix << "Valid                          : " << (isValid() ? "Yes" : "No")						<< std::endl
		<< prefix << "Reference Pos. Mode            : " << (int)refPostionMode								<< std::endl
		<< prefix << "Reference Pos.            [mm] : " << cnc::dblFormat(calculateRefPosition)			<< std::endl
		<< prefix << "Hardware Origin Offset    [mm] : " << cnc::dblFormat(hardwareOffset.getAsMetric())	<< std::endl
		<< prefix << "Bounderies min locally    [mm] : " << cnc::dblFormat(boundLocically.getMinBound())	<< std::endl
		<< prefix << "Bounderies max locally    [mm] : " << cnc::dblFormat(boundLocically.getMaxBound())	<< std::endl
		<< prefix << "Bounderies min physically [mm] : " << cnc::dblFormat(boundPhysically.getMinBound())	<< std::endl
		<< prefix << "Bounderies max physically [mm] : " << cnc::dblFormat(boundPhysically.getMaxBound())	<< std::endl
	;
}

