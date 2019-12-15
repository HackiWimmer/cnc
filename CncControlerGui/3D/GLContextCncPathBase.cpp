#include <iostream>
#include "3D/GLContextCncPathBase.h"
#include "3D/GLLabelCluster.h"
#include "CncConfig.h"
#include "3D/GLInclude.h"
#include "3D/CncGLCanvas.h"
#include <wx/bitmap.h>

CncGLContextObserver* CncGLContextObserver::theObserver = NULL;

///////////////////////////////////////////////////
CncGLContextObserver::CncGLContextObserver()
: currentContext(NULL)
, contextMap()
, contextIdx()
, callback(NULL)
///////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////
CncGLContextObserver::CncGLContextObserver(const CncGLContextObserver&) {
///////////////////////////////////////////////////
	abort();
}
///////////////////////////////////////////////////
CncGLContextObserver::~CncGLContextObserver() { 
///////////////////////////////////////////////////
	contextMap.clear();
	contextIdx.clear();
	CncGLContextObserver::theObserver = NULL; 
}
///////////////////////////////////////////////////
const wxString& CncGLContextObserver::getCurrentContextName() const { 
///////////////////////////////////////////////////
	static wxString ret;
	
	ret.assign(currentContext != NULL ? currentContext->getContextName() : ""); 
	return ret;
}
///////////////////////////////////////////////////
void CncGLContextObserver::appendMessage(const char type, const wxString& functName, const wxString& msg) const {
///////////////////////////////////////////////////
	if ( callback ) {
		const wxString ctxName(currentContext != NULL ? currentContext->getContextName() : "" ); 
		callback->nofifyMessage(type, ctxName, functName, msg);
	}
}
///////////////////////////////////////////////////
bool CncGLContextObserver::prepareContextSwitch(const GLContextCncPathBase* ctx) {
///////////////////////////////////////////////////
	if ( ctx == currentContext )
		return false;
	
	// deactivate previous context
	if ( currentContext != NULL )
		currentContext->deactivateOpenGlContext();
		
	return true;
}
///////////////////////////////////////////////////
void CncGLContextObserver::switchContext(const GLContextCncPathBase* ctx) {
///////////////////////////////////////////////////
	if ( ctx == NULL )
		return;
	
	GLContextCncPathBase* c = (GLContextCncPathBase*)ctx;
	if ( contextMap.find(c) == contextMap.end() ) {
		CncGLContextObserver::CtxInfo ctxInfo;
		contextMap[c] = ctxInfo;
		contextIdx.push_back(c);
		
		if ( callback )
			callback->nofifyForRegistered(c->getContextName());
	}
	
	if  ( currentContext != NULL )
		currentContext->deactivateOpenGlContext();
		
	currentContext = c;
	
	if ( callback ) {
		callback->nofifyForCurrent(currentContext->getContextName());
		callback->nofifyMessage('I', currentContext->getContextName(), CNC_LOG_FUNCT, getRegisteredContextItemText(currentContext));
	}
}
///////////////////////////////////////////////////
const wxString& CncGLContextObserver::getContextItemText(GLContextCncPathBase* ctx, long row, long column) const {
///////////////////////////////////////////////////
	static wxString retVal;
	
	auto assign = [&](long column, const char* key, const char* val) {
		if ( column == 0 )	retVal.assign(key != NULL ? key : "");
		else				retVal.assign(val != NULL ? val : "");
	};
	
	const CncGlCanvas* canvas = static_cast<const CncGlCanvas*>(ctx->getAssociatedCanvas());
	if ( canvas == NULL ) {
		std::cerr << CNC_LOG_FUNCT << ": Invalid Canavs pointer!" << std::endl;
		return retVal;
	}
	
	// Note: row count limited by getContextValueCount()
	switch ( ctx != NULL ? row : -42 ) {
		case  0:		assign(column, "Context Name", 		ctx->getContextName()); break;
		case  1: 		assign(column, "Context isValid", 	wxString::Format("%s", 					canvas->isContextValid()   ? "Yes" : "No" )); break;
		case  2:		assign(column, "Canvas Pointer", 	wxString::Format("%" PRIu64, 			(uint64_t)(ctx->getAssociatedCanvas())));     break;
		case  3: 		assign(column, "Canvas isShown",	wxString::Format("%s", 					canvas->IsShown()          ? "Yes" : "No" )); break;
		case  4: 		assign(column, "Canvas isShownOS",	wxString::Format("%s", 					canvas->IsShownOnScreen()  ? "Yes" : "No" )); break;
		case  5:		assign(column, "Current", 			wxString::Format("%s - (%" PRIu64 ")", 	ctx->isCurrent() ? "Yes" : "No", (uint64_t)(GLContextBase::getCurrentCanvas()))); break;
		
		case  7:		assign(column, "Store ID", 			ctx->cncPath.getOpenGLBufferStore()->getInstanceFullName()); break;
		case  8:		assign(column, "Vertex Count", 		wxString::Format("%lu", ctx->cncPath.getOpenGLBufferStore()->getVertexCount())); break;
		case  9:		assign(column, "Buffer Count", 		wxString::Format("%u",  ctx->cncPath.getOpenGLBufferStore()->getBufferCount())); break;
		case 10:		assign(column, "OpenGL Objects",	ctx->cncPath.getOpenGLBufferStore()->getVaoAndVboSummary()); break;
		
		case 11:		assign(column, "OpenGL State",		wxString::Format("%s", GLCommon::isGlAvailable()   ? "Valid" : "Invalid" )); break;
		case 12:		assign(column, "Glew State", 		wxString::Format("%s", GLCommon::isGlewAvailable() ? "Valid" : "Invalid" )); break;
		case 13:		assign(column, "Trace Level", 		wxString::Format("%d", GLCommon::getTraceLevel())); break;
		
		default:	retVal.clear();
	}
	
	return retVal;
}
///////////////////////////////////////////////////
const wxString& CncGLContextObserver::getCurrentContextItemText(long row, long column) const {
///////////////////////////////////////////////////
	return getContextItemText(currentContext, row, column);
}
///////////////////////////////////////////////////
const wxString& CncGLContextObserver::getRegisteredContextItemText(GLContextCncPathBase* ctx) const {
///////////////////////////////////////////////////
	for ( auto it=contextIdx.begin(); it != contextIdx.end(); ++it ) {
		
		if ( *it == ctx )
			return getRegisteredContextItemText(std::distance(contextIdx.begin(), it), 1);
		
	}
	
	return _("No context information available");
}
///////////////////////////////////////////////////
const wxString& CncGLContextObserver::getRegisteredContextItemText(long row, long column) const {
///////////////////////////////////////////////////
	static wxString retVal;
	retVal.clear();
	
	if ( row < 0 || row > (long)(contextIdx.size() - 1) )
		return retVal;
	
	GLContextCncPathBase* ctx = contextIdx.at(row);
	
	if ( column == 0 ) {
		retVal.assign(getContextItemText(ctx, 0, 1));
		
	} else {
		const unsigned int maxValues = getContextValueCount();
		for (unsigned int i=1; i<maxValues; i++) {
			
			const wxString keyname(getContextItemText(ctx, i, 0));
			const wxString parameter(getContextItemText(ctx, i, 1));
			if ( parameter.IsEmpty() == false ) {
				retVal.append(wxString::Format("%s=%s", keyname, parameter));
				
				if ( i < maxValues - 2 ) 
					retVal.append(", ");
			}
		}
	}
	
	return retVal;
}


