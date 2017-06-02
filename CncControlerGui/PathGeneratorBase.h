#ifndef PATH_GENERATOR_BASE_H
#define PATH_GENERATOR_BASE_H

#include <map>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/propgrid/manager.h>
#include <wx/valnum.h>
#include "SvgUnitCalculator.h"
#include "SvgPathGroup.h"

#define DEFAULT_PARAMETER_VALUE_TYPE 		""
#define DEFAULT_PARAMETER_NUM_VALUE 		0.0
#define DEFAULT_PARAMETER_ENUM_VALUE		0
#define DEFAULT_PARAMETER_STRING_VALUE		_T("")

typedef std::map<wxString, wxString> CncParameterBlockMap;
class PathGeneratorBase {
	
	public:
		///////////////////////////////////////////////////////////////////
		struct ParameterInfo {
			wxString label			= "";
			wxString help			= "";
			wxString propertyType 	= DEFAULT_PARAMETER_VALUE_TYPE;
			
			wxVariant value;
			
			// num members
			double minRange			= 0;
			double maxRange			= 0;
			unsigned int precision	= 0;
			
			// string members
			bool withButton			= false;
			unsigned int maxLength	= -1;
			
			// enum members
			wxArrayString enumItems;
			
			///////////////////////////////////////////////////////////////
			int setupParameter(const wxString& index, const wxString& name, const wxString& type, const wxString& val) {
				long i; index.ToLong(&i);
				if ( i < 0 )
					return i;
					
				label			= name;
				propertyType 	= type;
				value 			= val;
				return i;
			}
			
			///////////////////////////////////////////////////////////////
			void setupNum(const wxString& l, double v, double min, double max, double prec) {
				propertyType = wxPG_VARIANT_TYPE_DOUBLE;
				
				label 		= l;
				value 		= v;
				minRange 	= min;
				maxRange 	= max;
				precision 	= prec;
				
				help.clear();
			}
			
			///////////////////////////////////////////////////////////////
			void setupString(const wxString& l, const wxString& v, bool wb = false, unsigned int ml = -1) {
				propertyType = wxPG_VARIANT_TYPE_STRING;
				
				label 		= l;
				value		= v;
				withButton	= wb;
				maxLength	= ml;
				
				help.clear();
			}
			
			///////////////////////////////////////////////////////////////
			void setupBool(const wxString& l, bool v) {
				propertyType = wxPG_VARIANT_TYPE_BOOL;
				
				label 		= l;
				value		= v;

				help.clear();
			}
			
			///////////////////////////////////////////////////////////////
			void setupEnum(const wxString& l, wxString items, long v) {
				propertyType = wxPG_VARIANT_TYPE_LIST;
				
				label 		= l;
				value		= v;
				
				enumItems.clear();
				wxStringTokenizer tokenizer(items, ";");
				while ( tokenizer.HasMoreTokens() ) {
					wxString token = tokenizer.GetNextToken();
					enumItems.Add(token);
				}

				help.clear();
			}
			
			///////////////////////////////////////////////////////////////
			void setupEnum(const wxString& l, wxArrayString items, long v) {
				propertyType = wxPG_VARIANT_TYPE_LIST;
				
				label 		= l;
				value		= v;
				enumItems	= items;

				help.clear();
			}
			
			///////////////////////////////////////////////////////////////
			const wxString& getValueAsString() {
				static wxString s;
				
				if ( propertyType == wxPG_VARIANT_TYPE_LIST ) {
					s = wxString::Format("%s", enumItems.Item(value.GetLong()));
				} else {
					s = value.GetString();
				}
				
				return s;
			}
		};
		
		typedef std::map<unsigned int, ParameterInfo> ParameterMap;
		
		///////////////////////////////////////////////////////////////////
		struct CommonValues {
			
			//todo remove this obsolete value
			bool toolCorrection = false;
			
			bool canToolCorrection 		= true;
			bool canJointType			= true;
			bool canToolDiameter		= true;
			bool canPathColour			= true;
			
			int toolCorrectionMode		= 0;
			int toolCorrectionCorners 	= 0;
			
