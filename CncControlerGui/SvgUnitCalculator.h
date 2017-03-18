#ifndef SVG_UNIT_CALC_H
#define SVG_UNIT_CALC_H

#include <iostream>
#include <wx/string.h>
#include "CncCommon.h"

enum SVGUnit {unknown, px, pt, pc, mm, cm, m, in, ft};
enum SVGReferenceUnit {PX = px, MM = mm};

class SvgUnitCalculator {
	
	private:
		static SVGReferenceUnit	_referenceUnit;
		static double 			_returnValue;
		static const char*		_unknown;
	
		// dpi = 90 is defined by the most SVG tools like inkscape,
		// it can be set by the setDPI(...) method, otherwise it will be 
		// _DPI_DEFAULT by default.
		static const unsigned int	_DPI_DEFAULT = 90;
		static unsigned int 		_DPI;
		static double				_DPI_FACT_NULL;
		static double				_DPI_FACT_PX;
		static double				_DPI_FACT_PT;
		static double				_DPI_FACT_PC;
		static double				_DPI_FACT_MM;
		static double				_DPI_FACT_CM;
		static double				_DPI_FACT_M;
		static double				_DPI_FACT_IN;
		static double				_DPI_FACT_FT;
		
	public:
	
		/////////////////////////////////////////////////////////////////////
		static void init(const SVGReferenceUnit& ru, const unsigned int dpi = _DPI_DEFAULT) {
			setDPI(dpi);
			setReferenceUnit(ru);
		}

		/////////////////////////////////////////////////////////////////////
		static void setReferenceUnit(const SVGReferenceUnit& ru);
		
		/////////////////////////////////////////////////////////////////////
		static void setDPI(const unsigned int dpi) {
			_DPI = dpi ? dpi : _DPI_DEFAULT;
		}
		
		/////////////////////////////////////////////////////////////////////
		static const unsigned int getDPI() {
			return _DPI;
		}

		/////////////////////////////////////////////////////////////////////
		static bool isUnitValid(const SVGUnit& unit) {
			switch ( unit ) {
				case px:
				case pt:
				case pc:
				case mm:
				case cm:
				case m:
				case in: 
				case ft: return true;
				default: std::cerr << "SvgUnitCalculator::isUnitValid: Unsupported svg unit: " << unit << std::endl;
			}
			return false;
		}
		
		/////////////////////////////////////////////////////////////////////
		static double calculateUnit2MM(const SVGUnit& from, double value) {
			convertUnit2MM(from, _returnValue);
			return _returnValue;
		}
		
		/////////////////////////////////////////////////////////////////////
		static double convertUnit2MM(const SVGUnit& from, double& value) {
			value *= 1/getFactorMM2Input(from);
			return value;
		}
		
		/////////////////////////////////////////////////////////////////////
		static double calculateMM2Unit(const SVGUnit& from, double value) {
			convertMM2Unit(from, _returnValue);
			return _returnValue;
		}
		
		/////////////////////////////////////////////////////////////////////
		static double convertMM2Unit(const SVGUnit& from, double& value) {
			value *= getFactorMM2Input(from);
			return value;
		}
		
		/////////////////////////////////////////////////////////////////////
		static double calculateReferenceUnit2MM(double value) {
			convertReferenceUnit2MM(_returnValue);
			return _returnValue;
		}
		
		/////////////////////////////////////////////////////////////////////
		static double convertReferenceUnit2MM(double value) {
			value *= getFactorReferenceUnit2MM();
			return value;
		}
		
		/////////////////////////////////////////////////////////////////////
		static double convertMM2ReferenceUnit(double value) {
			value /= getFactorReferenceUnit2MM();
			return value;
		}
		
		/////////////////////////////////////////////////////////////////////
		static double getFactorReferenceUnit2MM() {
			return _DPI_FACT_MM;
		}
		
		/////////////////////////////////////////////////////////////////////
		static double getFactorMM2Input(const SVGUnit& from) {
			switch ( from ) {
				case px: 		return _DPI_FACT_PX;
				case pt: 		return _DPI_FACT_PT;
				case pc: 		return _DPI_FACT_PC;
				case mm: 		return _DPI_FACT_MM;
				case cm:		return _DPI_FACT_CM;
				case m:  		return _DPI_FACT_M;
				case in: 		return _DPI_FACT_IN;
				case ft: 		return _DPI_FACT_FT;
				case unknown:	return _DPI_FACT_NULL;
				default: std::cerr << "SvgUnitCalculator::calculate2mm: Unsupported svg unit: " << from << std::endl;
			}
			return _DPI_FACT_NULL;
			
		}
		
