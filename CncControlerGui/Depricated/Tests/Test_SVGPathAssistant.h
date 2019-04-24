#ifndef TEST_SVG_PATH_ASSSISTANT_H
#define TEST_SVG_PATH_ASSSISTANT_H

#include "Test_Base.h"
#include "SVGPathAssistant.h"

////////////////////////////////////////////////////////////////////////////
class TEST_SVG_PATH_ASSISTANT : public TEST_BASE {
	
	public:
		////////////////////////////////////////////////////////////////////
		TEST_SVG_PATH_ASSISTANT(UnitTests* frame)
		: TEST_BASE(frame)
		{}
		
		////////////////////////////////////////////////////////////////////
		virtual ~TEST_SVG_PATH_ASSISTANT() {}
		
		////////////////////////////////////////////////////////////////////
		virtual const char* name() { return "TEST_SVG_PATH_ASSISTANT"; }
		
	protected:
		////////////////////////////////////////////////////////////////////
		virtual bool runTest() {
			bool ret = true;
			SVGPathAssistant pa;
			
			HLINE();
			std::clog << "Init closed path (polygon)"<< std::endl;
			ret = pa.processSvgNode("<path d=\"M 136.196 0.000 L 68.098 117.949 -68.098 117.949 -136.196 0.000 -68.098 -117.949 68.098 -117.949 136.196 0.000 125.123 0.000 62.562 108.360 -62.562 108.360 -125.123 0.000 -62.562 -108.360 62.562 -108.360 125.123 0.000 114.050 0.000 57.025 98.770 -57.025 98.770 -114.050 0.000 -57.025 -98.770 57.025 -98.770 114.050 0.000 102.977 0.000 51.489 89.181 -51.489 89.181 -102.977 0.000 -51.489 -89.181 51.489 -89.181 102.977 0.000 91.905 0.000 45.952 79.592 -45.952 79.592 -91.905 0.000 -45.952 -79.592 45.952 -79.592 91.905 0.000 80.832 0.000 40.416 70.002 -40.416 70.002 -80.832 0.000 -40.416 -70.002 40.416 -70.002 80.832 0.000 69.759 0.000 34.879 60.413 -34.879 60.413 -69.759 0.000 -34.879 -60.413 34.879 -60.413 69.759 0.000 58.686 0.000 29.343 50.824 -29.343 50.824 -58.686 0.000 -29.343 -50.824 29.343 -50.824 58.686 0.000 47.613 0.000 23.807 41.234 -23.807 41.234 -47.613 0.000 -23.807 -41.234 23.807 -41.234 47.613 0.000 36.540 0.000 18.270 31.645 -18.270 31.645 -36.540 0.000 -18.270 -31.645 18.270 -31.645 36.540 0.000 25.468 0.000 12.734 22.055 -12.734 22.055 -25.468 0.000 -12.734 -22.055 12.734 -22.055 25.468 0.000 14.395 0.000 7.197 12.466 -7.197 12.466 -14.395 0.000 -7.197 -12.466 7.197 -12.466 14.395 0.000 z \"/>");
			CHECK_RESULT(ret, "processSvgNode failed!", __FILE__, __LINE__);
			trace(pa);
			
			HLINE();
			std::clog << "Init open path (linestring)" << std::endl;
			ret = pa.processSvgNode("<path d=\"M 136.196 0.000 L 68.098 117.949 -68.098 117.949\" transform=\"translate(20,20)\"/>");
			CHECK_RESULT(ret, "processSvgNode failed!", __FILE__, __LINE__);
			trace(pa);
			
			HLINE();
			std::clog << "Init point" << std::endl;
			ret = pa.processSvgNode("<path d=\"M 136.196 0.000\"/>");
			CHECK_RESULT(ret, "processSvgNode failed!", __FILE__, __LINE__);
			trace(pa);
			
			HLINE();
			std::clog << "Init open path (linestring)" << std::endl;
			ret = pa.processSvgNode("<path d=\"M 136.196 0.000 L 68.098 117.949 -68.098 117.949\" transform=\"translate(20,20)\"/>");
			CHECK_RESULT(ret, "processSvgNode failed!", __FILE__, __LINE__);
			trace(pa);
			
			PARAGRAPH();
			std::clog << "Reverse last path" << std::endl;
			ret = pa.reversePath();
			CHECK_RESULT(ret, "reversePath failed!", __FILE__, __LINE__);
			trace(pa);
			
			PARAGRAPH();
			std::clog << "Center last path" << std::endl;
			ret = pa.centerPath();
			CHECK_RESULT(ret, "centerPath failed!", __FILE__, __LINE__);
			trace(pa);
			
			PARAGRAPH();
			std::clog << "Set a path list" << std::endl;
			CncPathListManager pli;
			for (unsigned int i=0; i<10; i++)
				pli.calculateAndAddEntry({i*10.0, i*10.0});
				
			pa.setPathList(pli);
			trace(pa);
			
			PARAGRAPH();
			std::clog << "trace the last path" << std::endl;
			pa.tracePathList(std::clog);

			return true;
		}
		
		////////////////////////////////////////////////////////////////////
		void trace(SVGPathAssistant& pa) {
			std::clog << "Point#    : " << pa.getDataPointCount() << std::endl;
			std::clog << "Length    : " << pa.getCurrentPathLength() << std::endl;
			std::clog << "Closed    : " << pa.isPathClosed() << std::endl;
			std::clog << "WKT       : " << pa.getAsWktRepresentation() << std::endl;
			std::clog << "SVG path  : " << pa.getAsSvgPathRepresentation() << std::endl;
		}
};

#endif