#include "CncSimpleCalculator.h"
#include <unittest++/UnitTest++.h>

SUITE(Calculation)
{
	/////////////////////////////////////////////////////////////////////
	TEST(Calc_1) 
	{
		CncSimpleCalculator calc;
		double result; 
		
		CHECK(calc.calculate("3.14", result));					CHECK_EQUAL(result,   3.14);
		CHECK(calc.calculate("1 + 1", result));					CHECK_EQUAL(result,   2.0);
		CHECK(calc.calculate("1 + (3 - 2)", result));			CHECK_EQUAL(result,   2.0);
		CHECK(calc.calculate("1 * 3 - 2", result));				CHECK_EQUAL(result,   1.0);
		CHECK(calc.calculate("1 - 5 * 7", result));				CHECK_EQUAL(result, -34.0);
		CHECK(calc.calculate("((1 - 5) * 7.1)", result));		CHECK_EQUAL(result, -28.4);
		CHECK(calc.calculate("5 / 2", result));					CHECK_EQUAL(result,   2.5);
	}
	
	/////////////////////////////////////////////////////////////////////
	TEST(Calc_failed) 
	{
		CncSimpleCalculator calc;
		double result; 
		
		CHECK_EQUAL(calc.calculate("17 % 5 - 2", result) , false);
	}
}

