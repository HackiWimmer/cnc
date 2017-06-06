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

///////////////////////////////////////////////////////////////////////////
class PGenTest : public PathGeneratorBase {
	
	protected:
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "PGenTest";
			treePath 	= "Sample\\Categorie 1\\ next";
			
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
	
	public:
		///////////////////////////////////////////////////////////////////
		PGenTest() : PathGeneratorBase() {}
		virtual ~PGenTest() {}
		
		///////////////////////////////////////////////////////////////////
		virtual bool parameterChanged(PathGeneratorBase::ChangeCategory cc, int paraIdxInCat, const wxVariant& value) { 
			// implement this method only if a parameter change notification should be handled
			clog << cc << endl;
			// return false if the path shouldn't generated 
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
class PGenPolygon : public PathGeneratorBase {
	
	protected:
		CncPolygonPoints polygonData;
	
		///////////////////////////////////////////////////////////////////
		PGenPolygon() 
		: PathGeneratorBase()
		{
		}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenPolygon() {
			polygonData.clear();
		}
		
		unsigned int getDataPointCount() const { return polygonData.size(); }
		virtual void getInternalInformation(wxTextCtrl* ctl);
		const char* getDataPointsAsWktString(wxString& ret);
		const wxRealPoint& getMinPosition() const { return polygonData.getMinRealPos(); }
		const wxRealPoint& getMaxPosition() const { return  polygonData.getMaxRealPos(); }
		
		bool centerPolygon();
		const wxRealPoint& determineCentroid(wxRealPoint& cp);
		
		const wxRealPoint& getPolygonDataPoint(unsigned index);
		const wxRealPoint& getFirstPolygonDataPoint() { return getPolygonDataPoint(0); }
		const wxRealPoint& getSecondPolygonDataPoint() { return getPolygonDataPoint(1); }
		const wxRealPoint& getLastPolygonDataPoint() { return getPolygonDataPoint(polygonData.size() - 1); }
		
		void clearPolygonData() { polygonData.clear(); }
		int fillPolygonData(CncPolygonPoints& toFill, const wxString& data);
		int fillPolygonData(const wxString& data);
		void addPolygon(SvgPathGroup& spg, bool inlay=false);
		void addPolyLine(SvgPathGroup& spg);
		
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
			treePath 	= "Polygon [mm]";
			
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
			
			fillPolygonData(getParameterStringValue(IDX_DATA));
			if ( getDataPointCount() < 3 ) {
				addErrorInfo(wxString::Format("generate(): Not enough data points available: Current count: %d", getDataPointCount()));
				return false;
			}
			
			centerPolygon();
			origPolygonData = polygonData;
			addPolygon(spg, (getParameterEnumValue(IDX_INLAY) == 1));

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
			treePath 	= "Polygon [mm]";
			
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
						
				polygonData.append(x, y);
			}
			
			addPolygon(spg, (getParameterEnumValue(IDX_INLAY) == 1));
			
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
			treePath 	= "Polygon [mm]";
			
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
						
				polygonData.append(x, y);
			}
			
			addPolygon(spg, (getParameterEnumValue(IDX_INLAY) == 1));
			
			setTranslateX(getParameterNumValue(IDX_DIAMETER) / 2 + 5.0);
			setTranslateY(getParameterNumValue(IDX_DIAMETER) / 2 + 5.0);
			
			return spg.isOK();
		}
};

///////////////////////////////////////////////////////////////////////////
class PGenSvgElementPolygon : public PGenPolygon {
	
	protected:
		wxRealPoint centroid;
		CncPolygonPoints origPolygonData;
		
		///////////////////////////////////////////////////////////////////
		PGenSvgElementPolygon() 
		: PGenPolygon() 
		, centroid(0.0, 0.0)
		{}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenSvgElementPolygon() {}
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& getElementAsSvgPath() = 0;
		virtual double getXOffset() = 0;
		virtual double getYOffset() = 0;
		virtual bool getInlayMode() = 0;
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			clearPolygonData();
			
			SVGPathAssistant pa;
			pa.setCurveLibResolution(commonValues.curveLibResolution);
			
			if ( pa.processSvgNode(getElementAsSvgPath()) == false ) {
				addErrorInfo("processSvgNode failed");
				return false;
			}
			
			CncPathListManager plm = pa.getPathList();
			for ( auto it=plm.begin(); it!=plm.end(); ++it )
				polygonData.append(CncPolygonPoints::convertToIntPoint(it->abs));
				
			plm.getCentroid(centroid);
			