			double toolDiameter 		= 3.125;
			wxColour pathColour 		= wxColour(0,0,0);
			
			bool configBlock			= true;
			bool referenceCross 		= true;
			bool xmlPattern				= true;
			
			///////////////////////////////////////////////////////////////
			void copyCanValues(const CommonValues& from) {
				canToolCorrection 	= from.canToolCorrection;
				canJointType		= from.canJointType;
				canToolDiameter		= from.canToolDiameter;
				canPathColour		= from.canPathColour;
			}
			
			///////////////////////////////////////////////////////////////
			void setCorrectionMode(const wxString& t) {
				if 		( t == "none" )		toolCorrectionMode = CncCT_None;
				else if ( t == "inner")		toolCorrectionMode = CncCT_Inner;
				else if ( t == "outer")		toolCorrectionMode = CncCT_Outer;
				else if ( t == "center")	toolCorrectionMode = CncCT_Center;
				else						toolCorrectionMode = CncCT_None;
			}
			
			///////////////////////////////////////////////////////////////
			CncToolCorretionType getCorrectionType() {
				switch ( toolCorrectionMode ) {
					case CncCT_None: 	return CncCT_None;
					case CncCT_Inner: 	return CncCT_Inner;
					case CncCT_Outer: 	return CncCT_Outer;
					case CncCT_Center:	return CncCT_Center;
				}
				return CncCT_None;
			}
			
			///////////////////////////////////////////////////////////////
			const char* getCorrectionTypeAsString() {
				switch ( toolCorrectionMode ) {
					case CncCT_None: 	return "none";
					case CncCT_Inner: 	return "inner";
					case CncCT_Outer: 	return "outer";
					case CncCT_Center: 	return "center";
				}
				return "none";
			}
			
			///////////////////////////////////////////////////////////////
			void setCornerType(const wxString& t) {
				if 		( t == "round" )	toolCorrectionCorners = CncCCT_Round;
				else if ( t == "square")	toolCorrectionCorners = CncCCT_Square;
				else if ( t == "miter")		toolCorrectionCorners = CncCCT_Miter;
				else						toolCorrectionCorners = CncCCT_Round;
			}

			///////////////////////////////////////////////////////////////
			CncClipperCornerType getCornerType() {
				switch ( toolCorrectionCorners ) {
					case CncCCT_Round: 	return CncCCT_Round;
					case CncCCT_Square:	return CncCCT_Square;
					case CncCCT_Miter: 	return CncCCT_Miter;
				}
				return CncCCT_Round;
			}
			 
			///////////////////////////////////////////////////////////////
			const char* getCornerTypeAsString() {
				switch ( toolCorrectionMode ) {
					case CncCCT_Round: 	return "round";
					case CncCCT_Square: return "square";
					case CncCCT_Miter: 	return "miter";
				}
				return "round";
			}
		};
		
		///////////////////////////////////////////////////////////////////
		struct TransformValues {
			bool autoMode			= true;
			
			double translateX		= 0.0;
			double translateY		= 0.0;
			
			double scaleX			= 1.0;
			double scaleY			= 1.0;
			
			double rotateA			= 0.0;
			double rotateX			= 0.0;
			double rotateY			= 0.0;
			
			double skewX			= 0.0;
			double skewY			= 0.0;
			
			///////////////////////////////////////////////////////////////
			void copyCanValues(const TransformValues& from) {
				// currently nothing to do
			}
		};
		
		///////////////////////////////////////////////////////////////////
		struct CncParameterValues  {
			bool canReverse			= true;
			bool canCorrect			= true;
			
			bool include			= true;
			
			int zDepthMode 			= 0;
			double zDepth			= 0.0;
			bool reverse 			= false;
			int correct 			= 0;
			
			///////////////////////////////////////////////////////////////
			void copyCanValues(const CncParameterValues& from) {
				canReverse			= from.canReverse;
				canCorrect			= from.canCorrect;
			}
			
			///////////////////////////////////////////////////////////////
			const char getModeAsChar() {
				if ( zDepthMode == 1 )
					return 'Z';
					
				return 'z';
			}
			
