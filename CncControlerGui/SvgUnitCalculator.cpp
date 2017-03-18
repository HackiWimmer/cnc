#include "SvgUnitCalculator.h"

/////////////////////////////////////////////////////////////////////
// Initialize static SvgUnitCalculator values
/////////////////////////////////////////////////////////////////////
unsigned int SvgUnitCalculator::_DPI				= _DPI_DEFAULT;
const char* SvgUnitCalculator::_unknown 			= "unknown";
SVGReferenceUnit SvgUnitCalculator::_referenceUnit	= MM;

double SvgUnitCalculator::_returnValue 				= 1;

double SvgUnitCalculator::_DPI_FACT_NULL			= 0.0;
double SvgUnitCalculator::_DPI_FACT_PX				= _DPI_FACT_NULL;
double SvgUnitCalculator::_DPI_FACT_PT				= _DPI_FACT_NULL;
double SvgUnitCalculator::_DPI_FACT_PC				= _DPI_FACT_NULL;
double SvgUnitCalculator::_DPI_FACT_MM				= _DPI_FACT_NULL;
double SvgUnitCalculator::_DPI_FACT_CM				= _DPI_FACT_NULL;
double SvgUnitCalculator::_DPI_FACT_M				= _DPI_FACT_NULL;
double SvgUnitCalculator::_DPI_FACT_IN				= _DPI_FACT_NULL;
double SvgUnitCalculator::_DPI_FACT_FT				= _DPI_FACT_NULL;

/////////////////////////////////////////////////////////////////////
class SvgUnitCalculatorInitializer {
/////////////////////////////////////////////////////////////////////
	public:
		SvgUnitCalculatorInitializer() {
			// init defaut values, later init can be called from anywhere
			SvgUnitCalculator::init(PX, 90);
		}
}; SvgUnitCalculatorInitializer __SUCI;

/////////////////////////////////////////////////////////////////////
void SvgUnitCalculator::setReferenceUnit(const SVGReferenceUnit& ru) {
/////////////////////////////////////////////////////////////////////
	// _referenceUnit defines the unit where the svg elements are coded with
	// by default this will be px, but to support the direct coding in mm 
	// the following switch is placed.
	// Note: The factor is always defined as _referenceUnit to mm!
	
	_referenceUnit = ru;
	
	switch ( _referenceUnit ) {
		case MM:	_DPI_FACT_PX	= 1.00	/ _DPI / 25.4;
					_DPI_FACT_PT	= 1.25	/ _DPI / 25.4;
					_DPI_FACT_PC	= 15.0	/ _DPI / 25.4;
					_DPI_FACT_MM	= 1.00;
					_DPI_FACT_CM	= 0.1;
					_DPI_FACT_M	    = 0.001;
					_DPI_FACT_IN	= 0.0393701;
					_DPI_FACT_FT	= 0.00328082;
					break;
					
		case PX:	_DPI_FACT_PX	= 1.00;
					_DPI_FACT_PT	= 1.25;
					_DPI_FACT_PC	= 15.0;
					_DPI_FACT_MM	= 25.4   / _DPI;
					_DPI_FACT_CM	= 2.54   / _DPI;
					_DPI_FACT_M		= 0.0254 / _DPI;
					_DPI_FACT_IN	= 1.0    / _DPI;
					_DPI_FACT_FT	= 12.0   / _DPI;
					break;
	}
}