/////////////////////////////////////////////////////////////////
GLContextCncPathBase::GLContextCncPathBase(wxGLCanvas* canvas, const wxString& name)
: GLContextBase(canvas, name)
, cncPath(getContextName())
, ruler()
, drawType(DT_LINE_STRIP)
, currentClientId(-1L)
, rulerColourX(coordOriginInfo.colours.x)
, rulerColourY(coordOriginInfo.colours.y)
, rulerColourZ(coordOriginInfo.colours.z)
/////////////////////////////////////////////////////////////////
{
}
/////////////////////////////////////////////////////////////////
GLContextCncPathBase::~GLContextCncPathBase() {
/////////////////////////////////////////////////////////////////
	clearPathData();
}
//////////////////////////////////////////////////
void GLContextCncPathBase::initBufferStore() {
//////////////////////////////////////////////////
	if ( cncPath.getOpenGLBufferStore()->isInitialized() == false ) {
		const bool state = cncPath.getOpenGLBufferStore()->initialize();
		
		if ( state == true )
			activateOpenGlContext();
	}
}
//////////////////////////////////////////////////
bool GLContextCncPathBase::SetCurrent(const wxGLCanvas &win) const {
//////////////////////////////////////////////////
	static CncGLContextObserver* ctxObs = CncGLContextObserver::getInstance();
	const bool isContextSwitch = ctxObs->prepareContextSwitch(this);
	
	bool ret = true;
	if ( isContextSwitch == true ) {
		
		// standard procedere
		ret = wxGLContext::SetCurrent(win);
		
		// log the canvas
		GLContextBase::setCurrentCanvas(&win);
		
		// switch
		ctxObs->switchContext(this);
	}
	return ret;
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::activateOpenGlContext(bool state) {
/////////////////////////////////////////////////////////////////
	if ( state == true ) 	cncPath.createVertexArray();
	else					cncPath.destroyVertexArray();
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::markCurrentPosition() {
/////////////////////////////////////////////////////////////////
	// ensure cncPath.end() - 1 is valid
	if ( cncPath.size() == 0 )
		return;
		
	// get the last/current vecties - it must be valid
	GLOpenGLPathBuffer::CncVertex vertex;
	
	if ( cncPath.getOpenGLBufferStore() != NULL )
		cncPath.getOpenGLBufferStore()->getVertex(vertex, cncPath.getVirtualEnd() - 1);
	
	drawPosMarker(vertex.getX(), vertex.getY(), vertex.getZ());
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::clearPathData() {
/////////////////////////////////////////////////////////////////
	cncPath.clear();
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::appendPathData(const GLOpenGLPathBuffer::CncVertex& vertex) {
/////////////////////////////////////////////////////////////////
	cncPath.appendPathData(vertex);
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::reconstruct(const GLOpenGLPathBuffer::ReconstructOptions& opt) {
/////////////////////////////////////////////////////////////////
	cncPath.reconstruct(opt);
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::determineModel() {
/////////////////////////////////////////////////////////////////
	if ( isEnabled() == false ) {
		
		
		//std::cout  << " GLContextCncPathBase::determineModel(); false"<< std::endl;
		
		//drawTeapot();
		
		/*
		glPushMatrix();
			glEnable(GL_TEXTURE_2D);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glBindTexture(GL_TEXTURE_2D, theTexture);
				drawBox(0.05, GL_QUADS);
			glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		*/
		return;
	}
	
	
	drawRuler();
	
	switch ( drawType ) {
		case DT_POINTS:		cncPath.display(GLOpenGLPathBuffer::DT_DOTS); 		break;
		case DT_LINE_STRIP:	cncPath.display(GLOpenGLPathBuffer::DT_STRIPS); 	break;
	}
	
	drawBoundBox();
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawBoundBox() {
/////////////////////////////////////////////////////////////////
	if ( options.showBoundBox == false )
		return; 
		
	// ensure the right model
	glMatrixMode(GL_MODELVIEW);
	typedef GLI::BoundBox BoundBox;
	
	glLineStipple(1, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);
	
	glColor4ub(options.boundBoxColour.Red(), options.boundBoxColour.Green(), options.boundBoxColour.Blue(), 255);
	
	BoundBox bb = cncPath.getBoundBox();
	for ( BoundBox::iterator it=bb.begin(); it!=bb.end(); ++it ) {
		
		glBegin(GL_LINES);
			glVertex3f(std::get<0>(*it).getX(), std::get<0>(*it).getY(), std::get<0>(*it).getZ());
			glVertex3f(std::get<1>(*it).getX(), std::get<1>(*it).getY(), std::get<1>(*it).getZ());
		glEnd();
	}
	
	glDisable(GL_LINE_STIPPLE);
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawRuler() {
/////////////////////////////////////////////////////////////////
	//...........................................................
	auto skipIndex = [&](int counter) {
		const float sf = this->getCurrentScaleFactor();
		
		// the mod operands below 'm u s t' be even - always
		// otherwise the value of '0' isn't reached
		if      ( sf >= 0.00 && sf < 0.05 ) 	{ if ( counter % ( 100 ) != 0 ) return true; }
		if      ( sf >= 0.05 && sf < 0.10 ) 	{ if ( counter % (  50 ) != 0 ) return true; }
		else if ( sf >= 0.10 && sf < 0.25 ) 	{ if ( counter % (  10 ) != 0 ) return true; }
		else if ( sf >= 0.25 && sf < 0.50 ) 	{ if ( counter % (   4 ) != 0 ) return true; }
		else if ( sf >= 0.50 && sf < 1.00 ) 	{ if ( counter % (   2 ) != 0 ) return true; }
		
		return false;
	};
	
	//...........................................................
	auto drawRulerAxis = [&](const wxColour& colour, const GLI::GLLineCluster& ruler) {
		
		glColor4ub(colour.Red(), colour.Green(), colour.Blue(), 64);
		
		for ( auto it = ruler.begin(); it != ruler.end(); ++it ) {
			const GLI::GLVectiesTuple vt = *it;
			
			glBegin(GL_LINES);
				glVertex3f(vt.getX1(), vt.getY1(), vt.getZ1());
				glVertex3f(vt.getX2(), vt.getY2(), vt.getZ2());
			glEnd();
		}
	};
	
	//...........................................................
	auto drawHelpLines = [&](const wxColour& colour, const GLI::GLLineCluster& lines) {
		
		glLineStipple(2, 0x00FF);
		glEnable(GL_LINE_STIPPLE);
		
			glColor4ub(colour.Red(), colour.Green(), colour.Blue(), 32);
			
			int counter = 1;
			for ( auto it = lines.begin(); it != lines.end(); ++it ) {
				
				if ( skipIndex( counter++ ) == true ) 
					continue;
				
				glBegin(GL_LINES);
					const GLI::GLVectiesTuple& vt = *it;
					glVertex3f(vt.getX1(), vt.getY1(), vt.getZ1());
					glVertex3f(vt.getX2(), vt.getY2(), vt.getZ2());
				glEnd();
			}
		
		glDisable(GL_LINE_STIPPLE);
	};

	//...........................................................
	auto drawRulerLabels = [&](const wxColour& colour, const GLI::GLLabelCluster& lables) {
		
		glColor4ub(colour.Red(), colour.Green(), colour.Blue(), 255);
		
		int counter = 0;
		for ( auto it = lables.begin(); it != lables.end(); it++ ) {
			
			if ( skipIndex( counter++ ) == true ) 
				continue;
			
			const GLI::GLLabelInfo& li = *it;
			void* font = li.font;
			if ( font == NULL )
				font = GLUT_BITMAP_8_BY_13;
			
			this->renderBitmapString(li.x, li.y, li.z, font, li.label);
		}
	};
	
	// ensure the right model
	glMatrixMode(GL_MODELVIEW);
	
	// first deterine what's needed
	const short X  = 0, Y  = 1, Z  = 2, 
	            XY = 0, XZ = 1, YZ = 2;
				
	bool showAxis[3]; 	showAxis[X]   = false;  showAxis[Y]   = false;  showAxis[Z]   = false;
	bool showPlane[3];	showPlane[XY] = false;  showPlane[XZ] = false;  showPlane[YZ] = false;
	
	const GLContextBase::ViewMode vm = getViewMode();
	switch ( vm ) {
		case V2D_TOP:
		case V2D_BOTTOM:			showAxis[X]   = true;  showAxis[Y]   = true;  showAxis[Z]   = false;
									showPlane[XY] = true;  showPlane[XZ] = false; showPlane[YZ] = false;
									break;
		case V2D_LEFT:
		case V2D_RIGHT: 			showAxis[X]   = false; showAxis[Y]   = true;  showAxis[Z]   = true;
									showPlane[XY] = false; showPlane[XZ] = false; showPlane[YZ] = true;
									break;
		case V2D_FRONT:
		case V2D_REAR:				showAxis[X]   = true;  showAxis[Y]   = false; showAxis[Z]   = true;
									showPlane[XY] = false; showPlane[XZ] = true;  showPlane[YZ] = false;
									break;
		case V3D_ISO1:
		case V3D_ISO2: 
		case V3D_ISO3: 
		case V3D_ISO4:
		case V2D_CAM_ROT_XY_ZTOP: 	showAxis[X]   = true;  showAxis[Y]   = true;  showAxis[Z]   = true;
									showPlane[XY] = options.helpLines3D_XYPlane;  
									showPlane[XZ] = options.helpLines3D_XZPlane;
									showPlane[YZ] = options.helpLines3D_YZPlane;
									break;
	}
	
	// draw the rulers
	if ( options.showRuler == true ) {
		if ( showAxis[X] ) { drawRulerAxis(rulerColourX, ruler.xAxis.axisLines); }
		if ( showAxis[Y] ) { drawRulerAxis(rulerColourY, ruler.yAxis.axisLines); }
		if ( showAxis[Z] ) { drawRulerAxis(rulerColourZ, ruler.zAxis.axisLines); }
		
		if ( showAxis[X] ) { drawRulerLabels(rulerColourX, ruler.xAxis.axisLables); }
		if ( showAxis[Y] ) { drawRulerLabels(rulerColourY, ruler.yAxis.axisLables); }
		if ( showAxis[Z] ) { drawRulerLabels(rulerColourZ, ruler.zAxis.axisLables); }
	}
	
	if ( options.showHelpLines == true ) {
		wxColour colour(255, 255, 255);
		if ( showPlane[XY] ) { drawHelpLines(colour, ruler.helpLinesXY.helpLinesX); drawHelpLines(colour, ruler.helpLinesXY.helpLinesY); }
		if ( showPlane[XZ] ) { drawHelpLines(colour, ruler.helpLinesXZ.helpLinesX); drawHelpLines(colour, ruler.helpLinesXZ.helpLinesZ); }
		if ( showPlane[YZ] ) { drawHelpLines(colour, ruler.helpLinesYZ.helpLinesY); drawHelpLines(colour, ruler.helpLinesYZ.helpLinesZ); }
	}
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::setCurrentClientId(long id) { 
/////////////////////////////////////////////////////////////////
	currentClientId = id; 
	
	if ( currentClientId < 0 )
		return;

	GLOpenGLPathBufferStore* store = cncPath.getOpenGLBufferStore();
	const long entry = store->findFirstEntryForClientId(currentClientId);

	if ( entry < 0 )
		return;

	cncPath.setVirtualEnd(entry);
}
