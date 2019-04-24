#ifndef TEST_CNC_CLIPPER_WRAPPER_H
#define TEST_CNC_CLIPPER_WRAPPER_H

#include "Test_Base.h"
#include "PathHandlerBase.h"
#include "CncClipperWrapper.h"

////////////////////////////////////////////////////////////////////////////
class TEST_CNC_CLIPPER_WRAPPER : public TEST_BASE {
	
	public:
		////////////////////////////////////////////////////////////////////
		TEST_CNC_CLIPPER_WRAPPER(UnitTests* frame)
		: TEST_BASE(frame)
		{}
		
		////////////////////////////////////////////////////////////////////
		virtual ~TEST_CNC_CLIPPER_WRAPPER() {}
		
		////////////////////////////////////////////////////////////////////
		virtual const char* name() { return "TEST_CNC_CLIPPER_WRAPPER"; }
		
	protected:
		////////////////////////////////////////////////////////////////////
		virtual bool runTest() {
			
			/*
			CncPathListManager plm;
			plm.calculateAndAddEntry(348,257);
			plm.calculateAndAddEntry(364,148);
			plm.calculateAndAddEntry(362,148);
			plm.calculateAndAddEntry(326,241);
			plm.calculateAndAddEntry(295,219);
			plm.calculateAndAddEntry(258,88);
			plm.calculateAndAddEntry(440,129);
			plm.calculateAndAddEntry(370,196);
			plm.calculateAndAddEntry(372,275);
			
			// trace initial behaviour
			clog << plm << endl;
			
			CncClipperWrapper cw;
			cw.offsetPath(plm, +5.0 * 1000);
			*/
			
			CncPolygonPoints cpp;
			std::clog << cpp << std::endl;
			
			cpp.append(348,257);
			cpp.append(364,148);
			cpp.append(362,148);
			cpp.append(326,241);
			cpp.append(295,219);
			cpp.append(258,88);
			cpp.append(440,129);
			cpp.append(370,196);
			cpp.append(372,275);
			
			//clog << cpp << endl;
			
			CncClipperWrapper cw;
			CncPolygons results;
			cw.offsetPath(cpp, results, -15.5);
			
			CncPolygonPoints ret;
			results.getPolygonPoints(0, ret);
			
			//clog << ret << endl;
			
			//cout << cpp.getAsSvgPathRepresentation("style=\"fill:none;stroke:#ff0000;stroke-width:1.05952847\"") << endl;
			//cout << ret.getAsSvgPathRepresentation("style=\"fill:none;stroke:#0000ff;stroke-width:1.05952847\"") << endl;
			
			return true;
		}
};

#endif