#ifndef TEST_SVG_NODE_PARSER_H
#define TEST_SVG_NODE_PARSER_H

#include "Test_Base.h"
#include "SVGNodeParser.h"
#include "PathHandlerBase.h"

////////////////////////////////////////////////////////////////////////////
class TEST_SVG_NODE_PARSER : public TEST_BASE {
	
	public:
		////////////////////////////////////////////////////////////////////
		TEST_SVG_NODE_PARSER(UnitTests* frame)
		: TEST_BASE(frame)
		{}
		
		////////////////////////////////////////////////////////////////////
		virtual ~TEST_SVG_NODE_PARSER() {}
		
		////////////////////////////////////////////////////////////////////
		virtual const char* name() { return "TEST_SVG_NODE_PARSER"; }
		
	protected:
		
		////////////////////////////////////////////////////////////////////
		virtual bool runTest() {
			
			// will be deleted by the descructor of SVGNodeParser
			PathHandlerBase* phb = new PathHandlerBase();
			phb->setCurveLibResolution(0.05);
			phb->prepareWork();
			
			SVGNodeParser sp(phb);

			process(sp, *phb, "<path d=\"M 136.196 0.000 L 68.098 117.949 -68.098 117.949 -136.196 0.000 -68.098 -117.949 68.098 -117.949 136.196 0.000 125.123 0.000 62.562 108.360 -62.562 108.360 -125.123 0.000 -62.562 -108.360 62.562 -108.360 125.123 0.000 114.050 0.000 57.025 98.770 -57.025 98.770 -114.050 0.000 -57.025 -98.770 57.025 -98.770 114.050 0.000 102.977 0.000 51.489 89.181 -51.489 89.181 -102.977 0.000 -51.489 -89.181 51.489 -89.181 102.977 0.000 91.905 0.000 45.952 79.592 -45.952 79.592 -91.905 0.000 -45.952 -79.592 45.952 -79.592 91.905 0.000 80.832 0.000 40.416 70.002 -40.416 70.002 -80.832 0.000 -40.416 -70.002 40.416 -70.002 80.832 0.000 69.759 0.000 34.879 60.413 -34.879 60.413 -69.759 0.000 -34.879 -60.413 34.879 -60.413 69.759 0.000 58.686 0.000 29.343 50.824 -29.343 50.824 -58.686 0.000 -29.343 -50.824 29.343 -50.824 58.686 0.000 47.613 0.000 23.807 41.234 -23.807 41.234 -47.613 0.000 -23.807 -41.234 23.807 -41.234 47.613 0.000 36.540 0.000 18.270 31.645 -18.270 31.645 -36.540 0.000 -18.270 -31.645 18.270 -31.645 36.540 0.000 25.468 0.000 12.734 22.055 -12.734 22.055 -25.468 0.000 -12.734 -22.055 12.734 -22.055 25.468 0.000 14.395 0.000 7.197 12.466 -7.197 12.466 -14.395 0.000 -7.197 -12.466 7.197 -12.466 14.395 0.000 z \"/>");
			process(sp, *phb, "<path d=\"M 136.196 0.000 L 68.098 117.949 -68.098 117.949\" transform=\"translate(20,20)\"/>");
			
			CncPathListManager pli = phb->getPathList();
			
			process(sp, *phb, "<path d=\"M 136.196 0.000\"/>");
			//process(sp, phb, "<path d=\"\"/>");
			
			phb->setPathList(pli);
			process(sp, *phb);
			
			phb->finishWork();
			
			return true;
		}
		
		////////////////////////////////////////////////////////////////////
		void process(SVGNodeParser& sp, PathHandlerBase& phb, const char* n) {
			sp.processSvgNode(n);
			process(sp, phb);
		}
		
		////////////////////////////////////////////////////////////////////
		void process(SVGNodeParser& sp, PathHandlerBase& phb) {
			phb.reversePath();
			phb.centerPath();
			
			phb.tracePathList(std::clog);
			
			std::clog << "Point#    : " << phb.getDataPointCount() << std::endl;
			std::clog << "Length    : " << phb.getCurrentPathLength() << std::endl;
			std::clog << "Closed    : " << phb.isPathClosed() << std::endl;
			std::clog << "WKT       : " << phb.getAsWktRepresentation() << std::endl;
			std::clog << "SVG path  : " << phb.getAsSvgPathRepresentation("stroke=\"blue\"") << std::endl;
			
			wxRealPoint cp;
			bool ret = phb.getCentroid(cp);
			std::clog << "Centroid  : " << ret << " | "<< cp << std::endl;
			
			std::clog << "Total Len : " << phb.getTotalLength() << std::endl;
			std::clog << "*******************************************************************" << std::endl;
		}
};

#endif