			///////////////////////////////////////////////////////////////
			const char* getCorrectAsString() {
				switch ( correct ) {
					case 0: return "none";
					case 1: return "inner";
					case 2: return "outer";
				}
				return "none";
			}
			
			///////////////////////////////////////////////////////////////
			void setCorrection(const wxString& c) {
				if 		( c == "inner" )	correct = 1;
				else if ( c == "outer")		correct = 2;
				else						correct = 0;
			}
			
			///////////////////////////////////////////////////////////////
			double getDepth() {
				if ( zDepthMode == 1 ) {
					return abs(zDepth);
				} else {
					if ( zDepth >= 0.0 )
						return -zDepth;
				}
					
				return zDepth;
			}
			
			///////////////////////////////////////////////////////////////
			void setDepth(const wxString& d) {
				if ( (char)d[0] == 'Z' )	zDepthMode = 1;
				else						zDepthMode = 0;
				
				wxString v = d.SubString(1, d.length() - 1);
				v.ToDouble(&zDepth);
			}
			
			///////////////////////////////////////////////////////////////
			void setReverse(const wxString& r) {
				reverse = (r == "yes" ? true : false ); 
			}
		};
		
		///////////////////////////////////////////////////////////////////
		struct XmlPatternResult {
			ParameterMap pMap;
			CommonValues commonValues;
			TransformValues transformValues;
			CncParameterValues cncParameterValues;
			
			wxString checkSum	= _T("");
			wxString tplName	= _T("");
			wxString errorInfo 	= _T("");
			
			///////////////////////////////////////////////////////////////
			void reset() {
				pMap.clear();
				checkSum.clear();
				errorInfo.clear();
				tplName.clear();
			}
			
			///////////////////////////////////////////////////////////////
			bool setToolDiameter(double d) {
				if ( d <= 0.0 )
					return false;
				
				commonValues.toolDiameter = d;
				return true;
			}
			
		};
		
		///////////////////////////////////////////////////////////////////
		PathGeneratorBase() 
		: errorInfo("")
		, xmlPattern("")
		, selectorIndex(-1)
		, inputUnit(mm)
		, name("")
		, treePath("")
		, centerPoint(DBL_MAX, DBL_MAX)
		{
		}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PathGeneratorBase() {
		}
		
		///////////////////////////////////////////////////////////////////
		enum ChangeCategory {CC_UNKNOWN_CAT, CC_COMMON_CAT, CC_GRID_CAT, CC_TPL_CAT, CC_CNC_CAT, CC_SVG_CAT};
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters() = 0;
		
		///////////////////////////////////////////////////////////////////
		virtual void getInternalInformation(wxTextCtrl* ctl) {
			wxASSERT(ctl);
				
			ctl->Clear();
			ctl->SetDefaultStyle(wxTextAttr(*wxWHITE));
			ctl->AppendText("No further information available . . .");
		}
		
		///////////////////////////////////////////////////////////////////
		virtual bool parameterChanging(PathGeneratorBase::ChangeCategory cc, int paraIdxInCat, const wxVariant& value) { return true; }
		
		///////////////////////////////////////////////////////////////////
		virtual bool parameterChanged(PathGeneratorBase::ChangeCategory cc, int paraIdxInCat, const wxVariant& value) { return true; }
		
		///////////////////////////////////////////////////////////////////
		void setSelectorIndex(int index) {
			selectorIndex = index;
		}
		
		///////////////////////////////////////////////////////////////////
		const int getSelectorIndex() const {
			return selectorIndex;
		}
		
		///////////////////////////////////////////////////////////////////
		long long getCheckSum() {
			long long ret = std::hash<std::string>{}((const char*)(getName().c_str()));
			
			wxASSERT(parameterMap.size());
			for (ParameterMap::reverse_iterator it=parameterMap.rbegin(); it!=parameterMap.rend(); ++it) {
				wxString s(it->second.propertyType);
				ret += (it->first * std::hash<std::string>{}((const char*)(s.c_str())));
			}
			
			return ret%LONG_LONG_MAX;
		}
		
