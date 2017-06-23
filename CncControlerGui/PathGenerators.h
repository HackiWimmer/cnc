#ifndef PATH_GENERATORS_H
#define PATH_GENERATORS_H

#include <list>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include "CncCommon.h"
#include "PathGeneratorBase.h"
#include "LinearFunc.h"
#include "SvgUnitCalculator.h"
#include "SVGPathAssistant.h"
#include "SVGElementConverter.h"

///////////////////////////////////////////////////////////////////////////
class PGenSample : public PathGeneratorBase {
	
	protected:
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "PGenSample";
			treePath 	= "";
			
			PathGeneratorBase::ParameterInfo pi;
			//...
			setupCCReferencePoint(pi); 	// id = ?
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenSample() : PathGeneratorBase() {}
		virtual ~PGenSample() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool parameterChanging(PathGeneratorBase::ChangeCategory cc, int paraIdxInCat, const wxVariant& value) {
			// implement this method only if a parameter change should be rejected
			clog << cc << ", " << paraIdxInCat << ", " << value.GetString() << endl;
			// return false if the the parameter change shoudn't apply 
			return true; 
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool parameterChanged(PathGeneratorBase::ChangeCategory cc, int paraIdxInCat, const wxVariant& value) { 
			// implement this method only if a parameter change notification should be handled
			clog << cc << ", " << paraIdxInCat << ", " << value.GetString() << endl;
			// return false if the path shouldn't generated 
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

typedef std::vector<CncPolygonPoints> PolygonList;
///////////////////////////////////////////////////////////////////////////
class PGenPolygon : public PathGeneratorBase {
	private:
		PolygonList polygonDataList;
		
	protected:
		
		///////////////////////////////////////////////////////////////////
		PGenPolygon() 
		: PathGeneratorBase()
		{
			// initialize default polygon points - index = 0
			appendNextPolygonPoints();
		}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenPolygon() {
			for ( auto it=polygonDataList.begin(); it!=polygonDataList.end(); ++it)
				it->clear();
		}
		
		///////////////////////////////////////////////////////////////////
		unsigned int appendNextPolygonPoints() {
			CncPolygonPoints defaultPolygonPoints;
			polygonDataList.push_back(defaultPolygonPoints);
			
			return polygonDataList.size() - 1;
		}
		
		CncPolygonPoints& getPolygonData(unsigned int polygonIndex=0);
		
		unsigned int getDataPointCount(unsigned int polygonIndex=0) { return getPolygonData(polygonIndex).size(); }
		virtual void getInternalInformation(wxTextCtrl* ctl);
		const char* getDataPointsAsWktString(wxString& ret, unsigned int polygonIndex=0);
		const wxRealPoint& getMinPosition(unsigned int polygonIndex=0) { return getPolygonData(polygonIndex).getMinRealPos(); }
		const wxRealPoint& getMaxPosition(unsigned int polygonIndex=0) { return getPolygonData(polygonIndex).getMaxRealPos(); }
		
		bool centerPolygon(unsigned int polygonIndex=0);
		const wxRealPoint& determineCentroid(wxRealPoint& cp, unsigned int polygonIndex=0);
		
		const wxRealPoint& getPolygonDataPoint(unsigned int polygonIndex, unsigned pointIndex);
		const wxRealPoint& getFirstPolygonDataPoint(unsigned int polygonIndex=0) { return getPolygonDataPoint(polygonIndex, 0); }
		const wxRealPoint& getSecondPolygonDataPoint(unsigned int polygonIndex=0) { return getPolygonDataPoint(polygonIndex, 1); }
		const wxRealPoint& getLastPolygonDataPoint(unsigned int polygonIndex=0) { return getPolygonDataPoint(polygonIndex, getPolygonData(polygonIndex).size() - 1); }
		
		void clearPolygonData(unsigned int polygonIndex=0) { getPolygonData(polygonIndex).clear(); }
		int fillPolygonData(CncPolygonPoints& toFill, const wxString& data);
		int fillPolygonData(unsigned int polygonIndex, const wxString& data);
		void addPolygon(unsigned int polygonIndex, SvgPathGroup& spg, bool inlay=false);
		void addPolyLine(unsigned int polylineIndex, SvgPathGroup& spg);
		
		void spoolPolygon(SvgPathGroup& spg, const CncPolygonPoints& dataPoints);
};

///////////////////////////////////////////////////////////////////////////
class PGenFreehandPolygon : public PGenPolygon {
	private:
		unsigned int IDX_DATA;
		unsigned int IDX_INLAY;
		unsigned int IDX_REFPOINT;
		CncPolygonPoints origPolygonData;
		
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			SVGElementConverter::convertPolygonToPathData(origPolygonData, mm, path);
			addHelpContructPath(spg, path);
		}
		
	protected:
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name		= "Freehand";
			treePath 	= "Polygons [mm]";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupString("Data [mm]", "10.0,10.0\\n10.0,100.0\\n55.0,55.0", true);
			IDX_DATA = setupParameter(pi);

			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupTLReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
	public:
	
		///////////////////////////////////////////////////////////////////
		PGenFreehandPolygon() 
		: PGenPolygon() 
		, IDX_DATA(INT_MAX)
		, IDX_INLAY(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenFreehandPolygon() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			clearPolygonData();
			
			fillPolygonData(0, getParameterStringValue(IDX_DATA));
			if ( getDataPointCount() < 3 ) {
				addErrorInfo(wxString::Format("generate(): Not enough data points available: Current count: %d", getDataPointCount()));
				return false;
			}
			
			centerPolygon();
			origPolygonData = getPolygonData(0);
			addPolygon(0, spg, (getParameterEnumValue(IDX_INLAY) == 1));

			setTranslateX(abs(getMinPosition().x) + 5.0);
			setTranslateY(abs(getMinPosition().y) + 5.0);
			return spg.isOK();
		}
};

///////////////////////////////////////////////////////////////////////////
class PGenRegularRadiusPolygon : public PGenPolygon {
	private:
		unsigned int IDX_SECTIONS;
		unsigned int IDX_RADIUS;
		unsigned int IDX_INLAY;
		unsigned int IDX_REFPOINT;
		
	protected:
	
		///////////////////////////////////////////////////////////////
		virtual double getRadius() {
			return getParameterNumValue(IDX_RADIUS);
		}
		
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			double radius = getRadius();
			
			wxString path;
			SVGElementConverter::convertCircleToPathData(SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().x), 
			                                             SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().y), 
			                                             SvgUnitCalculator::convertMM2ReferenceUnit(radius), 
			                                             path);
			
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Regular - Points based on Radius";
			treePath 	= "Polygons [mm]";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Sections", 6, 1, 100, 0);
			pi.help = "Count of data points to connect on radius. Only values even values > 3 are valid. Exception value == 3";
			IDX_SECTIONS = setupParameter(pi); 
			
