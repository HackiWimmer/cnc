#ifndef UNIT_CALCULATOR_H
#define UNIT_CALCULATOR_H

#include <iostream>
#include <cstring>
#include <wx/string.h>

////////////////////////////////////////////////////////////////////////
class CncUnitCalculatorBase {

	public:
		enum Unit {px, pt, pc, mm, cm, m, in, ft};

		// the CSS working group dictated that one inch would be fixed to 96 pixels
		static const unsigned int DEFAULT_PPI = 96;

	protected:

		unsigned int 	PPI;

		Unit 			iUnit;
		Unit 			oUnit;

		float determineFactor() { return getFactor_UnitToPx(iUnit) * getFactor_PxToUnit(oUnit); }

	private:

		float FACT_px2pt;
		float FACT_px2pc;
		float FACT_px2mm;
		float FACT_px2cm;
		float FACT_px2m;
		float FACT_px2in;
		float FACT_px2ft;

		float FACT_pt2px;
		float FACT_pc2px;
		float FACT_mm2px;
		float FACT_cm2px;
		float FACT_m2px;
		float FACT_in2px;
		float FACT_ft2px;

	protected:

		float factor;

	public:

		///////////////////////////////////////////////////////////////////
		// in 	inches 	(1 in = 96px = 2.54 cm)
		// px  	pixels 	(1 px = 1/96   of 1 in)
		// pt 	points 	(1 pt = 1/72   of 1 in)
		// pc 	picas 	(1 pc = 12 pt)
		//
		CncUnitCalculatorBase()
		: PPI(DEFAULT_PPI)
	    , iUnit(px)
		, oUnit(iUnit)

		, FACT_px2pt( PPI      / 72.0      )
		, FACT_px2pc( PPI      / 72.0 * 12 )
		, FACT_px2mm( 25.4     / PPI       )
		, FACT_px2cm(  2.54    / PPI       )
		, FACT_px2m (  0.0254  / PPI       )
		, FACT_px2in(  1.0     / PPI       )
		, FACT_px2ft( 12.0     / PPI       )

		, FACT_pt2px( 72.0     / PPI       )
		, FACT_pc2px( 72.0     / PPI  * 12 )
		, FACT_mm2px( PPI      / 25.4      )
		, FACT_cm2px( PPI      /  2.54     )
		, FACT_m2px ( PPI      /  0.0254   )
		, FACT_in2px( PPI      /  1.0      )
		, FACT_ft2px( PPI      / 12.0      )
		, factor(determineFactor())
		{
		}

		///////////////////////////////////////////////////////////////////
		//
		CncUnitCalculatorBase(const CncUnitCalculatorBase::Unit& iu,
				              const CncUnitCalculatorBase::Unit& ou,
				              unsigned int ppi = DEFAULT_PPI)
		: PPI(ppi)
	    , iUnit(iu)
		, oUnit(ou)

		, FACT_px2pt( PPI      / 72.0      )
		, FACT_px2pc( PPI      / 72.0 * 12 )
		, FACT_px2mm( 25.4     / PPI       )
		, FACT_px2cm(  2.54    / PPI       )
		, FACT_px2m (  0.0254  / PPI       )
		, FACT_px2in(  1.0     / PPI       )
		, FACT_px2ft( 12.0     / PPI       )

		, FACT_pt2px( 72.0     / PPI       )
		, FACT_pc2px( 72.0     / PPI  * 12 )
		, FACT_mm2px( PPI      / 25.4      )
		, FACT_cm2px( PPI      /  2.54     )
		, FACT_m2px ( PPI      /  0.0254   )
		, FACT_in2px( PPI      /  1.0      )
		, FACT_ft2px( PPI      / 12.0      )
		, factor(determineFactor())
		{}

		///////////////////////////////////////////////////////////////////
		~CncUnitCalculatorBase() {}

		///////////////////////////////////////////////////////////////////
		inline float getFactor_PxToUnit(const CncUnitCalculatorBase::Unit& unit) const {
			switch( unit ) {
				case px: 		return  1.0;
				case pt:		return  FACT_px2pt;
				case pc:		return  FACT_px2pc;

				case mm: 		return  FACT_px2mm;
				case cm: 		return  FACT_px2cm;
				case m: 		return  FACT_px2m;
				case in: 		return  FACT_px2in;
				case ft: 		return  FACT_px2ft;
			}

			// should not happen
			return 1.0f;
		}

