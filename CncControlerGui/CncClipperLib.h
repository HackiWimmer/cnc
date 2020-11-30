#ifndef CNC_CLIPPER_LIB
#define CNC_CLIPPER_LIB

#include <iostream>
#include <wx/string.h>
#include "CncPosition.h"
#include "Clipper/clipper.hpp"

namespace ClipperLib {
	
	const int floatFactor = 10 * 1000;
	
	double				convert(double v);

	double				transform(const cInt& v);
	cInt				transform(double v);
	
	IntPoint			transform(CncDoublePosition p);
	IntPoint			transform(double pX, double pY, double pZ);
	IntPoint			transform(CncLongPosition p);
	
	CncLongPosition		asCncLongPosition(IntPoint p);
	CncDoublePosition	asCncDoublePosition(IntPoint p);
	
	bool				isPathClosed(const Path& p);
	bool				closePath(Path& p);
	bool				hasZAxisMovement(const Path& path);
	bool				isHelix(const Path& path, double diameter);
	
	cInt getPathLength(const Path& path);
	
	std::ostream& operator<<(std::ostream& os, const IntPoint& p);
	
};

#endif
