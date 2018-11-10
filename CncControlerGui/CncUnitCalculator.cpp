#include "CncUnitCalculator.h"

typedef CncUnitCalculatorBase::Unit Unit;

// create global Unit calculators
	CncUnitCalculator<float> px_to_mm(Unit::px, Unit::mm);
	CncUnitCalculator<float>* UC::PX_TO_MM = &px_to_mm;

	CncUnitCalculator<float> in_to_mm(Unit::px, Unit::mm);
	CncUnitCalculator<float>* UC::IN_TO_MM = &px_to_mm;

	CncUnitCalculator<float> mm_to_px(Unit::px, Unit::mm);
	CncUnitCalculator<float>* UC::MM_TO_PX = &px_to_mm;

	CncUnitCalculator<float> mm_to_in(Unit::px, Unit::mm);
	CncUnitCalculator<float>* UC::MM_TO_IN = &px_to_mm;

////////////////////////////////////////////////////////////////////////
void CncUnitCalculatorTest::test() {
////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////
void CncResolutionCalculatorTest::test() {
////////////////////////////////////////////////////////////////////////
	typedef CncUnitCalculatorBase::Unit Unit;
	
	#define DETERMINE_AND_TRACE(unit, ppi) \
		std::cout << CncUnitCalculatorBase::getUnitAsStr(unit) << ",\t"<<  ppi << "\t=\t"; \
		std::cout << CncResolutionCalculator::getResolutionForUnit(unit, ppi); \
		std::cout << " " << CncUnitCalculatorBase::getUnitAsStr(unit) << std::endl;
		
		{
			unsigned int ppi = 96;
			DETERMINE_AND_TRACE(Unit::in, ppi)
			DETERMINE_AND_TRACE(Unit::mm, ppi)
			DETERMINE_AND_TRACE(Unit::cm, ppi)
			DETERMINE_AND_TRACE(Unit::m,  ppi)
			DETERMINE_AND_TRACE(Unit::ft, ppi)
			
			DETERMINE_AND_TRACE(Unit::px, ppi)
			DETERMINE_AND_TRACE(Unit::pc, ppi)
			DETERMINE_AND_TRACE(Unit::pt, ppi)
		}
		{
			unsigned int ppi = 126;
			DETERMINE_AND_TRACE(Unit::in, ppi)
			DETERMINE_AND_TRACE(Unit::mm, ppi)
			DETERMINE_AND_TRACE(Unit::cm, ppi)
			DETERMINE_AND_TRACE(Unit::m,  ppi)
			DETERMINE_AND_TRACE(Unit::ft, ppi)
			
			DETERMINE_AND_TRACE(Unit::px, ppi)
			DETERMINE_AND_TRACE(Unit::pc, ppi)
			DETERMINE_AND_TRACE(Unit::pt, ppi)

		}
		{
			unsigned int ppi = 300;
			DETERMINE_AND_TRACE(Unit::in, ppi)
			DETERMINE_AND_TRACE(Unit::mm, ppi)
			DETERMINE_AND_TRACE(Unit::cm, ppi)
			DETERMINE_AND_TRACE(Unit::m,  ppi)
			DETERMINE_AND_TRACE(Unit::ft, ppi)
			
			DETERMINE_AND_TRACE(Unit::px, ppi)
			DETERMINE_AND_TRACE(Unit::pc, ppi)
			DETERMINE_AND_TRACE(Unit::pt, ppi)
		}

	#undef DETERMINE_AND_TRACE
}

