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
	if ( THE_CONFIG )	THE_CONFIG->convertMetricToSteps(offset, o);
	else				offset.zeroXYZ();
}
///////////////////////////////////////////////////////////////////	
CncLongPosition CncBoundarySpace::HardwareOriginOffset::transLog2Phy(const CncLongPosition& logicPos) const {
///////////////////////////////////////////////////////////////////	
	// transforms a logic cnc position into a corresponding physical hardware position
	CncLongPosition ret;
	
	const int32_t x = getAsStepsX() - logicPos.getX();
	const int32_t y = getAsStepsY() - logicPos.getY();
	const int32_t z = getAsStepsZ() - logicPos.getZ();

	// regarding Xmin, Ymin, Zmax as hardware reference ...
	ret.setX(x == 0 ? x : x * -1);
	ret.setY(y == 0 ? y : y * -1);
	ret.setZ(z == 0 ? z : z * -1);
	
	return ret;
}
///////////////////////////////////////////////////////////////////	
CncDoublePosition CncBoundarySpace::HardwareOriginOffset::transLog2Phy(const CncDoublePosition& logicPos) const {
///////////////////////////////////////////////////////////////////	
	// transforms a logic cnc position into a corresponding physical hardware position
	CncDoublePosition ret;
	
	const double x = getAsMetricX() - logicPos.getX();
	const double y = getAsMetricY() - logicPos.getY();
	const double z = getAsMetricZ() - logicPos.getZ();
	
	// regarding Xmin, Ymin, Zmax as hardware reference ...
	ret.setX(cnc::dblCmp::nu(x) ? x : x * -1.0);
	ret.setY(cnc::dblCmp::nu(y) ? y : y * -1.0);
	ret.setZ(cnc::dblCmp::nu(y) ? z : z * -1.0);
	
	return ret;
}
///////////////////////////////////////////////////////////////////	
CncLongPosition CncBoundarySpace::HardwareOriginOffset::transPhy2Log(const CncLongPosition& physicalPos) const {
///////////////////////////////////////////////////////////////////	
	// transforms a physical hardware position into a corresponding logic cnc position
	CncLongPosition ret;
	
	// regarding Xmin, Ymin, Zmax as hardware reference ...
	ret.setX(getAsStepsX() + physicalPos.getX());
	ret.setY(getAsStepsY() + physicalPos.getY());
	ret.setZ(getAsStepsZ() + physicalPos.getZ());
	
	return ret;
}
///////////////////////////////////////////////////////////////////	
CncDoublePosition CncBoundarySpace::HardwareOriginOffset::transPhy2Log(const CncDoublePosition& physicalPos) const {
///////////////////////////////////////////////////////////////////	
	// transforms a physical hardware position into a corresponding logic cnc position
	CncDoublePosition ret;
	
	// regarding Xmin, Ymin, Zmax as hardware reference ...
	ret.setX(getAsMetricX() + physicalPos.getX());
	ret.setY(getAsMetricY() + physicalPos.getY());
	ret.setZ(getAsMetricZ() + physicalPos.getZ());
	
	return ret;
}