		///////////////////////////////////////////////////////////////////
		inline float getFactor_UnitToPx(const CncUnitCalculatorBase::Unit& unit) const {
			switch( unit ) {
				case px: 		return  1.0;
				case pt:		return  FACT_pt2px;
				case pc:		return  FACT_pc2px;

				case mm: 		return  FACT_mm2px;
				case cm: 		return  FACT_cm2px;
				case m: 		return  FACT_m2px;
				case in: 		return  FACT_in2px;
				case ft: 		return  FACT_ft2px;
			}

			// should not happen
			return 1.0f;
		}

		///////////////////////////////////////////////////////////////////
		static const char* getUnitAsStr(const CncUnitCalculatorBase::Unit& unit) {
			switch( unit ) {
				case px: 		return "px";
				case pt:		return "pt";
				case pc:		return "pc";

				case mm: 		return "mm";
				case cm: 		return "cm";
				case m: 		return "m";
				case in: 		return "in";
				case ft: 		return "ft";
			}

			// should not happen
			return "";
		}

		/////////////////////////////////////////////////////////////////////
		static bool determineUnit(const char* u, CncUnitCalculatorBase::Unit& unit) {
			if ( u == NULL )					 { unit = px; 	return false; }
			if ( strlen(u) <= 0 )				 { unit = px; 	return false; }

			if (      strncmp (u, "px", 2) == 0) { unit = px; 	return true; }
			else if ( strncmp (u, "pc", 2) == 0) { unit = pc; 	return true; }
			else if ( strncmp (u, "pt", 2) == 0) { unit = pt; 	return true; }
			else if ( strncmp (u, "mm", 2) == 0) { unit = mm; 	return true; }
			else if ( strncmp (u, "cm", 2) == 0) { unit = cm; 	return true; }
			else if ( strncmp (u, "m" , 1) == 0) { unit = m;	return true; }
			else if ( strncmp (u, "in", 2) == 0) { unit = in; 	return true; }
			else if ( strncmp (u, "ft", 2) == 0) { unit = ft; 	return true; }


			std::cerr 	<< "CncUnitCalculator::determineUnit: Unsupported unit: "
						<< u
						<< std::endl;

			return false;
		}

		///////////////////////////////////////////////////////////////////
		static unsigned int getDefaultPPI()					 		{ return DEFAULT_PPI; }

		///////////////////////////////////////////////////////////////////
		unsigned int getPPI()	 							const 	{ return PPI; }
		float getFactor()									const 	{ return getFactor_UnitToPx(iUnit) * getFactor_PxToUnit(oUnit); }

		///////////////////////////////////////////////////////////////////
		const CncUnitCalculatorBase::Unit getInputUnit() 	const	{ return iUnit; }
		const CncUnitCalculatorBase::Unit getOutputUnit() 	const	{ return oUnit; }

		///////////////////////////////////////////////////////////////////
		const char* getInputUnitAsStr() 					const 	{ return getUnitAsStr(iUnit); }
		const char* getOutputUnitAsStr()				 	const	{ return getUnitAsStr(oUnit); }


		///////////////////////////////////////////////////////////////////
		float getFact_px2pt()  								const	{ return FACT_px2pt; }
		float getFact_px2pc()								const	{ return FACT_px2pc; }
		float getFact_px2mm()								const  	{ return FACT_px2mm; }
		float getFact_px2cm()								const	{ return FACT_px2cm; }
		float getFact_px2m()								const	{ return FACT_px2m;  }
		float getFact_px2in()								const	{ return FACT_px2in; }
		float getFact_px2ft()								const	{ return FACT_px2ft; }

		float getFact_pt2px()								const	{ return FACT_pt2px; }
		float getFact_pc2px()								const	{ return FACT_pc2px; }
		float getFact_mm2px()								const	{ return FACT_mm2px; }
		float getFact_cm2px()								const	{ return FACT_cm2px; }
		float getFact_m2px()								const	{ return FACT_m2px ; }
		float getFact_in2px()								const	{ return FACT_in2px; }
		float getFact_ft2px()								const	{ return FACT_ft2px; }

		///////////////////////////////////////////////////////////////////
		static float getFact_in2mm()								{ return 25.4;       }