			pi.setupNum("Radius [mm]", 40.0, 1.0, 500.0, 3);
			pi.help = "Radius on which the data points take place.";
			IDX_RADIUS = setupParameter(pi); 
			
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupCCReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenRegularRadiusPolygon() 
		: PGenPolygon() 
		, IDX_SECTIONS(INT_MAX)
		, IDX_RADIUS(INT_MAX)
		, IDX_INLAY(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{}
		
		virtual ~PGenRegularRadiusPolygon() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool parameterChanging(PathGeneratorBase::ChangeCategory cc, int paraIdxInCat, const wxVariant& value) { 
			if ( cc == PathGeneratorBase::ChangeCategory::CC_TPL_CAT && paraIdxInCat == (int)IDX_SECTIONS ) {
				
				if ( value.GetInteger() < 3 )
					return false;
					
				if ( value.GetInteger() == 3 )
					return true;
				
				if ( value.GetInteger()%2 != 0 )
					return false;
			}
			
			return true; 
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			clearPolygonData();
			
			// calculate polygon data points
			double steps  = 360.0 / getParameterNumValue(IDX_SECTIONS);
			double radius = getParameterNumValue(IDX_RADIUS);

			for (double i=0; i<360; i+=steps ) {
				double x = cos(i*PI/180) * radius;
				double y = sin(i*PI/180) * radius;
				
				x = round(x * 1000.0) / 1000.0;
				y = round(y * 1000.0) / 1000.0;
						
				getPolygonData(0).append(x, y);
			}
			
			addPolygon(0, spg, (getParameterEnumValue(IDX_INLAY) == 1));
			
			setTranslateX(getParameterNumValue(IDX_RADIUS) + 5.0);
			setTranslateY(getParameterNumValue(IDX_RADIUS) + 5.0);
			
			return spg.isOK();
		}
};

///////////////////////////////////////////////////////////////////////////
class PGenRegularDiameterPolygon : public PGenRegularRadiusPolygon {
	private:
		unsigned int IDX_SECTIONS;
		unsigned int IDX_DIAMETER;
		unsigned int IDX_INLAY;
		unsigned int IDX_REFPOINT;
		