			origPolygonData = polygonData;
			addPolygon(spg, getInlayMode());
			
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
			treePath 	= "Polygon [mm]";
			
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
		virtual const wxString& getElementAsSvgPath() {
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
			treePath 	= "Polygon [mm]";
			
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
		virtual const wxString& getElementAsSvgPath() {
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
			
			polygonData.append(getParameterNumValue(IDX_X), getParameterNumValue(IDX_Y));
			polygonData.append(getParameterNumValue(IDX_X), getParameterNumValue(IDX_Y));
			addPolyLine(spg);
			
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
			treePath 	= "Polyline [mm]";
			
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
			
			polygonData.append(getParameterNumValue(IDX_X1), getParameterNumValue(IDX_Y1));
			polygonData.append(getParameterNumValue(IDX_X2), getParameterNumValue(IDX_Y2));
			origPolygonData = polygonData;
			
			addPolyLine(spg);
			
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
			treePath 	= "Polyline [mm]";
			
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
			
			fillPolygonData(getParameterStringValue(IDX_DATA));
			origPolygonData = polygonData;
			addPolyLine(spg);

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
			SVGElementConverter::convertPolylineToPathData(origPolygonData, px, path);
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
		virtual double getXOffset() { return SvgUnitCalculator::convertReferenceUnit2MM(abs(polygonData.getMinRealPos().x)) + 5.0; }
		virtual double getYOffset() { return SvgUnitCalculator::convertReferenceUnit2MM(abs(polygonData.getMinRealPos().y)) + 5.0; }
		virtual bool getInlayMode() { return (getParameterEnumValue(IDX_INLAY) == 1); }
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& getElementAsSvgPath() {
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
			SVGElementConverter::convertPolylineToPathData(origPolygonData, px, path);
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
		virtual double getXOffset() { return SvgUnitCalculator::convertReferenceUnit2MM(abs(polygonData.getMinRealPos().x)) + 5.0; }
		virtual double getYOffset() { return SvgUnitCalculator::convertReferenceUnit2MM(abs(polygonData.getMinRealPos().y)) + 5.0; }
		virtual bool getInlayMode() { return (getParameterEnumValue(IDX_INLAY) == 1); }
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& getElementAsSvgPath() {
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
		unsigned int IDX_SECTIONS;
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
			SVGElementConverter::convertPolylineToPathData(origPolygonData, px, path);
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
			treePath 	= "Polygon [mm]";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Sections", 12, 1, 100, 0);
			pi.help = "Count of data points to connect on diameter. Only values even values > 3 are valid";
			IDX_SECTIONS = setupParameter(pi); 
			
			pi.setupNum("Knob Diameter [mm]", 60.0, 1.0, 120.0, 3);
			pi.help = "Diameter of the shaft whole. A value of 0.0 means no shaft and negative values . . .";
			IDX_KNOB_DIAMETER = setupParameter(pi); 
			
			pi.setupEnum("Shaft Type", "Pre-drilled (tool diameter);Whole (shaft diameter)", 0);
			pi.help = "Determines shaft type.";
			IDX_SHAFT_TYPE = setupParameter(pi); 
			
			pi.setupNum("Shaft Diameter [mm]", 10.0, -1, 40.0, 3);
			pi.help = "Diameter of rotary knob.";
			IDX_SHAFT_DIAMETER = setupParameter(pi); 
			
			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupCCReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual double getXOffset() { return SvgUnitCalculator::convertReferenceUnit2MM(abs(polygonData.getMinRealPos().x)) + 5.0; }
		virtual double getYOffset() { return SvgUnitCalculator::convertReferenceUnit2MM(abs(polygonData.getMinRealPos().y)) + 5.0; }
		virtual bool getInlayMode() { return (getParameterEnumValue(IDX_INLAY) == 1); }
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual const wxString& getElementAsSvgPath() {
			static wxString path;
			path.assign("<path d=\"");
			 
			// collect parameters
			unsigned int sections 	= getParameterNumValue(IDX_SECTIONS);
			double steps  			= 360.0 / sections;
			double radius			= getParameterNumValue(IDX_KNOB_DIAMETER) / 2.0;
			
			// store initial move
			path.append(wxString::Format("M%lf,0 ", SvgUnitCalculator::convertMM2ReferenceUnit(-radius)));
			
			double pos = 0.0;
			for ( unsigned int i=0; i<=sections; i++ ) {
				double x = cos(pos*PI/180) * radius;
				double y = sin(pos*PI/180) * radius;
				
				x = round(x * 1000.0) / 1000.0;
				y = round(y * 1000.0) / 1000.0;
				
				
				//clog << wxString::Format("%2d: %10.3lf %10.3lf", i, x, y ) << endl;
				
				if ( i%2==0 ) {
					path.append(wxString::Format("A %lf %lf 0 0 1 %lf,%lf ", SvgUnitCalculator::convertMM2ReferenceUnit(radius*2),
																			 SvgUnitCalculator::convertMM2ReferenceUnit(radius*2),
																			 SvgUnitCalculator::convertMM2ReferenceUnit(-x),
																			 SvgUnitCalculator::convertMM2ReferenceUnit(-y)));
				} else {
					path.append(wxString::Format("A %lf %lf 0 0 0 %lf,%lf ", SvgUnitCalculator::convertMM2ReferenceUnit(radius * 2),
																			 SvgUnitCalculator::convertMM2ReferenceUnit(radius * 2),
																			 SvgUnitCalculator::convertMM2ReferenceUnit(-x),
																			 SvgUnitCalculator::convertMM2ReferenceUnit(-y)));
																			  
				}
				pos += steps;
			}
			
			wxString shaft("M0,0 ");
			if ( getParameterEnumValue(IDX_SHAFT_TYPE) == 0 ) {
				shaft.Append("L0,0 ");
			} else {
				wxString whole;
				SVGElementConverter::convertCircleToPathData(SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().x),
															 SvgUnitCalculator::convertMM2ReferenceUnit(getReferencePoint().y), 
															 SvgUnitCalculator::convertMM2ReferenceUnit(getParameterNumValue(IDX_SHAFT_DIAMETER) / 2.0),
															 whole);
				shaft.Append(whole);
			}
			
			path.Append(shaft);
			path.Append("\"/>");
			return path;
		}
		
	public:
		///////////////////////////////////////////////////////////////////
		PGenRotaryKnob() 
		: PGenericPath()
		, IDX_SECTIONS(INT_MAX)
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
};

#endif