		////////////////////////////////////////////////////////////////
		friend std::ostream &operator<< (std::ostream &ostr, const CncUnitCalculatorBase &a) {
			ostr << "CncUnitCalaculatorBase("
				 << "From: '" 			<< a.getInputUnitAsStr()
			     << "' to: '"  			<< a.getOutputUnitAsStr()
				 << "', with: FACT = " 	<< a.getFactor()
				 << ", PPI = "			<< a.getPPI()
			     << ")";
			return ostr;
		}

		///////////////////////////////////////////////////////////////////
		static void trace(std::ostream& os) {
			CncUnitCalculatorBase uc;

			#define TRACE_FACT(value) \
				wxString::Format(wxT("%+ 6.9lf"), value)

			os << " PPI                     : " << uc.getPPI() 		 			  << std::endl;

			os << " Factor PX to PT         : " << TRACE_FACT(uc.getFact_px2pt()) << std::endl;
			os << " Factor PX to PC         : " << TRACE_FACT(uc.getFact_px2pc()) << std::endl;
			os << " Factor PX to MM         : " << TRACE_FACT(uc.getFact_px2mm()) << std::endl;
			os << " Factor PX to CM         : " << TRACE_FACT(uc.getFact_px2cm()) << std::endl;
			os << " Factor PX to  M         : " << TRACE_FACT(uc.getFact_px2m())  << std::endl;
			os << " Factor PX to IN         : " << TRACE_FACT(uc.getFact_px2in()) << std::endl;
			os << " Factor PX to FT         : " << TRACE_FACT(uc.getFact_px2ft()) << std::endl;

			os << " Factor PT to PX         : " << TRACE_FACT(uc.getFact_pt2px()) << std::endl;
			os << " Factor PC to PX         : " << TRACE_FACT(uc.getFact_pc2px()) << std::endl;
			os << " Factor MM to PX         : " << TRACE_FACT(uc.getFact_mm2px()) << std::endl;
			os << " Factor CM to PX         : " << TRACE_FACT(uc.getFact_cm2px()) << std::endl;
			os << " Factor M  to PX         : " << TRACE_FACT(uc.getFact_m2px())  << std::endl;
			os << " Factor IN to PX         : " << TRACE_FACT(uc.getFact_in2px()) << std::endl;
			os << " Factor FT to PX         : " << TRACE_FACT(uc.getFact_ft2px()) << std::endl;

			#undef TRACE_FACT
		}

};

////////////////////////////////////////////////////////////////////////
template <class T>
class CncUnitCalculator : public CncUnitCalculatorBase {

	public:
		///////////////////////////////////////////////////////////////////
		CncUnitCalculator()
		: CncUnitCalculatorBase()
		{}

		///////////////////////////////////////////////////////////////////
		CncUnitCalculator(const CncUnitCalculatorBase::Unit& iu,
	                      const CncUnitCalculatorBase::Unit& ou,
	                      unsigned int ppi = DEFAULT_PPI)
		: CncUnitCalculatorBase(iu, ou, ppi)
		{}
		
		///////////////////////////////////////////////////////////////////
		~CncUnitCalculator()
		{}
		
		///////////////////////////////////////////////////////////////////
		const T convert(T value) const {
			if ( iUnit == oUnit )
				return value;
		
			return value * factor;
		}
		
		///////////////////////////////////////////////////////////////////
		bool unitsAreDifferent() {
			return iUnit != oUnit;
		}
		
		///////////////////////////////////////////////////////////////////
		void changeInputUnit(const CncUnitCalculatorBase::Unit& iu) {
			iUnit = iu;
			factor = determineFactor();
		}

		///////////////////////////////////////////////////////////////////
		void changeOutputUnit(const CncUnitCalculatorBase::Unit& ou) {
			oUnit = ou;
			factor = determineFactor();
		}

		///////////////////////////////////////////////////////////////////
		void changeUnits(const CncUnitCalculatorBase::Unit& iu,
				         const CncUnitCalculatorBase::Unit& ou) {
			iUnit = iu;
			oUnit = ou;
			factor = determineFactor();
		}

};

struct UC {

	static CncUnitCalculator<float>* PX_TO_MM;
	static CncUnitCalculator<float>* IN_TO_MM;
	static CncUnitCalculator<float>* MM_TO_PX;
	static CncUnitCalculator<float>* MM_TO_IN;

