#include <iostream>
#include "CncCommon.h"
#include "CncPosition.h"

///////////////////////////////////////////////////////////////////////
void test() { 
	CncLongPosition 	p1;
	CncDoublePosition 	p2;
}

///////////////////////////////////////////////////////////////////////
bool operator== (const CncPosition<double> &a, const CncPosition<double> &b) {
///////////////////////////////////////////////////////////////////////
	const bool xc = cnc::dblCompare(a.getX(), b.getX());
	const bool yc = cnc::dblCompare(a.getY(), b.getY());
	const bool zc = cnc::dblCompare(a.getZ(), b.getZ());
	
	return ( xc && yc && zc ); 
}
