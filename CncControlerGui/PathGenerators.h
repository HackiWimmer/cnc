#ifndef PATH_GENERATORS_H
#define PATH_GENERATORS_H

#include "CncCommon.h"

#include "PathGeneratorBase.h"
#include "SvgUnitCalculator.h"


///////////////////////////////////////////////////////////////////////////
class xyz : public PathGenertorBase {
	
	public:
		///////////////////////////////////////////////////////////////////
		xyz() : PathGenertorBase() {}
		virtual ~xyz() {}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name = "xyz";
			
			PathGenertorBase::ParameterInfo pi;
			pi.setup("Radius [mm]", 70.0, 1.0, 400.0, 3);
			setupParameter(0, pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& generate() {
			static wxString s;
			//todo
			s = "xyz";
			return s;
		}
};

///////////////////////////////////////////////////////////////////////////
class abc : public PathGenertorBase {
	
	public:
		///////////////////////////////////////////////////////////////////
		abc() : PathGenertorBase() {}
		virtual ~abc() {}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters() {
			name = "abc";
			
			PathGenertorBase::ParameterInfo pi;
			pi.setup("Length [mm]", 42.0, 1.0, 400.0, 3);
			setupParameter(0, pi);
			
			pi.setup("sssssss [mm]", 40.0, 1.0, 400.0, 3);
			setupParameter(1, pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& generate() {
			static wxString s;
			//todo
			s = "abc";
			return s;
		}
};

///////////////////////////////////////////////////////////////////////////
class pgRoundPoketWhole : public PathGenertorBase {
	
	public:
		///////////////////////////////////////////////////////////////////
		pgRoundPoketWhole() : PathGenertorBase() {}
		virtual ~pgRoundPoketWhole() {}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters() {
			name = "Poket Whole - Elliptical";
			
			PathGenertorBase::ParameterInfo pi;
			pi.setup("Radius X [mm]", 40.0, 1.0, 400.0, 3);
			setupParameter(0, pi);
			
			pi.setup("Radius Y [mm]", 40.0, 1.0, 400.0, 3);
			setupParameter(1, pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& generate() {
			static wxString s;

			SvgPathGroup spg(mm);
			spg.pGen().setColour(commonValues.pathColour);
			spg.pGen().addPocketWhole(commonValues.toolDiameter, getParameterValue(0), getParameterValue(1));
			spg.add(spg.pGen().get());
			transform(spg);
			
			s = spg.get();
			return s;
		}
};

#endif