	protected:
	
		///////////////////////////////////////////////////////////////
		virtual double getRadius() {
			double diameter = getParameterNumValue(IDX_DIAMETER);
			int sections 	= getParameterNumValue(IDX_SECTIONS);
			double alpha  	= 360.0 / sections / 2;
			double b      	= tan(alpha*PI/180) * diameter / 2;
			
			return sqrt(pow(diameter/2, 2) + pow(b, 2));
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Regular - Points based on Diameter";
			treePath 	= "Polygons [mm]";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Sections", 6, 1, 100, 0);
			pi.help = "Count of data points to connect on radius. Only values even values > 3 are valid. Exception value == 3";
			IDX_SECTIONS = setupParameter(pi); 
			
			pi.setupNum("Diameter [mm]", 40.0, 1.0, 500.0, 3);
			pi.help = "Diameter between two opposite sides.";
			IDX_DIAMETER = setupParameter(pi); 
			
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupCCReferencePoint(pi);
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenRegularDiameterPolygon() 
		: PGenRegularRadiusPolygon() 
		, IDX_SECTIONS(INT_MAX)
		, IDX_DIAMETER(INT_MAX)
		, IDX_INLAY(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenRegularDiameterPolygon() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			clearPolygonData();
			
			int sections 	= getParameterNumValue(IDX_SECTIONS);
			double steps  	= 360.0 / sections;
			double radius   = getRadius();
		
			for (double i=0; i<360; i+=steps ) {
				double x = cos(i*PI/180) * radius;
				double y = sin(i*PI/180) * radius;
				
				x = round(x * 1000.0) / 1000.0;
				y = round(y * 1000.0) / 1000.0;
						
				getPolygonData(0).append(x, y);
			}
			
			addPolygon(0, spg, (getParameterEnumValue(IDX_INLAY) == 1));
			
			setTranslateX(getParameterNumValue(IDX_DIAMETER) / 2 + 5.0);
			setTranslateY(getParameterNumValue(IDX_DIAMETER) / 2 + 5.0);
			
			return spg.isOK();
		}
};

///////////////////////////////////////////////////////////////////////////
class PGenSvgElementPolygon : public PGenPolygon {
	private:
		unsigned int elementCount;
		PolygonList origPoligonList;
		
	protected:
		wxRealPoint centroid;
		
		///////////////////////////////////////////////////////////////////
		PGenSvgElementPolygon() 
		: PGenPolygon()
		, elementCount(1)
		, centroid(0.0, 0.0)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenSvgElementPolygon() {}
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& getElementAsSvgPath(unsigned int elementIndex) = 0;
		virtual double getXOffset() = 0;
		virtual double getYOffset() = 0;
		virtual bool getInlayMode() = 0;
		virtual unsigned int getElementCount() { return 1; }
		
		///////////////////////////////////////////////////////////////////
		const CncPolygonPoints& getOriginalPolygon(unsigned int idx=0) {
			static CncPolygonPoints defaultPolygon;
			
			if ( idx > origPoligonList.size() -1 ) 
				return defaultPolygon;
				
			return origPoligonList.at(idx);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			origPoligonList.clear();
			
			for ( unsigned int i=0; i<getElementCount(); i++) {
				clearPolygonData(i);
				
				SVGPathAssistant pa;
				pa.setCurveLibResolution(commonValues.curveLibResolution);
				
				if ( pa.processSvgNode(getElementAsSvgPath(i)) == false ) {
					addErrorInfo("processSvgNode failed");
					return false;
				}
				
				CncPathListManager plm = pa.getPathList();
				for ( auto it=plm.begin(); it!=plm.end(); ++it )
					getPolygonData(i).append(CncPolygonPoints::convertToIntPoint(it->abs));
					
				plm.getCentroid(centroid);
				
				origPoligonList.push_back(getPolygonData(i));
				addPolygon(i, spg, getInlayMode());
			}
			
			setTranslateX(getXOffset());
			setTranslateY(getYOffset());
			
			return spg.isOK();
		}
};

///////////////////////////////////////////////////////////////////////////
class PGenEllipticalArcPolygon : public PGenSvgElementPolygon {
	private:
		unsigned int IDX_RADIUS_X;
		unsigned int IDX_RADIUS_Y;
		unsigned int IDX_REFPOINT;
		unsigned int IDX_INLAY;
		
