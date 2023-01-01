#include <iostream>
#include "CncCommon.h"
#include "CncPosition.h"

#include <unittest++/UnitTest++.h>

SUITE(Position)
{
	/////////////////////////////////////////////////////////////////////
	TEST(Set_1) 
	{
		CncDoublePosition p3;
		p3.setScaling(0.5);
		p3.setTranslation(1.1, 2.2, 3.3);
		p3.zeroXYZ();
		
		CHECK_EQUAL(p3.getX(), 0.55);
		CHECK_EQUAL(p3.getY(), 1.10);
		CHECK_EQUAL(p3.getZ(), 1.65);
		
		p3.resetTransformation();
		p3.setTranslation(1.0, 1.0, 1.0);
		p3.setX(100.0);
		
		CHECK_EQUAL(p3.getX(), 101.0);
		CHECK_EQUAL(p3.getY(),   1.0);
		CHECK_EQUAL(p3.getZ(),   1.0);
		
		p3.setY(200.0);
		CHECK_EQUAL(p3.getX(), 101.0);
		CHECK_EQUAL(p3.getY(), 201.0);
		CHECK_EQUAL(p3.getZ(),   1.0);
		
		p3.setZ(300.0);
		CHECK_EQUAL(p3.getX(), 101.0);
		CHECK_EQUAL(p3.getY(), 201.0);
		CHECK_EQUAL(p3.getZ(), 301.0);
		
		CncDoublePosition p1;
		p1.set(p3);
		CHECK_EQUAL(p1.getX(), 101.0);
		CHECK_EQUAL(p1.getY(), 201.0);
		CHECK_EQUAL(p1.getZ(), 301.0);
	}
	/////////////////////////////////////////////////////////////////////
	TEST(Translate_1)
	{
		CncDoublePosition p3;
		p3.setTranslation(1.0, 2.0, 3.0);
		p3.setXYZ(10.0, 20.0, 30.0);

		CHECK_EQUAL(p3.getX(), 11.0);
		CHECK_EQUAL(p3.getY(), 22.0);
		CHECK_EQUAL(p3.getZ(), 33.0);
		
		p3.resetTransformation();
		
		CHECK_EQUAL(p3.getX(), 10.0);
		CHECK_EQUAL(p3.getY(), 20.0);
		CHECK_EQUAL(p3.getZ(), 30.0);
	}
	/////////////////////////////////////////////////////////////////////
	TEST(Scale_1)
	{
		CncDoublePosition p3;
		p3.setScaling(0.5);
		p3.setTranslation(1.0, 2.0, 3.0);
		p3.setXYZ(9.0, 18.0, 27.0);

		CHECK_EQUAL(p3.getX(),  5.0);
		CHECK_EQUAL(p3.getY(), 10.0);
		CHECK_EQUAL(p3.getZ(), 15.0);
		
		p3.resetTransformation();
		
		CHECK_EQUAL(p3.getX(),  9.0);
		CHECK_EQUAL(p3.getY(), 18.0);
		CHECK_EQUAL(p3.getZ(), 27.0);

		p3.resetTransformation();
		p3.setTranslation(1.0, 2.0, 3.0);
		p3.setScaling(0.5);
		p3.setXYZ(10.0, 20.0, 30.0);

		CHECK_EQUAL(p3.getX(),  6.0);
		CHECK_EQUAL(p3.getY(), 12.0);
		CHECK_EQUAL(p3.getZ(), 18.0);
	}
	/////////////////////////////////////////////////////////////////////
	TEST(IncDec_1)
	{
		CncDoublePosition p3;
		p3.setTranslation(1.0, 1.0, 1.0);
		p3.setXYZ(10.0, 20.0, 30.0);
		
		CHECK_EQUAL(p3.getX(), 11.0);
		CHECK_EQUAL(p3.getY(), 21.0);
		CHECK_EQUAL(p3.getZ(), 31.0);
		
		p3.inc(1,1,1);
		
		CHECK_EQUAL(p3.getX(), 12.0);
		CHECK_EQUAL(p3.getY(), 22.0);
		CHECK_EQUAL(p3.getZ(), 32.0);
		
		p3.dec(1,1,1);
		
		CHECK_EQUAL(p3.getX(), 11.0);
		CHECK_EQUAL(p3.getY(), 21.0);
		CHECK_EQUAL(p3.getZ(), 31.0);
		
		p3.resetTransformation();
		p3.setTranslation(1.0, 1.0, 1.0);
		p3.setXYZ(10.0, 20.0, 30.0);
		
		p3++;
		
		CHECK_EQUAL(p3.getX(), 12.0);
		CHECK_EQUAL(p3.getY(), 22.0);
		CHECK_EQUAL(p3.getZ(), 32.0);
		
		p3--;
		
		CHECK_EQUAL(p3.getX(), 11.0);
		CHECK_EQUAL(p3.getY(), 21.0);
		CHECK_EQUAL(p3.getZ(), 31.0);

		p3.resetTransformation();
		p3.setTranslation(1.0, 1.0, 1.0);
		p3.setXYZ(10.0, 20.0, 30.0);
		
		++p3;
		
		CHECK_EQUAL(p3.getX(), 12.0);
		CHECK_EQUAL(p3.getY(), 22.0);
		CHECK_EQUAL(p3.getZ(), 32.0);
		
		--p3;
		
		CHECK_EQUAL(p3.getX(), 11.0);
		CHECK_EQUAL(p3.getY(), 21.0);
		CHECK_EQUAL(p3.getZ(), 31.0);
	}
	
	/////////////////////////////////////////////////////////////////////
	template<class T>
	void test_plus_1()
	{
		const CncPosition<T> p1(4.0, 5.0, 6.0);
		const CncPosition<T> p2(4.0, 5.0, 6.0);
		const CncPosition<T> p3 = p1 + p2;
		
		CHECK_EQUAL(p3.getX(),  8.0);
		CHECK_EQUAL(p3.getY(), 10.0);
		CHECK_EQUAL(p3.getZ(), 12.0);
	}

	TEST(Plus_1) 
	{
		test_plus_1<int16_t>();
		test_plus_1<int32_t>();
		test_plus_1<float>();
		test_plus_1<double>();
	}
	
	/////////////////////////////////////////////////////////////////////
	template<class T>
	void test_minus_1()
	{
		const CncPosition<T> p1(14.0, 11.0, 16.0);
		const CncPosition<T> p2( 5.0,  5.0,  6.0);
		const CncPosition<T> p3 = p1 - p2;
		
		CHECK_EQUAL(p3.getX(),  9.0);
		CHECK_EQUAL(p3.getY(),  6.0);
		CHECK_EQUAL(p3.getZ(), 10.0);
	}
	TEST(MINUS_1) 
	{
		test_minus_1<int16_t>();
		test_minus_1<int32_t>();
		test_minus_1<float>();
		test_minus_1<double>();
	}
	
	/////////////////////////////////////////////////////////////////////
	template<class T>
	void test_compare_1()
	{
		CncPosition<T> p1(14, 11, 16);
		CncPosition<T> p2( 5,  5,  6);
		CncPosition<T> p3(14, 11, 16);
		
		CHECK_EQUAL(p1.isEqual(p1), true);
		CHECK_EQUAL(p1.isEqual(p2), false);
		CHECK_EQUAL(p1.isEqual(p3), true);
		
		p2.set(p1);
		CHECK_EQUAL(p2.isEqual(p1), true);
		CHECK_EQUAL(p1.isEqual(p2), true);
	}
	TEST(CMP_1) 
	{
		test_compare_1<int16_t>();
		test_compare_1<int32_t>();
		test_compare_1<float>();
		test_compare_1<double>();
		
		CncDoublePosition p1(1.00001, 1.0, 1.0);
		CncDoublePosition p2(1.0, 1.0, 1.0);
		CHECK_EQUAL(p1.isEqual(p1), true);

		CHECK_EQUAL(p1.isEqual(p2), false);
		CHECK_EQUAL(p1.isEqual(p2, CncDoublePosition::CMP_BASE, 0.001), true);
	}
}