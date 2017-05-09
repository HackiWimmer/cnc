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

///////////////////////////////////////////////////////////////////////////
class PGenSample : public PathGeneratorBase {
	
	public:
		///////////////////////////////////////////////////////////////////
		PGenSample() : PathGeneratorBase() {}
		virtual ~PGenSample() {}
		
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
	
	public:
		///////////////////////////////////////////////////////////////////
		PGenTest() : PathGeneratorBase() {}
		virtual ~PGenTest() {}
		
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




///////////////////////////////////////////////////////////////////////////
class PGenPolygon : public PathGeneratorBase {
	
	private:
		PathFragmentPolygonData polygonData;
		wxRealPoint pMin;
		wxRealPoint pMax;
		
	protected: 
		
		///////////////////////////////////////////////////////////////////
		PGenPolygon() 
		: PathGeneratorBase()
		, pMin(0.0, 0.0)
		, pMax(0.0, 0.0)
		{
		}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PGenPolygon() {
			polygonData.clear();
		}
		
		///////////////////////////////////////////////////////////////////
		unsigned int getDataPointCount() const {
			return polygonData.size();
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void getInternalInformation(wxTextCtrl* ctl) {
			wxASSERT(ctl);
			
			ctl->Clear();
			ctl->SetDefaultStyle(wxTextAttr(*wxWHITE));
			
			// data points
			std::stringstream ss;
			ss << "Polyon::Data::Points: generic format\n";
			for (PathFragmentPolygonData::iterator it = polygonData.begin(); it != polygonData.end(); ++it) {
				ss << *it << "\n";
			}
			
			// data points wkt style
			ctl->AppendText(ss.str().c_str());
			ctl->AppendText("\nPolyon::Data::Points wkt format:\n");
			wxString wkt;
			ctl->AppendText(getDataPointsAsWktString(wkt));
			ctl->AppendText("\n\n");
			
			// min max positons
			ctl->AppendText(wxString::Format("Polyon::Data::Min: %.3lf, %.3lf\n", getMinPosition().x, getMinPosition().y));
			ctl->AppendText(wxString::Format("Polyon::Data::Max: %.3lf, %.3lf\n", getMaxPosition().x, getMaxPosition().y));
			
			// final line feed
			ctl->AppendText("\n");
		}
		
		///////////////////////////////////////////////////////////////////
		const char* getDataPointsAsWktString(wxString& ret) {
			ret.assign("POLYGON((");
			
			unsigned int cnt = 0;
			PathFragmentPolygonData::iterator it;
			for ( it = polygonData.begin(); it != polygonData.end(); ++it ) {
				if ( cnt == 0 )	ret.Append(wxString::Format("%.3lf %.3lf", it->getTransformedPoint().x, it->getTransformedPoint().y));
				else			ret.Append(wxString::Format(",%.3lf %.3lf", it->getTransformedPoint().x, it->getTransformedPoint().y));
				cnt++;
			}
			
			// append first point at the end too
			it = polygonData.begin();
			ret.Append(wxString::Format(",%.3lf %.3lf", it->getTransformedPoint().x, it->getTransformedPoint().y));
			
			ret.Append("))");
			return ret;
		}
		
		///////////////////////////////////////////////////////////////////
		bool centerPolygon() {
			wxRealPoint cp;
			determineCentroid(cp);
			
			if ( cnc::dblCompareNull(cp.x) == true && cnc::dblCompareNull(cp.x) == true )
				return true;
				
			for (PathFragmentPolygonData::iterator it = polygonData.begin(); it != polygonData.end(); ++it) {
				wxRealPoint op(it->getOriginalPoint());
				wxRealPoint np = op - cp;
				
				changeDataPoint(it, np);
			}
			
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		const wxRealPoint& getPolygonDataPoint(unsigned index) {
			if ( index > polygonData.size() -1 ) {
				addErrorInfo(wxString::Format("getPolygonDataPoint(): Invalid index: %d, current count: %d", index, polygonData.size()));
				static wxRealPoint p;
				return p;
			}
			
			return polygonData.at(index).getTransformedPoint();
		}
		
		///////////////////////////////////////////////////////////////////
		const wxRealPoint& getFirstPolygonDataPoint() {
			return getPolygonDataPoint(0);
		}
		
		///////////////////////////////////////////////////////////////////
		const wxRealPoint& getSecondPolygonDataPoint() {
			return getPolygonDataPoint(1);
		}
		
		///////////////////////////////////////////////////////////////////
		const wxRealPoint& getLastPolygonDataPoint() {
			return getPolygonDataPoint(polygonData.size() - 1);
		}
		
		///////////////////////////////////////////////////////////////////
		int fillPolygonData(const wxString& data) {
			polygonData.clear();
			
			wxString d(data);
			d.Replace("\\n", ";", true);
			d.Replace("\\t", "",  true);
			
			wxStringTokenizer lineTokenizer(d, ";");
			while ( lineTokenizer.HasMoreTokens() ) {
				wxString token = lineTokenizer.GetNextToken();
				
				if ( token.Matches("*,*") == true ) {
					wxStringTokenizer pointTokenizer(token, ",");
					wxString xv = pointTokenizer.GetNextToken();
					wxString yv = pointTokenizer.GetNextToken();
					
					wxRealPoint p;
					xv.ToDouble(&p.x);
					yv.ToDouble(&p.y);
					appendPolygonPoint(p);
				}
			}
			
			return polygonData.size();
		}
		
		///////////////////////////////////////////////////////////////////
		void appendPolygonPoint(const wxRealPoint& p) {
			if ( polygonData.size() == 0 ) {
				pMin = p;
				pMax = p;
			} else {
				if ( p.x < pMin.x ) pMin.x = p.x;
				if ( p.y < pMin.y ) pMin.y = p.y;
				if ( p.x > pMax.x ) pMax.x = p.x;
				if ( p.y > pMax.y ) pMax.y = p.y;
			}
			
			polygonData.push_back(p);
		}
		
		///////////////////////////////////////////////////////////////////
		void changeDataPoint(PathFragmentPolygonData::iterator it, const wxRealPoint& p) {
			if ( p.x < pMin.x ) pMin.x = p.x;
			if ( p.y < pMin.y ) pMin.y = p.y;
			if ( p.x > pMax.x ) pMax.x = p.x;
			if ( p.y > pMax.y ) pMax.y = p.y;
						
			it->setOriginalPoint(p);
		}
		
		///////////////////////////////////////////////////////////////////
		void transformPolygon(SVGTransformer& t) {
			for (PathFragmentPolygonData::iterator it = polygonData.begin(); it != polygonData.end(); ++it) {
				it->transform(t);
			}
		}
		
		///////////////////////////////////////////////////////////////////
		void transformPolygonAgain(SVGTransformer& t) {
			for (PathFragmentPolygonData::iterator it = polygonData.begin(); it != polygonData.end(); ++it) {
				it->transformAgain(t);
			}
		}
		
		///////////////////////////////////////////////////////////////////
		const wxRealPoint& determineCentroid(wxRealPoint& cp) {
			try {
				typedef boost::geometry::model::d2::point_xy<double> point_type;
				typedef boost::geometry::model::polygon<point_type> polygon_type;

				polygon_type poly;
				wxString wkt;
				boost::geometry::read_wkt(getDataPointsAsWktString(wkt), poly);

				point_type p(0.0, 0.0);
				boost::geometry::centroid(poly, p);
				
				cp.x = (cnc::dblCompare(p.x(), 0.0, 0.001) == true ? 0.0 : p.x());
				cp.y = (cnc::dblCompare(p.y(), 0.0, 0.001) == true ? 0.0 : p.y());
			}
			catch (...) {
				addErrorInfo("determineCentroid(): Error while determine centroid");
			}
			return cp;
		}
		
		///////////////////////////////////////////////////////////////////
		void addPolygon(SvgPathGroup& spg, bool inlay=false) {
			spg.add(spg.fGen().addPolygon(polygonData));
			
			if ( inlay == true ) {
				// todo factor + loop
				for ( int i=0; i<5; i++ ) {
					wxRealPoint cp;
					determineCentroid(cp);
					
					SVGTransformer t;
					t.applyScale(0.8);
					
					if ( i == 0 )	transformPolygon(t);
					else			transformPolygonAgain(t);
					
					spg.add(spg.fGen().addPolygon(polygonData));
				}
			}
		}
		
		///////////////////////////////////////////////////////////////////
		void clearPolygonData() {
			polygonData.clear();
		}
		
		///////////////////////////////////////////////////////////////////
		const wxRealPoint& getMinPosition() const {
			return pMin;
		}
		
		///////////////////////////////////////////////////////////////////
		const wxRealPoint& getMaxPosition() const {
			return pMax;
		}
		
	public:
};

///////////////////////////////////////////////////////////////////////////
class PGenFreehandPolygon : public PGenPolygon {
	private:
		unsigned int IDX_DATA;
		unsigned int IDX_INLAY;
		unsigned int IDX_REFPOINT;
		
	public:
	
		///////////////////////////////////////////////////////////////////
		PGenFreehandPolygon() 
		: PGenPolygon() 
		, IDX_DATA(INT_MAX)
		, IDX_INLAY(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{}
		
		virtual ~PGenFreehandPolygon() {}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name		= "Freehand";
			treePath 	= "Polygon";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupString("Data [mm]", "10.0,10.0\\n10.0,100.0\\n55.0,55.0", true);
			IDX_DATA= setupParameter(pi);

			IDX_INLAY = setupInlayMode(pi, "Path;Whole", 0);
			IDX_REFPOINT = setupTLReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			clearPolygonData();
			
			fillPolygonData(getParameterStringValue(0));
			if ( getDataPointCount() < 3 ) {
				addErrorInfo(wxString::Format("generate(): Not enough data points available: Current count: %d", getDataPointCount()));
				return false;
			}
			
			centerPolygon();
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
		virtual void initParameters()  {
			name 		= "Regular - Radius";
			treePath 	= "Polygon";
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupNum("Sections", 6, 1, 100, 0);
			pi.help = "Count of data points to connect on radius. Only values even values > 3 are valid. Exception value == 3";
			IDX_SECTIONS = setupParameter(pi); 
			
			pi.setupNum("Radius [mm]", 40.0, 5.0, 500.0, 3);
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
			double radius = (commonValues.toolCorrection == true ? getParameterNumValue(IDX_RADIUS) - commonValues.toolDiameter/2 : getParameterNumValue(1));

			for (double i=0; i<360; i+=steps ) {
				double x = cos(i*PI/180) * radius;
				double y = sin(i*PI/180) * radius;
				
				x = round(x * 1000.0) / 1000.0;
				y = round(y * 1000.0) / 1000.0;
						
				appendPolygonPoint({x,y});
			}
			
			addPolygon(spg, (getParameterEnumValue(IDX_INLAY) == 1));
			
			setTranslateX(getParameterNumValue(IDX_RADIUS) + 5.0);
			setTranslateY(getParameterNumValue(IDX_RADIUS) + 5.0);
			
			return spg.isOK();
		}
};

///////////////////////////////////////////////////////////////////////////
class PGenericPath : public PathGeneratorBase {
	
	private:
		unsigned int IDX_DATA;
		unsigned int IDX_REFPOINT;
	
	public:
		///////////////////////////////////////////////////////////////////
		PGenericPath() 
		: PathGeneratorBase()
		, IDX_DATA(INT_MAX)
		, IDX_REFPOINT(INT_MAX)
		{}
		
		virtual ~PGenericPath() {}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters()  {
			name 		= "Path";
			treePath 	= "Generic";
			
			commonValues.canPathColour = false;
			commonValues.canToolCorrection = false;
			commonValues.canToolDiameter = false;
			
			PathGeneratorBase::ParameterInfo pi;
			pi.setupString("Path d [px]", "M0,0 l20,20", true);
			IDX_DATA = setupParameter(pi); 
			IDX_REFPOINT = setupTLReferencePoint(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool setReferencePoint(SvgPathGroup& spg) {
			determineReferencePoint(spg, 0.0, 0.0);
			return true;
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool parameterChanging(PathGeneratorBase::ChangeCategory cc, int paraIdxInCat, const wxVariant& value) {
			// implement this method only if a parameter change should be rejected
			// clog << cc << ", " << paraIdxInCat << ", " << value.GetString() << endl;
			// return false if the the parameter change shoudn't apply 
			return true; 
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool parameterChanged(PathGeneratorBase::ChangeCategory cc, int paraIdxInCat, const wxVariant& value) { 
			// implement this method only if a parameter change notification should be handled
			// clog << cc << ", " << paraIdxInCat << ", " << value.GetString() << endl;
			// return false if the path shouldn't generated 
			return true; 
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) {
			
			// ...
			spg.pGen().addPath(getParameterStringValue(IDX_DATA));
			spg.add(spg.pGen().get());
			
			setTranslateX(40.0);
			setTranslateY(40.0);
			
			return spg.isOK();
		}
};

#endif
