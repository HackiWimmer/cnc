#ifndef PGEN_SAMPLE_H
#define PGEN_SAMPLE_H

#include "PathGeneratorBase.h"

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

#endif