	static float CNV_PX_TO_MM(const float v) { wxASSERT(PX_TO_MM); return PX_TO_MM->convert(v); }
	static float CNV_IN_TO_MM(const float v) { wxASSERT(IN_TO_MM); return IN_TO_MM->convert(v); }
	static float CNV_MM_TO_PX(const float v) { wxASSERT(MM_TO_PX); return MM_TO_PX->convert(v); }
	static float CNV_MM_TO_IN(const float v) { wxASSERT(MM_TO_IN); return MM_TO_IN->convert(v); }
};


////////////////////////////////////////////////////////////////////////
struct CncUnitCalculatorTest {

	static void test() {

		#define CONVERT_AND_TRACE(value) \
			std::cout << value << uc.getInputUnitAsStr() << "\t=\t" << uc.convert(value) << uc.getOutputUnitAsStr() << std::endl; 

		typedef CncUnitCalculatorBase::Unit Unit;

		{
			CncUnitCalculator<double> uc(Unit::px, Unit::mm);
			std::cout << uc << std::endl;;
			CONVERT_AND_TRACE(  1.0)
			CONVERT_AND_TRACE( 10.0)
			CONVERT_AND_TRACE(112.0)
			CONVERT_AND_TRACE(  2.4)
		}
		{
			CncUnitCalculator<float> uc(Unit::px, Unit::mm);
			std::cout << uc << std::endl;;
			CONVERT_AND_TRACE(  1.0)
			CONVERT_AND_TRACE( 10.0)
			CONVERT_AND_TRACE(112.0)
			CONVERT_AND_TRACE(  2.4)
		}
		{
			CncUnitCalculator<float> uc(Unit::mm, Unit::cm);
			std::cout << uc << std::endl;;
			CONVERT_AND_TRACE(  1.0)
			CONVERT_AND_TRACE( 10.0)
			CONVERT_AND_TRACE(112.0)
			CONVERT_AND_TRACE(  2.4)
		}
		{
			CncUnitCalculator<float> uc(Unit::mm, Unit::mm);
			std::cout << uc << std::endl;;
			CONVERT_AND_TRACE(  1.0)
			CONVERT_AND_TRACE( 10.0)
			CONVERT_AND_TRACE(112.0)
			CONVERT_AND_TRACE(  2.4)
		}
		{
			CncUnitCalculator<float> uc = *UC::PX_TO_MM;
			std::cout << uc << std::endl;
			CONVERT_AND_TRACE((double)  1.0)
			CONVERT_AND_TRACE((double) 10.0)
			CONVERT_AND_TRACE((double)112.0)
			CONVERT_AND_TRACE((double)  2.4)
		}
		{
			CncUnitCalculator<double> uc(Unit::px, Unit::mm);
			std::cout << uc << std::endl;
			CONVERT_AND_TRACE((double)  1.0)
			CONVERT_AND_TRACE((double) 10.0)
			CONVERT_AND_TRACE((double)112.0)
			CONVERT_AND_TRACE((double)  2.4)
			
			uc.changeOutputUnit(Unit::in);
			std::cout << uc << std::endl;
			CONVERT_AND_TRACE((double) 96.0)
			CONVERT_AND_TRACE((double) 10.0)
			CONVERT_AND_TRACE((double)112.0)
			CONVERT_AND_TRACE((double)  2.4)
			
			uc.changeOutputUnit(Unit::px);
			std::cout << uc << std::endl;
			CONVERT_AND_TRACE((double)  1.0)
			CONVERT_AND_TRACE((double) 10.0)
			CONVERT_AND_TRACE((double)112.0)
			CONVERT_AND_TRACE((double)  2.4)
			
			uc.changeInputUnit(Unit::in);
			std::cout << uc << std::endl;
			CONVERT_AND_TRACE((double)  1.0)
			CONVERT_AND_TRACE((double) 10.0)
			CONVERT_AND_TRACE((double)112.0)
			CONVERT_AND_TRACE((double)  2.4)
		}
		{
			std::cout << std::endl;
			std::cout << UC::CNV_PX_TO_MM(96) << std::endl;
		}

		#undef CONVERT_AND_TRACE
	}
};

#endif