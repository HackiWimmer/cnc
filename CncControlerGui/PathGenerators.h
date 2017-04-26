#ifndef PATH_GENERATORS_H
#define PATH_GENERATORS_H

#include "CncCommon.h"

#include "PathGeneratorBase.h"
#include "SvgUnitCalculator.h"

///////////////////////////////////////////////////////////////////////////
class PGenSample : public PathGeneratorBase {
	
	public:
		///////////////////////////////////////////////////////////////////
		PGenSample() : PathGeneratorBase() {}
		virtual ~PGenSample() {}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name = "PGenSample";
			
			PathGeneratorBase::ParameterInfo pi;
			//...
			setupCCReferencePoint(pi); 	// id = ?
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			
			// ...
			
			setTranslateX(0.0);
			setTranslateY(0.0);
			
			return spg.isOK();
		}
};

///////////////////////////////////////////////////////////////////////////
class PGenTest : public PathGeneratorBase {
	
	public:
		///////////////////////////////////////////////////////////////////
		PGenTest() : PathGeneratorBase() {}
		virtual ~PGenTest() {}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name = "PGenTest";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Radius [mm]", 70.0, 1.0, 400.0, 3);
			setupParameter(pi);
			
			pi.setupBool("bool", true);
			setupParameter(pi);
			
			wxArrayString items;
			items.Add("first");
			items.Add("second");
			pi.setupEnum("enum", items, 1);
			setupParameter(pi);
			
			setupCCReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			spg.pGen().add("PGenTest");
			spg.add(spg.pGen().get());

			setTranslateX(0.0);
			setTranslateY(0.0);
			
			return spg.isOK();
		}
};

///////////////////////////////////////////////////////////////////////////
class PGenPoint : public PathGeneratorBase {
	
	public:
		///////////////////////////////////////////////////////////////////
		PGenPoint() : PathGeneratorBase() {}
		virtual ~PGenPoint() {}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name = "Point";
			
			commonValues.canToolCorrection 	= false;
			commonValues.canToolDiameter	= false;
			
			cncParameterValues.canCorrect	= false;
			cncParameterValues.canReverse	= false;
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Point X [mm]", 20.0, 1.0, 400.0, 3);
			pi.help = "Determine X coordinate.";
			setupParameter(pi); 		// id = 0

			pi.setupNum("Point Y [mm]", 20.0, 1.0, 400.0, 3);
			pi.help = "Determine Y coordinate.";
			setupParameter(pi); 		// id = 1
			
			setupCCReferencePoint(pi); 	// id = 2
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			spg.add(spg.fGen().addPoint(getParameterNumValue(0), getParameterNumValue(1)));
			
			setTranslateX(getParameterNumValue(0) + 5.0);
			setTranslateY(getParameterNumValue(1) + 5.0);
			
			return spg.isOK();
		}
};

///////////////////////////////////////////////////////////////////////////
class PGenLine : public PathGeneratorBase {
	
	public:
		///////////////////////////////////////////////////////////////////
		PGenLine() : PathGeneratorBase() {}
		virtual ~PGenLine() {}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name = "Line";
			
			cncParameterValues.canCorrect	= false;
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("X1 [mm]", 0.0, 1.0, 400.0, 3);
			pi.help = "Determine X1 coordinate.";
			setupParameter(pi); // id = 0

			pi.setupNum("Y1 [mm]", 0.0, 1.0, 400.0, 3);
			pi.help = "Determine Y1 coordinate.";
			setupParameter(pi); // id = 1
			
			pi.setupNum("X2 [mm]", 40.0, 1.0, 400.0, 3);
			pi.help = "Determine X2 coordinate.";
			setupParameter(pi); // id = 2

			pi.setupNum("Y2 [mm]", 40.0, 1.0, 400.0, 3);
			pi.help = "Determine Y2 coordinate.";
			setupParameter(pi); // id = 3
			