	protected:
		
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			SVGElementConverter::convertEllipseToPathData(SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().x), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().y), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_RADIUS_X)), 
														  SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_RADIUS_Y)), 
			                                              path);
			
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Circle/Elliptical ARC";
			treePath 	= "Polygons [mm]";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Radius X [mm]", 30.0, 1.0, 400.0, 3);
			pi.help = "Determine the x radius parameter of the elliptical arg.";
			IDX_RADIUS_X = setupParameter(pi);
			
			pi.setupNum("Radius Y [mm]", 30.0, 1.0, 400.0, 3);
			pi.help = "Determine the y radius parameter of the elliptical arg.";
			IDX_RADIUS_Y = setupParameter(pi); 
			
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupCCReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual double getXOffset() { return getParameterNumValue(IDX_RADIUS_X) + 5.0; }
		virtual double getYOffset() { return getParameterNumValue(IDX_RADIUS_Y) + 5.0; }
		virtual bool getInlayMode() { return (getParameterEnumValue(IDX_INLAY) == 1); }
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& getElementAsSvgPath(unsigned int elementIndex) {
			static wxString ellipse;
			ellipse.clear();
			SVGElementConverter::convertEllipseToPathData(0.0, 0.0, 
			                                              getParameterNumValue(IDX_RADIUS_X), 
			                                              getParameterNumValue(IDX_RADIUS_Y), 
			                                              ellipse);
			ellipse.Prepend("<path d=\"");
			ellipse.Append("\"/>");
			
			return ellipse;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenEllipticalArcPolygon() 
		: PGenSvgElementPolygon() 
		, IDX_RADIUS_X(INT_MAX)
		, IDX_RADIUS_Y(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		, IDX_INLAY(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenEllipticalArcPolygon() {}
		
};

///////////////////////////////////////////////////////////////////////////
class PGenRectanglePolygon : public PGenSvgElementPolygon {
	private:
		unsigned int IDX_WIDTH;
		unsigned int IDX_HEIGHT;
		unsigned int IDX_CORNER_STYLE;
		unsigned int IDX_REFPOINT;
		unsigned int IDX_INLAY;
		
	protected:
		
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			
			// square corners
			double rx = 0.0, ry = 0.0;
			if ( getParameterEnumValue(IDX_CORNER_STYLE) == 0 ) {
				//round corners
				rx = ry = commonValues.toolDiameter/2;
			}
			
			SVGElementConverter::convertRectToPathData(SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().x), 
			                                           SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().y), 
			                                           SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_WIDTH)), 
			                                           SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_HEIGHT)), 
			                                           rx,
			                                           ry,
			                                           path);
			
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Rectangle";
			treePath 	= "Polygons [mm]";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Width [mm]", 40.0, 1.0, 400.0, 3);
			pi.help = "Determine rect width (x axis).";
			IDX_WIDTH = setupParameter(pi);
			
			pi.setupNum("Height [mm]", 10.0, 1.0, 400.0, 3);
			pi.help = "Determine rect height (y axis).";
			IDX_HEIGHT = setupParameter(pi);
			
			pi.setupEnum("Corners Style", "round;square", 0);
			pi.help = "Determine corner style.";
			IDX_CORNER_STYLE = setupParameter(pi);
			
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupReferencePoint(pi, "center/center;left/center;left/top;left/bottom", 1);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			switch ( getParameterEnumValue(IDX_REFPOINT) ) {
				// center/center
				case 0: 	determineReferencePoint(spg, -getParameterNumValue(IDX_WIDTH)/2, -getParameterNumValue(IDX_HEIGHT)/2);
							break;
				// left/center
				case 1: 	determineReferencePoint(spg, 0.0, -getParameterNumValue(IDX_HEIGHT)/2);
							break;
				// left/top
				case 2:		determineReferencePoint(spg, 0.0, 0.0);
							break;
				// left bootom
				case 3:		determineReferencePoint(spg, 0.0, -getParameterNumValue(IDX_HEIGHT));
							break;
							
				default:	addErrorInfo(wxString::Format("setReferencePoint: invalid enum: %d", getParameterEnumValue(IDX_REFPOINT)));
							return false;
			}
			
			return true;
		}

		///////////////////////////////////////////////////////////////////
		virtual double getXOffset() { return getParameterNumValue(IDX_WIDTH) + 5.0; }
		virtual double getYOffset() { return getParameterNumValue(IDX_HEIGHT) + 5.0; }
		virtual bool getInlayMode() { return (getParameterEnumValue(IDX_INLAY) == 1); }
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& getElementAsSvgPath(unsigned int elementIndex) {
			static wxString rect;
			rect.clear();
			
			// square corners
			double rx = 0.0, ry = 0.0;
			if ( getParameterEnumValue(IDX_CORNER_STYLE) == 0 ) {
				//round corners
				rx = ry = commonValues.toolDiameter/2;
			}
			
			// ref point
			double x = 0.0, y = 0.0;
			if ( isReferencePointDefined() ) {
				x = getReferencePoint().x;
				y = getReferencePoint().y;
			}
			
			SVGElementConverter::convertRectToPathData(x, 
			                                           y, 
			                                           getParameterNumValue(IDX_WIDTH), 
			                                           getParameterNumValue(IDX_HEIGHT), 
			                                           rx,
			                                           ry,
			                                           rect);
			rect.Prepend("<path d=\"");
			rect.Append("\"/>");
			
			return rect;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenRectanglePolygon() 
		: PGenSvgElementPolygon() 
		, IDX_WIDTH(INT_MAX)
		, IDX_HEIGHT(INT_MAX)
		, IDX_CORNER_STYLE(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		, IDX_INLAY(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenRectanglePolygon() {}
		
};

///////////////////////////////////////////////////////////////////////////
class PGenSimplePoint : public PGenPolygon {
	private:
		unsigned int IDX_X;
		unsigned int IDX_Y;
		unsigned int IDX_REFPOINT;
	
	protected:
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Simple Point";

			commonValues.canJointType = false;
			
			PathGeneratorBase::ParameterInfo pi;
			
			pi.setupNum("X [mm]", 10.0, 0.0, 500.0, 3);
			pi.help = "Position X";
			IDX_X = setupParameter(pi); 
			
			pi.setupNum("Y [mm]", 15.0, 0.0, 500.0, 3);
			pi.help = "Position Y";
			IDX_Y = setupParameter(pi); 
			
			IDX_REFPOINT = setupCCReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenSimplePoint() 
		: PGenPolygon() 
		, IDX_X(INT_MAX)
		, IDX_Y(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenSimplePoint() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			clearPolygonData();
			
			getPolygonData().append(getParameterNumValue(IDX_X), getParameterNumValue(IDX_Y));
			getPolygonData().append(getParameterNumValue(IDX_X), getParameterNumValue(IDX_Y));
			addPolyLine(0, spg);
			
			setTranslateX(getParameterNumValue(IDX_X) + 5.0);
			setTranslateY(getParameterNumValue(IDX_Y) + 5.0);
			
			return spg.isOK();
		}
};

///////////////////////////////////////////////////////////////////////////
class PGenSimpleLine : public PGenPolygon {
	private:
		unsigned int IDX_X1;
		unsigned int IDX_Y1;
		unsigned int IDX_X2;
		unsigned int IDX_Y2;
		unsigned int IDX_REFPOINT;
		CncPolygonPoints origPolygonData;
	
	protected:
	
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			SVGElementConverter::convertPolylineToPathData(origPolygonData, mm, path);
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Simple Line";
			treePath 	= "Polylines [mm]";
			
			commonValues.canJointType = false;
			
			PathGeneratorBase::ParameterInfo pi;
			
			pi.setupNum("X1 [mm]", 0.0, 0.0, 500.0, 3);
			pi.help = "Start position X";
			IDX_X1 = setupParameter(pi); 
			
			pi.setupNum("Y1 [mm]", 0.0, 0.0, 500.0, 3);
			pi.help = "Start position Y";
			IDX_Y1 = setupParameter(pi); 

			pi.setupNum("X2 [mm]", 60.0, 0.0, 500.0, 3);
			pi.help = "End position X";
			IDX_X2 = setupParameter(pi); 
			
			pi.setupNum("Y2 [mm]", 70.0, 0.0, 500.0, 3);
			pi.help = "End position Y";
			IDX_Y2 = setupParameter(pi); 
			
			IDX_REFPOINT = setupReferencePoint(pi,"start");
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenSimpleLine() 
		: PGenPolygon() 
		, IDX_X1(INT_MAX)
		, IDX_Y1(INT_MAX)
		, IDX_X2(INT_MAX)
		, IDX_Y2(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenSimpleLine() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			clearPolygonData();
			
			getPolygonData().append(getParameterNumValue(IDX_X1), getParameterNumValue(IDX_Y1));
			getPolygonData().append(getParameterNumValue(IDX_X2), getParameterNumValue(IDX_Y2));
			origPolygonData = getPolygonData();
			
			addPolyLine(0, spg);
			
			setTranslateX(abs(getMinPosition().x) + 5.0);
			setTranslateY(abs(getMinPosition().y) + 5.0);
			
			return spg.isOK();
		}
};

///////////////////////////////////////////////////////////////////////////
class PGenFreehandPolyline : public PGenPolygon {
	private:
		unsigned int IDX_DATA;
		unsigned int IDX_REFPOINT;
		CncPolygonPoints origPolygonData;
		
	protected:
	
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			SVGElementConverter::convertPolylineToPathData(origPolygonData, mm, path);
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name		= "Freehand";
			treePath 	= "Polylines [mm]";
			
			commonValues.canJointType = false;
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupString("Data [mm]", "0.0,0.0\\n10.0,100.0\\n55.0,55.0\\n95.0,95.0", true);
			IDX_DATA= setupParameter(pi);
			
			IDX_REFPOINT = setupReferencePoint(pi,"start");
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
	public:
	
		///////////////////////////////////////////////////////////////////
		PGenFreehandPolyline() 
		: PGenPolygon() 
		, IDX_DATA(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenFreehandPolyline() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			clearPolygonData();
			
			const unsigned int polygonIndex = 0;
			fillPolygonData(polygonIndex, getParameterStringValue(IDX_DATA));
			origPolygonData = getPolygonData(polygonIndex);
			addPolyLine(polygonIndex, spg);

			setTranslateX(abs(getMinPosition().x) + 5.0);
			setTranslateY(abs(getMinPosition().y) + 5.0);
			return spg.isOK();
		}
};

///////////////////////////////////////////////////////////////////////////
class PGenericPath : public PGenSvgElementPolygon {
	
	private:
		unsigned int IDX_DATA;
		unsigned int IDX_INLAY;
		unsigned int IDX_REFPOINT;
	
	protected:
		
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			SVGElementConverter::convertPolylineToPathData(getOriginalPolygon(), px, path);
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "SVG Path";
			treePath 	= "Generic [px]";
			
			PathGeneratorBase::ParameterInfo pi;
			
			pi.setupString("Path d [px]", "M348.000,257.000 L 364.000,148.000 362.000,148.000 326.000,241.000 295.000,219.000 258.000,88.000 440.000,129.000 370.000,196.000 372.000,275.000 z", true);
			IDX_DATA = setupParameter(pi); 
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupTLReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		//virtual double getXOffset() { return + 5.0; }
		//virtual double getYOffset() { return + 5.0; }
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
			path.assign(getParameterStringValue(IDX_DATA));
		
			path.Prepend("<path d=\"");
			path.Append("\"/>");
			
			return path;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenericPath() 
		: PGenSvgElementPolygon()
		, IDX_DATA(INT_MAX)
		, IDX_INLAY(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{
			setInputUnit(px);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenericPath() {}
		
};

///////////////////////////////////////////////////////////////////////////
class PGenericElement : public PGenSvgElementPolygon {
	
	private:
		unsigned int IDX_DATA;
		unsigned int IDX_INLAY;
		unsigned int IDX_REFPOINT;
	
	protected:
	
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			SVGElementConverter::convertPolylineToPathData(getOriginalPolygon(), px, path);
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "SVG Element";
			treePath 	= "Generic [px]";
			
			PathGeneratorBase::ParameterInfo pi;
			
			pi.setupString("<Element ...> [px]", "<circle cx=\"0\" cy=\"0\" r=\"47\"/>", true);
			IDX_DATA = setupParameter(pi); 
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupTLReferencePoint(pi);
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
			path.assign(getParameterStringValue(IDX_DATA));
			return path;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenericElement() 
		: PGenSvgElementPolygon()
		, IDX_DATA(INT_MAX)
		, IDX_INLAY(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{
			setInputUnit(px);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenericElement() {}
		
};

///////////////////////////////////////////////////////////////////////////
class PGenRotaryKnob : public PGenericPath {
	
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
		: PGenericPath()
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

///////////////////////////////////////////////////////////////////////////
class PGenEllipticalRing : public PGenSvgElementPolygon {
	private:
		unsigned int IDX_OUTER_RADIUS_X;
		unsigned int IDX_OUTER_RADIUS_Y;
		unsigned int IDX_INNER_RADIUS_X;
		unsigned int IDX_INNER_RADIUS_Y;
		unsigned int IDX_REFPOINT;
		unsigned int IDX_INLAY;
		
	protected:
		
		///////////////////////////////////////////////////////////////
		virtual void generateHelpConstructs(SvgPathGroup& spg) {
			wxString path;
			// outer
			SVGElementConverter::convertEllipseToPathData(SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().x), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().y), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_OUTER_RADIUS_X)), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_OUTER_RADIUS_Y)), 
			                                              path);
			
			addHelpContructPath(spg, path);
			
			// inner
			path.clear();
			SVGElementConverter::convertEllipseToPathData(SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().x), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().y), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_INNER_RADIUS_X)), 
			                                              SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_INNER_RADIUS_Y)), 
			                                              path);
			
			addHelpContructPath(spg, path);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Circle/Elliptical Ring";
			treePath 	= "Advanced Polygons [mm]";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Outer Radius X [mm]", 30.0, 1.0, 400.0, 3);
			pi.help = "Determine the outer x radius parameter of the elliptical arg.";
			IDX_OUTER_RADIUS_X = setupParameter(pi);
			
			pi.setupNum("Outer Radius Y [mm]", 30.0, 1.0, 400.0, 3);
			pi.help = "Determine the outer y radius parameter of the elliptical arg.";
			IDX_OUTER_RADIUS_Y = setupParameter(pi); 
			
			pi.setupNum("Inner Radius X [mm]", 15.0, 1.0, 400.0, 3);
			pi.help = "Determine the inner x radius parameter of the elliptical arg.";
			IDX_INNER_RADIUS_X = setupParameter(pi);
			
			pi.setupNum("Inner Radius Y [mm]", 15.0, 1.0, 400.0, 3);
			pi.help = "Determine the inner y radius parameter of the elliptical arg.";
			IDX_INNER_RADIUS_Y = setupParameter(pi); 
			
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupCCReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual unsigned int getElementCount() { return 2; }
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual double getXOffset() { return getParameterNumValue(IDX_OUTER_RADIUS_X) + 5.0; }
		virtual double getYOffset() { return getParameterNumValue(IDX_OUTER_RADIUS_Y) + 5.0; }
		virtual bool getInlayMode() { return (getParameterEnumValue(IDX_INLAY) == 1); }
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& getElementAsSvgPath(unsigned int elementIndex) {
			static wxString ellipse;
			ellipse.clear();
			
			switch (elementIndex) {
				// outer
				case 0:	SVGElementConverter::convertEllipseToPathData(0.0, 0.0, 
																	  getParameterNumValue(IDX_OUTER_RADIUS_X), 
																	  getParameterNumValue(IDX_OUTER_RADIUS_Y), 
																	  ellipse);
						ellipse.Prepend("<path d=\"");
						ellipse.Append("\"/>");
						break;
						
				// inner
				case 1:	SVGElementConverter::convertEllipseToPathData(0.0, 0.0, 
																	  getParameterNumValue(IDX_INNER_RADIUS_X), 
																	  getParameterNumValue(IDX_INNER_RADIUS_Y), 
																	  ellipse);
						ellipse.Prepend("<path d=\"");
						ellipse.Append("\"/>");
						break;
			}
			
			return ellipse;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenEllipticalRing() 
		: PGenSvgElementPolygon() 
		, IDX_OUTER_RADIUS_X(INT_MAX)
		, IDX_OUTER_RADIUS_Y(INT_MAX)
		, IDX_INNER_RADIUS_X(INT_MAX)
		, IDX_INNER_RADIUS_Y(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		, IDX_INLAY(INT_MAX)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenEllipticalRing() {}
		
};

#endif