		///////////////////////////////////////////////////////////////////
		wxString getCheckSumAsString() {
			static wxString s;
			s = wxString::Format("%llu", getCheckSum());
			return s;
		}
		
		///////////////////////////////////////////////////////////////////
		const wxString& generatePath();
		const wxString& generatePath(double toolDiameter, const ParameterMap& pMap);
		
		///////////////////////////////////////////////////////////////////
		wxXmlNode* evaluateCncPattern(double toolDiameter, const ParameterMap& pMap);
		
		///////////////////////////////////////////////////////////////////
		unsigned int getParameterCount() {
			return parameterMap.size();
		}
		
		///////////////////////////////////////////////////////////////////
		const wxString& getName() {
			return name;
		}
		
		///////////////////////////////////////////////////////////////////
		void resetErrorInfo() {
			errorInfo.Clear();
		}
		
		///////////////////////////////////////////////////////////////////
		bool hasErrorInfo() {
			return ( errorInfo.IsEmpty() == false );
		}
		
		///////////////////////////////////////////////////////////////////
		const wxString& getErrorInfo() {
			return errorInfo;
		}
		
		///////////////////////////////////////////////////////////////////
		const CommonValues& getCommonValues() {
			return commonValues;
		}
		
		///////////////////////////////////////////////////////////////////
		void setCommonValues(const CommonValues& cv) {
			commonValues = cv;
		}
		
		///////////////////////////////////////////////////////////////////
		const TransformValues& getTransformValues() {
			return transformValues;
		}
		
		///////////////////////////////////////////////////////////////////
		void setTransformValues(const TransformValues& tv) {
			transformValues = tv;
		}
		
		///////////////////////////////////////////////////////////////////
		const CncParameterValues& getCncParameterValues() {
			return cncParameterValues;
		}

		///////////////////////////////////////////////////////////////////
		void setCncParameterValues(const CncParameterValues& cv) {
			cncParameterValues = cv;
		}
		
		///////////////////////////////////////////////////////////////////
		void setParameterValue(unsigned int idx, const wxVariant v) {
			ParameterInfo* pi = getParameterInfo(idx);
			if ( pi != NULL )
				pi->value = v;
				
			addErrorInfo(wxString::Format("setParameterValue(): Invalid index: %u, value %s", idx, v.GetString()));
		}
		
		///////////////////////////////////////////////////////////////////
		void setParameterValue(unsigned int idx, double v) {
			ParameterInfo* pi = getParameterInfo(idx);
			if ( pi != NULL )
				pi->value = v;
				
			addErrorInfo(wxString::Format("setParameterValue(): Invalid index: %u, value %lf", idx, v));
		}
		
		///////////////////////////////////////////////////////////////////
		void setParameterValue(unsigned int idx, const wxString& v) {
			ParameterInfo* pi = getParameterInfo(idx);
			if ( pi != NULL )
				pi->value = v;
				
			addErrorInfo(wxString::Format("setParameterValue(): Invalid index: %u, value %s", idx, v));
		}
		
		///////////////////////////////////////////////////////////////////
		void setParameterValue(unsigned int idx, bool v) {
			ParameterInfo* pi = getParameterInfo(idx);
			if ( pi != NULL )
				pi->value = v;
				
			addErrorInfo(wxString::Format("setParameterValue(): Invalid index: %u, value %d", idx, v));
		}
		
		///////////////////////////////////////////////////////////////////
		void setParameterValue(unsigned int idx, long v) {
			ParameterInfo* pi = getParameterInfo(idx);
			if ( pi != NULL )
				pi->value = v;
				
			addErrorInfo(wxString::Format("setParameterValue(): Invalid index: %u, value %ld", idx, v));
		}
		
		///////////////////////////////////////////////////////////////////
		const char* getParameterType(unsigned int idx) {
			ParameterInfo* pi = getParameterInfo(idx);
			if ( pi != NULL ) {
				return pi->value.GetType();
			}
			
			addErrorInfo(wxString::Format("getParameterType(): Invalid index: %u", idx));
			return DEFAULT_PARAMETER_VALUE_TYPE;
		}
		
