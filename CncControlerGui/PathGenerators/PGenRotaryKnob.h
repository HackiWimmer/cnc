#ifndef PGEN_ROTARY_KNOB_H
#define PGEN_ROTARY_KNOB_H

#include "PGenGenericPath.h"

///////////////////////////////////////////////////////////////////////////
class PGenRotaryKnob : public PGenGenericPath {
	
	private:
		unsigned int IDX_KNOB_FORM;
		unsigned int IDX_SECTIONS;
		unsigned int IDX_DEPTH;
		unsigned int IDX_KNOB_DIAMETER;
		unsigned int IDX_SHAFT_DIAMETER;
		unsigned int IDX_SHAFT_TYPE;
		unsigned int IDX_INLAY;
		unsigned int IDX_REFPOINT;
		
	protected:
		
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			
			// original envelope
			SVGElementConverter::convertPolylineToPathData(getOriginalPolygon(), px, path);
			addHelpContructPath(spg, path);
			
			// help circle
			double radius = getParameterNumValue(IDX_KNOB_DIAMETER) / 2.0;
			SVGElementConverter::convertCircleToPathData(SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().x),
			                                             SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().y), 
			                                             SvgUnitCalculator::convertMM2ReferenceUnit(radius),
			                                             path);
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Rotray Knob";
			treePath 	= "Advanced Polygons [mm]";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupEnum("Knob Form", "concave;convex", 0);
			pi.help = "Determines the knob type.";
			IDX_KNOB_FORM = setupParameter(pi); 

			pi.setupNum("Sections", 12, 1, 100, 0);
			pi.help = "Count of data points to connect on diameter. Only values even values > 3 are valid";
			IDX_SECTIONS = setupParameter(pi); 
			
			pi.setupNum("Knob Diameter [mm]", 60.0, 1.0, 120.0, 3);
			pi.help = "Diameter of the shaft whole. A value of 0.0 means no shaft and negative values . . .";
			IDX_KNOB_DIAMETER = setupParameter(pi); 
			
			pi.setupNum("Depth", 2, 0, 10, 1);
			pi.help = "Depth of the grid troughes";
			IDX_DEPTH = setupParameter(pi); 
			
			pi.setupEnum("Shaft Type", "Pre-drilled (tool diameter);Whole (shaft diameter);No shaft", 0);
			pi.help = "Determines shaft type.";
			IDX_SHAFT_TYPE = setupParameter(pi); 
			
			pi.setupNum("Shaft Diameter [mm]", 10.0, -1, 40.0, 3);
			pi.help = "Diameter of rotary knob.";
			IDX_SHAFT_DIAMETER = setupParameter(pi); 
			
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupCCReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initPreDefinedParameterSetups() {
			PreDefParameterMap map;
			map[IDX_KNOB_FORM] 			= wxVariant(0);
			map[IDX_SECTIONS] 			= wxVariant(6);
			map[IDX_DEPTH]				= wxVariant(1.5);
			map[IDX_KNOB_DIAMETER] 		= wxVariant(50);
			map[IDX_SHAFT_DIAMETER] 	= wxVariant(10);
			map[IDX_SHAFT_TYPE] 		= wxVariant(0);
			map[IDX_INLAY] 				= wxVariant(0);
			map[IDX_REFPOINT] 			= wxVariant(0);
			
			addPreDefinedParameterSetup("Knob Pattern A", map);
			
			map.clear();
			map[IDX_KNOB_FORM] 			= wxVariant(0);
			map[IDX_SECTIONS] 			= wxVariant(10);
			map[IDX_DEPTH]				= wxVariant(0.9);
			map[IDX_KNOB_DIAMETER] 		= wxVariant(50);
			map[IDX_SHAFT_DIAMETER] 	= wxVariant(10);
			map[IDX_SHAFT_TYPE] 		= wxVariant(1);
			map[IDX_INLAY] 				= wxVariant(0);
			map[IDX_REFPOINT] 			= wxVariant(0);
			
			addPreDefinedParameterSetup("Knob Pattern B", map);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual double getXOffset() { return SvgUnitCalculator::convertReferenceUnit2MM(abs(getPolygonData().getMinRealPos().x)) + 5.0; }
		virtual double getYOffset() { return SvgUnitCalculator::convertReferenceUnit2MM(abs(getPolygonData().getMinRealPos().y)) + 5.0; }
		virtual bool getInlayMode() { return (getParameterEnumValue(IDX_INLAY) == 1); }
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& getElementAsSvgPath(unsigned int elementIndex) {
			static wxString path;
			path.assign("<path d=\"");
			 
			// collect parameters
			unsigned int sections 	= getParameterNumValue(IDX_SECTIONS);
			double steps  			= 360.0 / sections;
			double radius			= getParameterNumValue(IDX_KNOB_DIAMETER) / 2.0;
			
			int form = getParameterEnumValue(IDX_KNOB_FORM);
			
			// store initial move
			path.append(wxString::Format("M%lf,0 ", SvgUnitCalculator::convertMM2ReferenceUnit(-radius)));
			
			double pos = 0.0;
			for ( unsigned int i=0; i<=sections; i++ ) {
				double x = cos(pos*PI/180) * radius;
				double y = sin(pos*PI/180) * radius;
				
				x = round(x * 1000.0) / 1000.0;
				y = round(y * 1000.0) / 1000.0;
				
				if ( i%2==0 ) {
					path.append(wxString::Format("A %lf %lf 0 0 1 %lf,%lf ", SvgUnitCalculator::convertMM2ReferenceUnit(radius * 2),
																			 SvgUnitCalculator::convertMM2ReferenceUnit(radius * 2),
																			 SvgUnitCalculator::convertMM2ReferenceUnit(-x),
																			 SvgUnitCalculator::convertMM2ReferenceUnit(-y)));
				} else {
					path.append(wxString::Format("A %lf %lf 0 0 %d %lf,%lf ", SvgUnitCalculator::convertMM2ReferenceUnit(radius * getParameterNumValue(IDX_DEPTH)),
																			 SvgUnitCalculator::convertMM2ReferenceUnit(radius * getParameterNumValue(IDX_DEPTH)),
																			 form,
																			 SvgUnitCalculator::convertMM2ReferenceUnit(-x),
																			 SvgUnitCalculator::convertMM2ReferenceUnit(-y)));
																			  
				}
				pos += steps;
			}
			
			path.Append("\"/>");
			return path;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenRotaryKnob() 
		: PGenGenericPath()
		, IDX_KNOB_FORM(INT_MAX)
		, IDX_SECTIONS(INT_MAX)
		, IDX_DEPTH(INT_MAX)
		, IDX_KNOB_DIAMETER(INT_MAX)
		, IDX_SHAFT_DIAMETER(INT_MAX)
		, IDX_SHAFT_TYPE(INT_MAX)
		, IDX_INLAY(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{
			setInputUnit(px);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenRotaryKnob() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool parameterChanging(PathGeneratorBase::ChangeCategory cc, int paraIdxInCat, const wxVariant& value) { 
			if ( cc == PathGeneratorBase::ChangeCategory::CC_TPL_CAT && paraIdxInCat == (int)IDX_SECTIONS ) {
				
				if ( value.GetInteger() < 4 )
					return false;
					
				if ( value.GetInteger()%2 != 0 )
					return false;
			}
			
			return true; 
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			bool ret = PGenSvgElementPolygon::generate(spg, toolDiameter);
			
			// additional genarete the shaft
			if ( getParameterEnumValue(IDX_SHAFT_TYPE) != 2 ) {
				wxString shaft("M0,0 ");
				if ( getParameterEnumValue(IDX_SHAFT_TYPE) == 0 || toolDiameter > getParameterNumValue(IDX_SHAFT_DIAMETER) ) {
					shaft.Append("L0,0 ");
					
				} else {
					wxString whole;
					SVGElementConverter::convertCircleToPathData(SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().x),
																 SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().y), 
																 SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_SHAFT_DIAMETER) / 2.0 - toolDiameter / 2.0),
																 whole);
					shaft.Append(whole);
				}
				
				spg.pGen().add(shaft);
				spg.add(spg.pGen().get());
			}
			
			return ret;
		}
};

#endif