///////////////////////////////////////////////////////////////////
CncBoundarySpace::CncBoundarySpace()
: maxDimensionX					(10)
, maxDimensionY					(10)
, maxDimensionZ					(10)
, hardwareOffset				()
, calculateRefPosition			({0.0, 0.0, 0.0})
, boundLogically				()
, boundPhysically				()
, refPostionMode				(CncRM_Mode1)
, measurementOffset				({0.0, 0.0, 0.0})
, workpieceThickness			(0.0)
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
const CncLongBoundaries& CncBoundarySpace::getLogicallyBoundariesAsSteps() const {
///////////////////////////////////////////////////////////////////
	CncLongPosition min;
	THE_CONFIG->convertMetricToSteps(min, boundLogically.getMinBound());
	
	CncLongPosition max;
	THE_CONFIG->convertMetricToSteps(max, boundLogically.getMaxBound());

	static CncLongBoundaries ret;
	ret.setMinBound(min);
	ret.setMaxBound(max);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
const CncLongBoundaries& CncBoundarySpace::getPhysicallyBoundariesAsSteps() const {
///////////////////////////////////////////////////////////////////
	CncLongPosition min;
	THE_CONFIG->convertMetricToSteps(min, boundPhysically.getMinBound());
	
	CncLongPosition max;
	THE_CONFIG->convertMetricToSteps(max, boundPhysically.getMaxBound());

	static CncLongBoundaries ret;
	ret.setMinBound(min);
	ret.setMaxBound(max);
	
	return ret;
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
		
	const double mdX = fabs(getMaxDimensionMetricX());
	const double mdY = fabs(getMaxDimensionMetricY());
	const double mdZ = fabs(getMaxDimensionMetricZ());
	
	const double wpt = workpieceThickness;
	const double moX = measurementOffset.getX();
	const double moY = measurementOffset.getY();
	const double moZ = measurementOffset.getZ();
	
	switch ( refPostionMode ) 
	{
		case CncRM_Unknown:		// only a place holder, nothing to do . . 
								break;
								
		case CncRM_Mode1:		calculateRefPosition	 = { 0.0, 0.0, 0.0 };
								boundLogically.setMinBound({ 0.0, 0.0, 0.0 });
								boundLogically.setMaxBound({ mdX, mdY, mdZ });
								break;
								
		case CncRM_Mode2:		calculateRefPosition	 = { 0.0, 0.0, 0.0 + fabs(moZ) };
								boundLogically.setMinBound({ 0.0, 0.0, 0.0 });
								boundLogically.setMaxBound({ mdX, mdY, mdZ });
								break;
								
		case CncRM_Mode3:		calculateRefPosition	 = { 0.0, 0.0, 0.0 + fabs(wpt) };
								boundLogically.setMinBound({ 0.0, 0.0, 0.0 });
								boundLogically.setMaxBound({ mdX, mdY, mdZ });
								break;
								
		case CncRM_Mode4:		calculateRefPosition	 = { 0.0, 0.0, 0.0 + fabs(wpt) + fabs(moZ) };
								boundLogically.setMinBound({ 0.0, 0.0, 0.0 });
								boundLogically.setMaxBound({ mdX, mdY, mdZ });
								break;
								
		case CncRM_Mode5:		calculateRefPosition	 = { 0.0, 0.0, 0.0 };
								boundLogically.setMinBound({ 0.0, 0.0, 0.0 - fabs(wpt) });
								boundLogically.setMaxBound({ mdX, mdY, mdZ - fabs(wpt) });
								break;
								
		case CncRM_Mode6:		calculateRefPosition	 = { 0.0, 0.0, 0.0 + fabs(moZ) };
								boundLogically.setMinBound({ 0.0, 0.0, 0.0 - ( fabs(wpt) + fabs(moZ) ) });
								boundLogically.setMaxBound({ mdX, mdY, mdZ - ( fabs(wpt) + fabs(moZ) ) });
								break;
								
		case CncRM_Touchblock:	calculateRefPosition	 = { 0.0 + moX, 
															 0.0 + moY, 
															 0.0 + moZ 
															};
								boundLogically.setMinBound({ 0.0 - ( fabs(wpt) + moX ), 
															 0.0 - ( fabs(wpt) + moY ), 
															 0.0 - ( fabs(wpt) + moZ ) 
															});
								boundLogically.setMaxBound({ mdX - ( fabs(wpt) + moX ),
															 mdY - ( fabs(wpt) + moY ), 
															 mdZ - ( fabs(wpt) + moZ ) 
															 });
								break;
		
		case CncRM_Camera:		calculateRefPosition	 = { 0.0 + moX, 
															 0.0 + moY, 
															 0.0 + moZ 
															};
								boundLogically.setMinBound({ 0.0 - ( fabs(wpt) + moX ), 
															 0.0 - ( fabs(wpt) + moY ), 
															 0.0 - ( fabs(wpt) + moZ ) 
															});
								boundLogically.setMaxBound({ mdX - ( fabs(wpt) + moX ),
															 mdY - ( fabs(wpt) + moY ), 
															 mdZ - ( fabs(wpt) + moZ ) 
															 });
								break;
		
	}
	
	if ( isValid() == false ) {
		std::cerr << CNC_LOG_FUNCT_A(": Invalid state!\n");
		return;
	}
	
	// normalize physically values
	const CncDoublePosition hwo(hardwareOffset.getAsMetric());
	const CncDoublePosition cor(0.0, 0.0, -THE_BOUNDS->getMaxDimensionMetricZ());
	
	boundPhysically = boundLogically;
	boundPhysically.shift(hwo);
	boundPhysically.shift(cor);
	
	if ( THE_APP != NULL )
	{
		std::stringstream ss; traceTo(ss, 4);
		THE_APP->GetBoundariesSummary()->ChangeValue(ss.str());
	}
}
///////////////////////////////////////////////////////////////////
void CncBoundarySpace::traceTo(std::ostream& o, unsigned int indent) const{
///////////////////////////////////////////////////////////////////
	const wxString prefix(' ', indent);
	
	const wxString rpMode(cnc::getReferenceModeAsString(refPostionMode));
	
	o	<< "Current Boundary Space:"																			<< std::endl
																												<< std::endl
		<< prefix << "Valid                             : " << (isValid() ? "Yes" : "No")						<< std::endl
		
		<< prefix << "Reference Pos. Mode               : " << rpMode											<< std::endl
		<< prefix << "Reference Pos.               [mm] : " << cnc::dblFormat(calculateRefPosition)				<< std::endl
		<< prefix << "Hardware Origin Offset       [mm] : " << cnc::dblFormat(hardwareOffset.getAsMetric())		<< std::endl
																												<< std::endl
		<< prefix << "Boundaries min    logically  [mm] : " << cnc::dblFormat(boundLogically.getMinBound())		<< std::endl
		<< prefix << "Boundaries centre logically  [mm] : " << cnc::dblFormat(boundLogically.getCentre())		<< std::endl
		<< prefix << "Boundaries max    logically  [mm] : " << cnc::dblFormat(boundLogically.getMaxBound())		<< std::endl
																												<< std::endl
		<< prefix << "Boundaries min    physically [mm] : " << cnc::dblFormat(boundPhysically.getMinBound())	<< std::endl
		<< prefix << "Boundaries centre physically [mm] : " << cnc::dblFormat(boundPhysically.getCentre())		<< std::endl
		<< prefix << "Boundaries max    physically [mm] : " << cnc::dblFormat(boundPhysically.getMaxBound())	<< std::endl
	;
}
///////////////////////////////////////////////////////////////////
int32_t CncBoundarySpace::getMinStepsX() const {
///////////////////////////////////////////////////////////////////
	int32_t min = (-1) * getMaxDimensionStepsX();
	
	if ( getHardwareOffset().isValid() )
		min = getHardwareOffset().getAsStepsX();
		
	return min;
}
///////////////////////////////////////////////////////////////////
int32_t CncBoundarySpace::getMinStepsY() const {
///////////////////////////////////////////////////////////////////
	int32_t min = (-1) * getMaxDimensionStepsY();
	
	if ( getHardwareOffset().isValid() )
		min = getHardwareOffset().getAsStepsY();
		
	return min;
}
///////////////////////////////////////////////////////////////////
int32_t CncBoundarySpace::getMinStepsZ() const {
///////////////////////////////////////////////////////////////////
	int32_t min = (-1) * getMaxDimensionStepsZ();

	if ( getHardwareOffset().isValid() )
	{
		// The Z origin has to be corrected from max to min because 
		// the hardware reference is located at min(x), min(y) and max(z)
		min = getHardwareOffset().getAsStepsZ() - getMaxDimensionStepsZ();
	}
	
	return min;
}
///////////////////////////////////////////////////////////////////
int32_t CncBoundarySpace::getMaxStepsX() const {
///////////////////////////////////////////////////////////////////
	int32_t max = (+1) * getMaxDimensionStepsX();

	if ( getHardwareOffset().isValid() )
		max = getMinStepsX() + getMaxDimensionStepsX();
		
	return max;
}
///////////////////////////////////////////////////////////////////
int32_t CncBoundarySpace::getMaxStepsY() const {
///////////////////////////////////////////////////////////////////
	int32_t max = (+1) * getMaxDimensionStepsY();

	if ( getHardwareOffset().isValid() )
		max = getMinStepsY() + getMaxDimensionStepsY();
		
	return max;
}
///////////////////////////////////////////////////////////////////
int32_t CncBoundarySpace::getMaxStepsZ() const {
///////////////////////////////////////////////////////////////////
	int32_t max = (+1) * getMaxDimensionStepsZ();

	if ( getHardwareOffset().isValid() )
		max = getMinStepsZ() + getMaxDimensionStepsZ();
		
	return max;
}
///////////////////////////////////////////////////////////////////
void CncBoundarySpace::setMaxDimensionStepsX(int32_t v) { 
///////////////////////////////////////////////////////////////////
	maxDimensionX = v; 
}
///////////////////////////////////////////////////////////////////
void CncBoundarySpace::setMaxDimensionStepsY(int32_t v) { 
///////////////////////////////////////////////////////////////////
	maxDimensionY = v; 
}
///////////////////////////////////////////////////////////////////
void CncBoundarySpace::setMaxDimensionStepsZ(int32_t v) { 
///////////////////////////////////////////////////////////////////
	maxDimensionZ = v; 
}
///////////////////////////////////////////////////////////////////
void CncBoundarySpace::setMaxDimensionMetricX(double v)	{ setMaxDimensionStepsX(THE_CONFIG->convertMetricToStepsX(v)); }
void CncBoundarySpace::setMaxDimensionMetricY(double v)	{ setMaxDimensionStepsY(THE_CONFIG->convertMetricToStepsY(v)); }
void CncBoundarySpace::setMaxDimensionMetricZ(double v)	{ setMaxDimensionStepsZ(THE_CONFIG->convertMetricToStepsZ(v)); }

///////////////////////////////////////////////////////////////////
double CncBoundarySpace::getMaxDimensionMetric()  const { return std::max(std::max(getMaxDimensionMetricX(), getMaxDimensionMetricY()), getMaxDimensionMetricZ()); }
double CncBoundarySpace::getMaxDimensionMetricX() const	{ return THE_CONFIG->convertStepsToMetricX(getMaxDimensionStepsX()); }
double CncBoundarySpace::getMaxDimensionMetricY() const	{ return THE_CONFIG->convertStepsToMetricY(getMaxDimensionStepsY()); }
double CncBoundarySpace::getMaxDimensionMetricZ() const	{ return THE_CONFIG->convertStepsToMetricZ(getMaxDimensionStepsZ()); }
double CncBoundarySpace::getMinMetricX() const			{ return THE_CONFIG->convertStepsToMetricX(getMinStepsX()); }
double CncBoundarySpace::getMinMetricY() const			{ return THE_CONFIG->convertStepsToMetricY(getMinStepsY()); }
double CncBoundarySpace::getMinMetricZ() const			{ return THE_CONFIG->convertStepsToMetricZ(getMinStepsZ()); }
double CncBoundarySpace::getMaxMetricX() const			{ return THE_CONFIG->convertStepsToMetricX(getMaxStepsX()); }
double CncBoundarySpace::getMaxMetricY() const			{ return THE_CONFIG->convertStepsToMetricY(getMaxStepsY()); }
double CncBoundarySpace::getMaxMetricZ() const			{ return THE_CONFIG->convertStepsToMetricZ(getMaxStepsZ()); }
///////////////////////////////////////////////////////////////////