		///////////////////////////////////////////////////////////////////
		const wxVariant& getParameterValue(unsigned int idx) {
			ParameterInfo* pi = getParameterInfo(idx);
			if ( pi != NULL ) {
				return pi->value;
			}
			
			addErrorInfo(wxString::Format("getParameterType(): Invalid index: %u", idx));
			defaultValue = 0.0;
			return defaultValue;
		}
		
		///////////////////////////////////////////////////////////////////
		double getParameterNumValue(unsigned int idx) {
			ParameterInfo* pi = getParameterInfo(idx);
			if ( pi != NULL ) {
				if ( pi->propertyType == wxPG_VARIANT_TYPE_DOUBLE )
					return pi->value.GetDouble();
			}
			
			addErrorInfo(wxString::Format("getParameterType(): Invalid index: %u", idx));
			return DEFAULT_PARAMETER_NUM_VALUE;
		}
		
		///////////////////////////////////////////////////////////////////
		long getParameterEnumValue(unsigned int idx) {
			ParameterInfo* pi = getParameterInfo(idx);
			if ( pi != NULL ) {
				return pi->value.GetLong();
			}
			
			addErrorInfo(wxString::Format("getParameterType(): Invalid index: %u", idx));
			return DEFAULT_PARAMETER_ENUM_VALUE;
		}
		
		///////////////////////////////////////////////////////////////////
		const wxString getParameterStringValue(unsigned int idx) {
			ParameterInfo* pi = getParameterInfo(idx);
			if ( pi != NULL ) {
				return pi->value.GetString();
			}
			
			addErrorInfo(wxString::Format("getParameterType(): Invalid index: %u", idx));
			return DEFAULT_PARAMETER_STRING_VALUE;
		}
		
		///////////////////////////////////////////////////////////////////
		ParameterInfo* getParameterInfo(unsigned int idx) {
			ParameterMap::iterator it;
			it = parameterMap.find(idx);
			if ( it != parameterMap.end() )
				return &it->second;
				
			addErrorInfo(wxString::Format("getParameterType(): Invalid index: %u", idx));
			return NULL;
		}
		
		///////////////////////////////////////////////////////////////////
		bool isReferencePointDefined() {
			return ( centerPoint.x != DBL_MAX && centerPoint.y != DBL_MAX);
		}
		
		///////////////////////////////////////////////////////////////////
		const wxRealPoint& getReferencePoint() {
			return centerPoint;
		}
		
		///////////////////////////////////////////////////////////////////
		void clearParameters() {
			parameterMap.clear();
		}
		
		///////////////////////////////////////////////////////////////////
		static bool decodeXmlPattern(const wxString& pattern, XmlPatternResult& result);
		
		///////////////////////////////////////////////////////////////////
		static const wxString& maskXmlPattern(wxString& pattern);
		static const wxString& demaskXmlPattern(wxString& pattern);
		
		///////////////////////////////////////////////////////////////////
		const wxString& getTreePath() const {
			return treePath;
		}
		
	private:
	
		wxVariant defaultValue;
		wxString errorInfo;
		wxString xmlPattern;
		TransformValues transformValues;
		
		///////////////////////////////////////////////////////////////////
		const wxString& fillConfigBlock(wxString& fragment) {
			wxString cb;
			getConfigBlock(cb);
			
			fragment.Replace(SvgGeneratorBase::svgConfigPlaceholderBlock, cb, true);
			return fragment;
		}
		
		///////////////////////////////////////////////////////////////////
		void generatePathIntern(SvgPathGroup& spg);
		
		///////////////////////////////////////////////////////////////////
		const wxString& getConfigBlock(wxString& block);
		
	protected:
		
		int selectorIndex;
		SVGUnit inputUnit;
		wxString name;
		wxString treePath;
		wxRealPoint centerPoint;
		ParameterMap parameterMap;
		CommonValues commonValues;
		CncParameterValues cncParameterValues;
		
