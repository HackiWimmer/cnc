#include "CncControl.h"
#include "CncFileNameService.h"
#include "SvgViewBox.h"
#include "SvgUnitCalculator.h"
#include "SerialEmulatorSVG.h"

const char* _useIdPrefix    	= "Use_";

const char* _scaleY 			= "%SCALE_Y%";
const char* _translateY 		= "%TRANSLATE_cY%";

///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::initClass() {
///////////////////////////////////////////////////////////////////
	fileFlags = std::ios::out | std::ios::trunc;
	writeOnlyMoveCommands = false;
}
///////////////////////////////////////////////////////////////////
const char* SerialEmulatorSVG::getSVGUnitAsStr() {
///////////////////////////////////////////////////////////////////
	return SvgUnitCalculator::getUnitAsStr(unit);
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::beginSVG(SVGUnit u, double w, double h, const wxString& vb) {
///////////////////////////////////////////////////////////////////
	unit = u;
	
	// determine the step factor with respect to the reference unit
	stepFactX = CncConfig::getGlobalCncConfig()->getDisplayFactX() * svgOutputParameters.zoomFactor * 1/SvgUnitCalculator::getFactorReferenceUnit2MM();
	stepFactY = CncConfig::getGlobalCncConfig()->getDisplayFactY() * svgOutputParameters.zoomFactor * 1/SvgUnitCalculator::getFactorReferenceUnit2MM();

	// always values from original svg, because only then the original path fits correctly
	width  		= w;
	height 		= h;
	viewBox 	= vb;
	pathCounter = 0;
	
	xyWatermarksAsPx.reset();
	originalPathUseInfo.clear();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::streamSvgHeader(std::fstream& fs) {
///////////////////////////////////////////////////////////////////
	SvgViewBox svgViewBox(viewBox);

	fs << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
	fs << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\"" << std::endl;
	fs << "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">" << std::endl;
	fs << "<svg xmlns=\"http://www.w3.org/2000/svg\" "; 
	fs << "width=\"" << width + width * 0.1 << getSVGUnitAsStr() << "\" "; 
	fs << "height=\"" << height + height * 0.1 << getSVGUnitAsStr() << "\" ";
	if ( viewBox != "" ) {
		fs << "viewBox=\"";
		if ( svgViewBox.isValid() ) {
			fs << svgViewBox.getX() << " ";
			fs << svgViewBox.getY() << " ";
			fs << svgViewBox.getW() + svgViewBox.getW() * 0.1 << " ";
			fs << svgViewBox.getH() + svgViewBox.getH() * 0.1;
		} else {
			fs << svgViewBox.getViewBoxStr();
		}
		fs << "\" ";
	}
	fs << "xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << std::endl;
	fs << "<title>CncControl SerialEmulatorSVG output</title>" << std::endl;
	fs << "<desc>Reconstructed move commands</desc>" << std::endl;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::streamToggleOrigPath(std::fstream& fs) {
///////////////////////////////////////////////////////////////////
	if ( svgOutputParameters.copyOrigPath == true ) {
		
		fs << "	function toggleOrigPath() {" << std::endl;
		fs << "		var elem, attr;" << std::endl;
		
		for ( OriginalPathUseInfo::iterator it = originalPathUseInfo.begin(); it != originalPathUseInfo.end(); ++it ) {
		
			fs << "		elem = document.getElementById('" << _useIdPrefix << it->first.c_str() << "'); " ;
			fs << "attr = elem.getAttribute('xlink:href');" << std::endl;
			
			fs << "		if ( attr.indexOf('#') == 0 )  elem.setAttribute('xlink:href', '" << it->first.c_str() << "'); ";
			fs << "else elem.setAttribute('xlink:href', '#" << it->first.c_str() << "');" << std::endl << std::endl;
		}
		
		fs << "	}" << std::endl;
	}
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::streamUseDirectives(std::fstream& fs) {
///////////////////////////////////////////////////////////////////
	for (unsigned int i=0; i<pathCounter; i++ ) {
		fs << "<use xlink:href=\"#Path_" << i + 1 << "_Crossing_1\"/>" << std::endl;
	}

	if ( svgOutputParameters.copyOrigPath == true ) {
		for ( OriginalPathUseInfo::iterator it = originalPathUseInfo.begin(); it != originalPathUseInfo.end(); ++it ) {
			fs << "<use ";
			fs << "id=\"" << _useIdPrefix << it->first.c_str() << "\"";
			fs << " xlink:href=\"#" << it->first.c_str() << "\"";
			if ( it->second != "" )
				fs << " transform=\"" << it->second.c_str() << "\"";
				
			fs << "/>" << std::endl;
		}
	}
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::streamSvgBody(std::fstream& fs) {
///////////////////////////////////////////////////////////////////
	fileStream.flush();
	
	std::fstream pfs;
	pfs.open(CncFileNameService::getCncOutboundTempFileName(), std::ios::in);
	if ( pfs.is_open() ) {
		std::string line;
		
		// modify with resepect to current coorinate system
		wxString rs("1"), rt("0");
		while ( getline (pfs, line) ) {
			wxString l(line);
			// replace placeholders
			l.Replace(_scaleY, rs, true);
			l.Replace(_translateY, 	rt, 	true);
			
			fs << l << '\n';
		}
		pfs.close();
	}
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::rebuildSVG() {
///////////////////////////////////////////////////////////////////
	closeSVG();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::closeSVG(CncDoublePosition::Watermarks& wm) {
///////////////////////////////////////////////////////////////////
	//  wm as a result of cnc processing always in unit mm
	xyWatermarksAsPx.xMin 			= 0; // currently not used
	xyWatermarksAsPx.yMin 			= 0; // currently not used
	
	xyWatermarksAsPx.xMax 			= SvgUnitCalculator::convertMM2ReferenceUnit(wm.xMax);
	xyWatermarksAsPx.yMax 			= SvgUnitCalculator::convertMM2ReferenceUnit(wm.yMax);
	
	return closeSVG();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::closeSVG() {
///////////////////////////////////////////////////////////////////
	if ( origFileName.size() == 0 )
		return;
	
	std::fstream fs;
	fs.open(origFileName.c_str(), fileFlags);
	if ( fs.is_open() == false ) {
		std::cerr << "SerialEmulatorSVG::closeSVG: Failed" << std::endl;
		std::cerr << "Can't open file: '" << origFileName.c_str() << "'" << std::endl;
		return;
	}
	
	streamSvgHeader(fs);
	fs << "<defs>" << std::endl;
	
		fs << "<script type=\"text/javascript\">" << std::endl;
		fs << "<![CDATA[" << std::endl;
		
			streamToggleOrigPath(fs);
			// stream more javascript functions here
		
		fs << "]]>" << std::endl;
		fs << "</script>" << std::endl;

	streamSvgBody(fs);

	fs << "</defs>" << std::endl;
	streamUseDirectives(fs);
	
	fs << "</svg>";
	fs << std::endl;
	fs.flush();
	fs.close();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::clearSVG() {
///////////////////////////////////////////////////////////////////
	//close filestream
	disconnect();
	
	// clear path file
	std::fstream pfs;
	pfs.open(CncFileNameService::getCncOutboundTempFileName(), std::ios::in | std::ios::out | std::ios::trunc);
	if ( pfs.is_open() ) {
		pfs.close();
	}
	
	// write empty svg file
	closeSVG();
	
	//reopen filestream
	connect(origFileName.c_str());
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::beginDuration(unsigned int count) {
///////////////////////////////////////////////////////////////////
	durationCount = count;
	if ( durationCount == 0 )
		pathCounter++;
		
	if ( durationCount >= maxDurationCount && svgOutputParameters.onlyFirstCrossing == true )
		return;

	fileStream << "<symbol id=\"Path_";
	fileStream << pathCounter;
	fileStream << "_Crossing_";
	fileStream << durationCount + 1;
	fileStream << "\">\n";
	fileStream.flush();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::closeDuration() {
///////////////////////////////////////////////////////////////////
	if ( durationCount >= maxDurationCount && svgOutputParameters.onlyFirstCrossing == true )
		return;
	
	fileStream << "</symbol>\n";
	fileStream.flush();
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::beginPath(double xStartPos, double yStartPos ) {
///////////////////////////////////////////////////////////////////
	if ( durationCount >= maxDurationCount && svgOutputParameters.onlyFirstCrossing == true )
		return;
		
	fileStream << "\t<path class=\"CncPath_" << pathCounter << "\"";
	fileStream << " d=\"M ";
	fileStream << xStartPos * svgOutputParameters.zoomFactor * 1/SvgUnitCalculator::getFactorReferenceUnit2MM();
	fileStream << " ";
	
	fileStream << yStartPos * svgOutputParameters.zoomFactor * 1/SvgUnitCalculator::getFactorReferenceUnit2MM(); 
	fileStream << " l"; // generally only relative move provided be the interface
	firstWriteMove = true;
	fileStream.flush();
	
	writeState = true;
	subPathState = false;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::beginSubPath(double xPos, double yPos) {
///////////////////////////////////////////////////////////////////
	if ( writeState == true )
		subPathState = true;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::closePath() {
///////////////////////////////////////////////////////////////////
	if ( durationCount >= maxDurationCount && svgOutputParameters.onlyFirstCrossing == true )
		return;
	
	// Add a default transform block to the cnc pathes to mirrow the Y axis on demand
	// See also streamSvgBody(...)
	fileStream << wxString::Format("\" transform=\"scale(1,%s) translate(0,%s)\"", _scaleY, _translateY);
	
	if ( svgOutputParameters.copyOrigPath == true )	cncPathStyle.dasharray = "2,1";
	else											cncPathStyle.dasharray = "";
	fileStream << cncPathStyle.getStyleAttr();
	
 	fileStream << std::endl;
	fileStream.flush();
	
	writeState = false;
	subPathState = false;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::writeOrigPath(const SvgOriginalPathInfo& sopi) {
///////////////////////////////////////////////////////////////////
	if ( svgOutputParameters.copyOrigPath == true ) {
		if ( durationCount == 0 ) {
			wxString path;
			wxString id("Orig_"); id << pathCounter;
			
			path << "<symbol id=\"";
			path << id.c_str();
			path << "\">\n";
			path << "\t<g";
			// Add a default transform block to the cnc pathes to mirrow the Y axis on demand
			// See also streamSvgBody(...)
			path << wxString::Format(" transform=\"scale(1,%s) translate(0,%s)\">\n", _scaleY, _translateY);
			path << "\t\t<path";
			path << " class=\"SvgPath_" << pathCounter << "\"";
			path << " d=\"";
			path << sopi.pathData.c_str();
			path << "\"";
			
			if ( sopi.transformInfo != "" ) {
				path << " transform=\"";
				path << sopi.transformInfo.c_str();
				path << "\"";
			}
			
			path << origPathStyle.getStyleAttr();
			path << "\n";
			path << "\t</g>\n";
			path << "</symbol>\n";
			
			originalPathUseInfo[id] = sopi.useTransformInfo;
			
			fileStream << path.c_str();
			fileStream.flush();
		}
	}
}
///////////////////////////////////////////////////////////////////
const char* SerialEmulatorSVG::getPortName() {
///////////////////////////////////////////////////////////////////
	return origFileName.c_str();
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorSVG::connect(const char* fileName) {
///////////////////////////////////////////////////////////////////
	origFileName = fileName;
	bool ret = SerialEmulatorFile::connect(CncFileNameService::getCncOutboundTempFileName());
	return ret;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::disconnect(void) {
///////////////////////////////////////////////////////////////////
	fileStream.flush();
	SerialEmulatorFile::disconnect();
}
///////////////////////////////////////////////////////////////////
bool SerialEmulatorSVG::writeMoveCmd(int32_t x , int32_t y , int32_t z, unsigned char *b, unsigned int nbByte) {
///////////////////////////////////////////////////////////////////
	if ( writeState == false )
		return true;
		
	if ( x == 0 && y == 0 && firstWriteMove == false ) // compensate first 'l'
		return true;
		
	if ( subPathState == true ) {
		if ( firstWriteMove == false )  fileStream << " m ";
		else							fileStream << " 0 0  m "; // compensate first 'l'
		subPathState = false;
	} 
	
	firstWriteMove = false;
		
	if      ( lastCommand.cmd == 'x' ) 	fileStream << " -"   << stepFactX << " 0";
	else if ( lastCommand.cmd == 'X' ) 	fileStream << " "    << stepFactX << " 0";
	else if ( lastCommand.cmd == 'y' ) 	fileStream << " 0 -" << stepFactY;
	else if ( lastCommand.cmd == 'Y' ) 	fileStream << " 0 "  << stepFactY;
	else if ( lastCommand.cmd == 'M'  || lastCommand.cmd == 'm' ) {
		fileStream << " " << x * stepFactX << " " << y * stepFactY;}
	
	return true;
}
///////////////////////////////////////////////////////////////////
void SerialEmulatorSVG::setSVGOutputParameters(const SvgOutputParameters& sp) {
///////////////////////////////////////////////////////////////////
	svgOutputParameters = sp;
	
	if (svgOutputParameters.onlyFirstCrossing == false ) 	maxDurationCount = INT_MAX;
	else													maxDurationCount = 1;
}
