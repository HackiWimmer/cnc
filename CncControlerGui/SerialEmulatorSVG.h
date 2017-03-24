#ifndef SERIAL_EMULATOR_SVG_CLASS
#define SERIAL_EMULATOR_SVG_CLASS

#include <string.h>
#include <map>
#include "SerialEmulatorFile.h"

typedef std::map<wxString, wxString> OriginalPathUseInfo;

////////////////////////////////////////////////////////////////////////////
struct SvgPathStrokeAttr {
	private:
		wxString ret;
		
	public:
		wxString color;
		wxString width;
		wxString fill;
		wxString dasharray;
		
		///////////////////////////////////////////////////////////////
		void initCncPath() {
			color 		= "black";
			fill 		= "none";
			width 		= "0.5";
			dasharray 	= "2,1";
		}
		
		///////////////////////////////////////////////////////////////
		void initOrigPath() {
			color 		= "red";
			fill 		= "none";
			width 		= "0.5";
			dasharray 	= "";
		}
		
		///////////////////////////////////////////////////////////////
		const char* getStyleAttr() {
			ret.clear();
			
			ret << " style=\"";
			if ( color != "" )		ret << "stroke:" << color << "; ";
			else 					ret << "stroke:black; ";
			
			if ( fill != "" )		ret << "fill:" << fill << "; "; 
			else					ret << "fill:none; ";
			
			if ( width != "" )		ret << "stroke-width:" << width << "px; ";
			else					ret << "stroke-width:1px; ";
			
			if ( dasharray != "")	ret << "stroke-dasharray:" << dasharray << "; ";
			
			ret << "\"/>";
			
			return ret.c_str();
		}
};

////////////////////////////////////////////////////////////////////////////
const double stepFactDefault = 0.01;
class SerialEmulatorSVG : public SerialEmulatorFile {
	protected:
		std::string origFileName;
		bool writeState;
		bool firstWriteMove;
		bool subPathState;
		
		unsigned int pathCounter;
		unsigned int durationCount;
		unsigned int maxDurationCount;
		
		SVGUnit unit;

		double width;
		double height;
		double stepFactX;
		double stepFactY;
		wxString viewBox;
		
		CncDoublePosition::Watermarks xyWatermarksAsPx;
		
		SvgPathStrokeAttr origPathStyle;
		SvgPathStrokeAttr cncPathStyle;

		SvgOutputParameters svgOutputParameters;
		
		OriginalPathUseInfo originalPathUseInfo;
		
		void initClass();
		const char* getSVGUnitAsStr();
		
		virtual bool writeSetter(void *buffer, unsigned int nbByte);
		virtual bool writeMoveCmd(int32_t x , int32_t y , int32_t z, void *buffer, unsigned int nbByte);
		
		// stream output 
		void streamSvgHeader(std::fstream& fs);
		void streamSvgBody(std::fstream& fs);
		void streamToggleOrigPath(std::fstream& fs);
		void streamUseDirectives(std::fstream& fs);
		
	public:
	
		//Initialize Serial communication without an acitiv connection 
		SerialEmulatorSVG(CncControl* cnc)
		: SerialEmulatorFile(cnc) 
		, writeState(false)
		, firstWriteMove(false)
		, subPathState(false)
		, pathCounter(0)
		, durationCount(0)
		, maxDurationCount(INT_MAX)
		, unit(px)
		, width(0.0)
		, height(0.0)
		, stepFactX(stepFactDefault)
		, stepFactY(stepFactDefault)
		, viewBox("")
		{
			initClass();
			origPathStyle.initOrigPath();
			cncPathStyle.initCncPath();
		}
		//Initialize Serial communication with the given COM port
		SerialEmulatorSVG(const char *fileName)
		: SerialEmulatorFile(fileName) 
		, writeState(false)
		, pathCounter(0)
		, durationCount(0)
		, maxDurationCount(INT_MAX)
		, unit(px)
		, width(0.0)
		, height(0.0)
		, stepFactX(stepFactDefault)
		, stepFactY(stepFactDefault)
		{
			initClass();
			origPathStyle.initOrigPath();
			cncPathStyle.initCncPath();
		}
		virtual ~SerialEmulatorSVG() {}
		
		// returns the class name
		virtual const char* getClassName() { return "SerialEmulatorSVG"; }
		// returns the emulator type
		virtual bool isEmulator() const { return true; }
		// return the port type
		virtual const CncPortType getPortType() const { return CncEMU_SVG; }
		// returns the port name
		virtual const char* getPortName();
		//Open file
		virtual bool connect(const char* fileName);
		//Close file
		virtual void disconnect(void);
		//SVG path handling
		virtual void setSVGOutputParameters(const SvgOutputParameters& sp);
		virtual void beginSVG(SVGUnit u, double width, double height, const wxString& viewBox = "");
		virtual void beginPath(double xStartPos, double yStartPos );
		virtual void beginSubPath(double xPos, double yPos);
		virtual void beginDuration(unsigned int count);
		virtual void closePath();
		virtual void closeDuration();
		virtual void closeSVG();
		virtual void closeSVG(CncDoublePosition::Watermarks& wm);
		virtual void clearSVG();
		virtual void rebuildSVG();
		virtual void writeOrigPath(const SvgOriginalPathInfo& sopi);
		
};

#endif