#ifndef SVG_PATH_HANDLER_CNC_H
#define SVG_PATH_HANDLER_CNC_H

#include "PathHandlerBase.h"
#include "SerialPort.h"
#include "CncToolCorrection.h"
#include "SvgCncParameters.h"
#include "CncCommon.h"

class CncControl;

//////////////////////////////////////////////////////////////////
class SVGPathHandlerCnc : public PathHandlerBase {
//////////////////////////////////////////////////////////////////

	private:
	
		SVGUnit 			unit;
		CncControl* 		cncControl;
		SvgOriginalPathInfo origPathInfo;
		double 				toolRadius;
		bool 				zAxisDown;
		bool 				initialized;
		bool 				debugState;
		double 				width, height;
		wxString 			viewBox;
		SvgCncParameters currentCncParameters;
		
		// spool path to cnc control
		bool moveLinearXY(double x, double y, bool alreadyRendered);
		
		// path handling
		bool beginCurrentPath();
		bool repeatCurrentPath();
		bool spoolCurrentPath(bool firstRun);
		bool closeCurrentPath();
		
	protected:
	
		virtual bool shouldConvertRefToMM() const { return true; }
		
		// debug functions
		virtual void appendDebugValueDetail(const char* key, wxVariant value);
		virtual void appendDebugValueDetail(const CncPathListEntry& cpe);
		
		virtual bool isInitialized();
		
		// z axis management
		virtual void simulateZAxisUp();
		virtual void simulateZAxisDown();
		virtual bool isZAxisUp();
		virtual bool isZAxisDown();
		bool moveUpZ();
		bool moveDownZ();
		
	public:
		SVGPathHandlerCnc(CncControl* cnc);
		virtual ~SVGPathHandlerCnc();
		
		virtual void initNextClientId(long id);
		
		// Getter and setter
		void setMaxDimensions(SVGUnit u, double width, double height);
		void setViewBox(const wxString& vb);
		
		SVGUnit getSVGUnit() { return unit; }
		double getW() { return width; }
		double getH() { return height; }
		const char* getViewBox() { return viewBox.c_str(); }

		SvgCncParameters& getSvgCncParameters();
		void setCncWorkingParameters(SvgCncParameters& cwp);
		void setDebugState(bool state) { debugState = state; }
		
		// path handling
		virtual void prepareWork();
		virtual bool initNextPath();
		virtual bool initNextPath(const SvgOriginalPathInfo& sopi);
		virtual bool finishCurrentPath();
		virtual bool runCurrentPath();
		virtual void finishWork();
		
		virtual void logMeasurementStart();
		virtual void logMeasurementEnd();
};

#endif