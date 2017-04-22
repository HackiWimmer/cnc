#ifndef PATH_GENERATOR_BASE_H
#define PATH_GENERATOR_BASE_H

#include <wx/string.h>
#include <wx/valnum.h>
#include "SvgPathGenerator.h"

#define DEFAULT_PARAMETER_VALUE  0.0;

class PathGenertorBase {
	
	public:
		///////////////////////////////////////////////////////////////////
		struct ParameterInfo {
			wxString label			= "";
			wxString help			= "";
			
			double minRange			= 0;
			double maxRange			= 0;
			unsigned int precision	= 0;
			
			double value			= DEFAULT_PARAMETER_VALUE;
			
			///////////////////////////////////////////////////////////////
			void setup(const wxString& l, double v, double min, double max, double prec) {
				label 		= l;
				value 		= v;
				minRange 	= min;
				maxRange 	= max;
				precision 	= prec;
			}
		};
		
		typedef std::map<unsigned int, ParameterInfo> ParameterMap;
		
		///////////////////////////////////////////////////////////////////
		struct CommonValues  {
			double toolDiameter = 0.0;
			wxColour pathColour = wxColour(0,0,0);
		};
		
		///////////////////////////////////////////////////////////////////
		struct TransformValues {
			double translateX		= 0.0;
			double translateY		= 0.0;
			
			double scaleX			= 1.0;
			double scaleY			= 1.0;
			
			double rotateA			= 0.0;
			double rotateX			= 0.0;
			double rotateY			= 0.0;
			
			double skewX			= 1.0;
			double skewY			= 1.0;
		};
		
		///////////////////////////////////////////////////////////////////
		PathGenertorBase() 
		: name("")
		{
		}
		
		///////////////////////////////////////////////////////////////////
		virtual ~PathGenertorBase() {
		}
		
		///////////////////////////////////////////////////////////////////
		virtual void initParameters() = 0;
		virtual const wxString& generate() = 0;
		
		///////////////////////////////////////////////////////////////////
		const wxString& getName() {
			return name;
		}
		
		///////////////////////////////////////////////////////////////////
		void setCommonValues(const CommonValues& cv) {
			commonValues = cv;
		}
		
		///////////////////////////////////////////////////////////////////
		void setTransformValues(const TransformValues& tv) {
			transformValues = tv;
		}
		
		///////////////////////////////////////////////////////////////////
		void setParameterValue(unsigned int idx, double v) {
			ParameterInfo* pi = getParameterInfo(idx);
			if ( pi != NULL )
				pi->value = v;
		}
		
		///////////////////////////////////////////////////////////////////
		double getParameterValue(unsigned int idx) {
			ParameterInfo* pi = getParameterInfo(idx);
			if ( pi != NULL )
				return pi->value;
			
			return DEFAULT_PARAMETER_VALUE;
		}
		
		///////////////////////////////////////////////////////////////////
		ParameterInfo* getParameterInfo(unsigned int idx) {
			ParameterMap::iterator it;
			it = parameterMap.find(idx);
			if ( it != parameterMap.end() )
				return &it->second;
				
			return NULL;
		}
		
	protected:
		wxString name;
		ParameterMap parameterMap;
		CommonValues commonValues;
		TransformValues transformValues;
		
		///////////////////////////////////////////////////////////////////
		void setupParameter(unsigned int idx, const ParameterInfo& pi) {
			parameterMap[idx] = pi;
		}
		
		///////////////////////////////////////////////////////////////////
		void transform(SvgPathGroup& spg) {
			spg.translate(transformValues.translateX, transformValues.translateY);
			spg.scale(transformValues.scaleX, transformValues.scaleY);
			spg.rotate(transformValues.rotateA, transformValues.rotateX, transformValues.rotateY);
			spg.skewX(transformValues.skewX);
			spg.skewY(transformValues.skewY);
		}
		
};

#endif