			//todo add ref point
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			spg.add(spg.fGen().addLine(toolDiameter, getParameterNumValue(0), getParameterNumValue(1), 
			                                         getParameterNumValue(2), getParameterNumValue(3),
													 commonValues.toolCorrection));
			
			setTranslateX(getParameterNumValue(0) + 5.0);
			setTranslateY(getParameterNumValue(1) + 5.0);
			
			return spg.isOK();
		}
};

///////////////////////////////////////////////////////////////////////////
class PGenLongWhole : public PathGeneratorBase {
	
	public:
		///////////////////////////////////////////////////////////////////
		PGenLongWhole() : PathGeneratorBase() {}
		virtual ~PGenLongWhole() {}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters() {
			name = "Long Whole";
			
			cncParameterValues.canCorrect	= false;
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Width [mm]", 40.0, 1.0, 400.0, 3);
			pi.help = "Determine long whole width (x axis).";
			setupParameter(pi); // id = 0
			
			pi.setupNum("Height [mm]", 10.0, 1.0, 400.0, 3);
			pi.help = "Determine long whole height (y axis).";
			setupParameter(pi); // id = 1
			
			pi.setupEnum("Corners Style", "round;square", 0);
			pi.help = "Determine corner style.";
			setupParameter(pi); // id = 2
			
			setupReferencePoint(pi, "center/center;left/center;left/top;left/bottom", 1); // id = 3
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			switch ( getParameterEnumValue(3) ) {
				// center/center
				case 0: 	determineReferencePoint(spg, -getParameterNumValue(0)/2, -getParameterNumValue(1)/2);
							break;
				// left/center
				case 1: 	determineReferencePoint(spg, 0.0, -getParameterNumValue(1)/2);
							break;
				// left/top
				case 2:		determineReferencePoint(spg, 0.0, 0.0);
							break;
				// left bootom
				case 3:		determineReferencePoint(spg, 0.0, -getParameterNumValue(1));
							break;
							
				default:	addErrorInfo(wxString::Format("setReferencePoint: invalid enum: %d", getParameterEnumValue(3)));
							return false;
			}
			
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			
			if ( getParameterEnumValue(2) == 0 )
				spg.add(spg.fGen().addRoundRect(toolDiameter, getParameterNumValue(0), getParameterNumValue(1), commonValues.toolCorrection));
			else 
				spg.add(spg.fGen().addSquareRect(toolDiameter, getParameterNumValue(0), getParameterNumValue(1), commonValues.toolCorrection));

			setTranslateX(getParameterNumValue(0) + 5.0);
			setTranslateY(getParameterNumValue(1) + 5.0);

			return spg.isOK();
		}
};

///////////////////////////////////////////////////////////////////////////
class PGenRoundPoketWhole : public PathGeneratorBase {
	
	public:
		///////////////////////////////////////////////////////////////////
		PGenRoundPoketWhole() : PathGeneratorBase() {}
		virtual ~PGenRoundPoketWhole() {}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters() {
			name = "Poket Whole - Elliptical";
			
			cncParameterValues.canCorrect	= false;
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Radius X [mm]", 40.0, 1.0, 400.0, 3);
			pi.help = "Determine the x radius parameter of the elliptical arg.";
			setupParameter(pi); 		// id = 0
			
			pi.setupNum("Radius Y [mm]", 40.0, 1.0, 400.0, 3);
			pi.help = "Determine the y radius parameter of the elliptical arg.";
			setupParameter(pi); 		// id = 1
			
			setupCCReferencePoint(pi);	// id = 2
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			
			spg.pGen().addPocketWhole(toolDiameter, getParameterNumValue(0), getParameterNumValue(1), commonValues.toolCorrection);
			spg.add(spg.pGen().get());
			
			setTranslateX(getParameterNumValue(0) + 5.0);
			setTranslateY(getParameterNumValue(1) + 5.0);

			return spg.isOK();
		}
};

#endif