		///////////////////////////////////////////////////////////////////
		inline double cv(SVGUnit u, double v) {
			return SvgPathFragment::convertToDouble(u, v);
		}
		
		///////////////////////////////////////////////////////////////
		void setInputUnit(SVGUnit u) { inputUnit = u; }
		SVGUnit getInputUnit() const { return inputUnit; }
		
		///////////////////////////////////////////////////////////////////
		virtual bool generate(SvgPathGroup& spg, double toolDiameter) = 0;
		virtual bool setReferencePoint(SvgPathGroup& spg) = 0;
		
		///////////////////////////////////////////////////////////////////
		void addErrorInfo(const wxString& ei) {
			if ( ei.IsEmpty() == false ) {
				errorInfo << ei;
				errorInfo << "\n";
			}
		}
		
		///////////////////////////////////////////////////////////////////
		void setTranslateX(double val) { if ( transformValues.autoMode == true ) transformValues.translateX = val; }
		void setTranslateY(double val) { if ( transformValues.autoMode == true ) transformValues.translateY = val; }
		// todo ... more tranform values
		
		///////////////////////////////////////////////////////////////////
		unsigned int setupParameter(const ParameterInfo& pi) {
			parameterMap[parameterMap.size()] = pi;
			return parameterMap.size() - 1;
		}
		
		///////////////////////////////////////////////////////////////////
		void transform(SvgPathGroup& spg) {
			spg.translate(transformValues.translateX, transformValues.translateY);
			spg.scale(transformValues.scaleX, transformValues.scaleY);
			spg.rotate(transformValues.rotateA, transformValues.rotateX, transformValues.rotateY);
			spg.skewX(transformValues.skewX);
			spg.skewY(transformValues.skewY);
		}
		
		///////////////////////////////////////////////////////////////////
		void createCncParameterBlock(SvgPathGroup& spg);
		
		///////////////////////////////////////////////////////////////////
		const CncParameterBlockMap& getCncParameterBlock(CncParameterBlockMap& cpm);
		
		///////////////////////////////////////////////////////////////////
		void determineReferencePoint(SvgPathGroup& spg, const wxRealPoint& p) {
			determineReferencePoint(spg, p.x, p.y);
		}
		
		///////////////////////////////////////////////////////////////////
		void determineReferencePoint(SvgPathGroup& spg, double x, double y) {
			centerPoint.x = x;
			centerPoint.y = y;
			
			spg.setReferencePoint(centerPoint);
		}
		
		///////////////////////////////////////////////////////////////////
		unsigned int setupInlayMode(PathGeneratorBase::ParameterInfo& pi, const wxString& items, long value=0) {
			pi.setupEnum("Inlay Mode", items, value);
			pi.help = "Determines the fill mode.";
			return setupParameter(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		unsigned int setupCCReferencePoint(PathGeneratorBase::ParameterInfo& pi) {
			wxArrayString items;
			items.Add("center/center");
			return setupReferencePoint(pi, items, 0);
		}
		
		///////////////////////////////////////////////////////////////////
		unsigned int setupTLReferencePoint(PathGeneratorBase::ParameterInfo& pi) {
			wxArrayString items;
			items.Add("top/left");
			return setupReferencePoint(pi, items, 0);
		}
		
		///////////////////////////////////////////////////////////////////
		unsigned int setupReferencePoint(PathGeneratorBase::ParameterInfo& pi, const wxString& items, long value=0) {
			pi.setupEnum("Reference Point(x/y)", items, value);
			pi.help = "Determines the reference point as basis for further transformations.";
			return setupParameter(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		unsigned int setupReferencePoint(PathGeneratorBase::ParameterInfo& pi, const wxArrayString& items, long value=0) {
			pi.setupEnum("Reference Point(x/y)", items, value);
			pi.help = "Determines the reference point as basis for further transformations.";
			return setupParameter(pi);
		}
		
		///////////////////////////////////////////////////////////////////
		void encodeXmlPattern(const SvgPathGroup& spg);
		const wxString& getXmlPattern() { return xmlPattern; }
		
};

#endif