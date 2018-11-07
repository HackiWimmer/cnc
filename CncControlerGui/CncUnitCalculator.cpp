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