		/////////////////////////////////////////////////////////////////////
		static SVGUnit determineUnit(const wxString& u) {
			if (      u.StartsWith("px") )		return px;
			else if ( u.StartsWith("pc") )		return pc;
			else if ( u.StartsWith("pt") )		return pt;
			else if ( u.StartsWith("mm") )		return mm;
			else if ( u.StartsWith("cm") )		return cm;
			else if ( u.StartsWith("m") )		return m;
			else if ( u.StartsWith("in") )		return in;
			else if ( u.StartsWith("ft") )		return ft;
			else if ( u.StartsWith(_unknown) )	return unknown;
			else 							std::cerr << "SvgUnitCalculator::determineUnit: Unsupported svg unit: " << u << std::endl;
			
			return unknown;
		}
		
		///////////////////////////////////////////////////////////////////
		static const char* getUnitAsStr(const SVGUnit& unit) {
			switch( unit ) {
				case unknown: 	return _unknown;
				case px: 		return "px";
				case pt:		return "pt";
				case pc:		return "pc";
				case mm: 		return "mm";
				case cm: 		return "cm";
				case m: 		return "m";
				case in: 		return "in";
				case ft: 		return "ft";
				default:		std::cerr << "SvgUnitCalculator::getUnitAsStr: Unsupported svg unit: " << unit << std::endl;
			}
			return _unknown;
		}
		
		///////////////////////////////////////////////////////////////////
		static double getFactorPx2Unit(const SVGUnit& unit) {
			switch( unit ) {
				case unknown: 	return _DPI_FACT_NULL;
				case px: 		return 1.0;
				case pt:		return 1.25;
				case pc:		return 15.0;
				case mm: 		return 25.4   / _DPI;
				case cm: 		return 2.54   / _DPI;
				case m: 		return 0.0254 / _DPI;
				case in: 		return 1.0    / _DPI;
				case ft: 		return 12.0   / _DPI;
				default:		std::cerr << "SvgUnitCalculator::getFactorPx2Unit: Unsupported svg unit: " << unit << std::endl;
			}
			return _DPI_FACT_NULL;
		}
		
		///////////////////////////////////////////////////////////////////
		static double getFactorMM2Unit(const SVGUnit& unit) {
			switch( unit ) {
				case unknown: 	return _DPI_FACT_NULL;
				case px: 		return 1.00	* _DPI / 25.4;
				case pt:		return 1.25	* _DPI / 25.4;
				case pc:		return 15.0	* _DPI / 25.4;
				case mm: 		return 1.0;
				case cm: 		return 0.1;
				case m: 		return 0.001;
				case in: 		return 0.0393701;
				case ft: 		return 0.00328082;
				default:		std::cerr << "SvgUnitCalculator::getFactorMM2Unit: Unsupported svg unit: " << unit << std::endl;
			}
			return _DPI_FACT_NULL;
		}
		
		///////////////////////////////////////////////////////////////////
		static const char* getReferenceUnitAsStr(const SVGReferenceUnit& unit) {
			switch( unit ) {
				case PX:	return "px";
				case MM: 	return "mm";
				default:	std::cerr << "SvgUnitCalculator::getReferenceUnitAsStr: Unsupported reference unit: " << unit << std::endl;
			}
			
			return _unknown;
		}
		
		///////////////////////////////////////////////////////////////////
		static const char* getReferenceUnitAsStr() {
			return getReferenceUnitAsStr(_referenceUnit);
		}
		
		///////////////////////////////////////////////////////////////////
		static void trace(std::ostream& os) {
			os << "SvgUnitCalculator:" << std::endl;
			os << " DPI                        : " << getDPI() << std::endl;
			os << " SVG Reference Unit         : " << getReferenceUnitAsStr() << std::endl;
			os << " Factor PX                  : " << wxString::Format(wxT(" %4.9f"), _DPI_FACT_PX) << std::endl;
			os << " Factor PT                  : " << wxString::Format(wxT(" %4.9f"), _DPI_FACT_PT) << std::endl;
			os << " Factor PC                  : " << wxString::Format(wxT("%4.9f"), _DPI_FACT_PC)  << std::endl;
			os << " Factor MM                  : " << wxString::Format(wxT(" %4.9f"), _DPI_FACT_MM) << std::endl;
			os << " Factor CM                  : " << wxString::Format(wxT(" %4.9f"), _DPI_FACT_CM) << std::endl;
			os << " Factor M                   : " << wxString::Format(wxT(" %4.9f"), _DPI_FACT_M)  << std::endl;
			os << " Factor IN                  : " << wxString::Format(wxT(" %4.9f"), _DPI_FACT_IN) << std::endl;
			os << " Factor FT                  : " << wxString::Format(wxT(" %4.9f"), _DPI_FACT_FT) << std::endl;
		}
};

#endif