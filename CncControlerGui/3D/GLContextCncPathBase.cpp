#include <iomanip>
#include <iostream>
#include "3D/GLContextCncPathBase.h"
#include "3D/GLLabelCluster.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncBoundarySpace.h"
#include "CncVector.h"
#include "CncAnchorInfo.h"
#include "CncBoundarySpace.h"
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
, cncPath					(getContextName())
, highlightedClientIds		()
, ruler						()
, drawType					(DT_LINE_STRIP)
, continiousDirConeFlag		(false)
, currentClientId			(-1L)
, processContext			()
, replayContext				()
, rulerColourX				(coordOriginInfo.colours.x)
, rulerColourY				(coordOriginInfo.colours.y)
, rulerColourZ				(coordOriginInfo.colours.z)
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
		
		// standard procedure
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
long GLContextCncPathBase::getPositionWithinBuffer(float x, float y, float z) {
/////////////////////////////////////////////////////////////////
	GLOpenGLPathBuffer::CncVertex vertex;
	const long p = cncPath.getOpenGLBufferStore()->findfirstVertexWithPos(x, y, z, vertex);
	
	if ( p >= 0 )	return p;
	else			return -1;
}
/////////////////////////////////////////////////////////////////
wxString GLContextCncPathBase::getNormalizedClientIdOfPos(float x, float y, float z) {
/////////////////////////////////////////////////////////////////
	GLOpenGLPathBuffer::CncVertex vertex;
	const long p = cncPath.getOpenGLBufferStore()->findfirstVertexWithPos(x, y, z, vertex);
	
	if ( p >= 0 ) {
		return _(wxString::Format("%ld", ClientIds::normalize(vertex.getClientId())));
	}
	
	return _("");
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::markCurrentPosition() {
/////////////////////////////////////////////////////////////////
	if ( cncPath.size() == 0 )
		return;
		
	if ( cncPath.getOpenGLBufferStore() == NULL )
		return;

	// get the last/current vertices - it must be valid
	GLOpenGLPathBuffer::CncVertex vertex;
	if ( cncPath.getOpenGLBufferStore()->getPosVertex(vertex, cncPath.getVirtualEnd() - 1) ) {
		drawMovePosition(vertex.getX(), vertex.getY(), vertex.getZ());
		
		if ( continiousDirConeFlag == true )
			keepVisible(vertex.getX(), vertex.getY(), vertex.getZ());
	}
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawDirectionCone() {
/////////////////////////////////////////////////////////////////
	if ( continiousDirConeFlag == false )
		return;

	if ( cncPath.getVirtualEnd() > 0 )
		drawDirectionCone(cncPath.getVirtualEnd() - 1);
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawDirectionCone(unsigned int idx) {
/////////////////////////////////////////////////////////////////
	static const float croneDiameter	= 0.0010f;
	static const float croneHight		= 0.0035f;
	
	static const CncFloatVector ux(1.0f, 0.0f, 0.0f);
	static const CncFloatVector uy(0.0f, 1.0f, 0.0f);
	static const CncFloatVector uz(0.0f, 0.0f, 1.0f);
	
	if ( cncPath.size() < 2 )
		return;
		
	if ( cncPath.getOpenGLBufferStore() == NULL )
		return;
	
	// evaluate vector at [idx] and [idx -1]
	GLOpenGLPathBuffer::CncVertex cv0, cv1;
	if ( cncPath.getOpenGLBufferStore()->getPosVertex(cv0, idx - 1 ) == false )
		return;
		
	if ( cncPath.getOpenGLBufferStore()->getPosVertex(cv1, idx) == false )
		return;
	
	// create position vectors
	const CncFloatVector p0(cv0.getX(), cv0.getY(), cv0.getZ());
	CncFloatVector p1(cv1.getX(), cv1.getY(), cv1.getZ());
	
	// evaluate direction vector
	CncFloatVector n(p1.sub(p0).normalize());
	
	// evaluate v and w as the both other parts of the new local coordinate system
	// together with n
	const CncFloatVector v(n.getNormale().normalize());
	const CncFloatVector w(v.getVectorProduct(n).normalize());
	
	// create the rotation matrix
	CncFloatMatrix4x4 rotMatrix;
	rotMatrix.set(CncFloatMatrix4x4::V1, v);
	rotMatrix.set(CncFloatMatrix4x4::V2, w);
	rotMatrix.set(CncFloatMatrix4x4::V3, n.mul(-1));
	
	//std::cout << n << std::endl;
	//std::cout << rotMatrix << std::endl;
	
	// translate, rotate and draw cone
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
		glColor4ub(128, 128, 128, 100);
		glTranslatef(cv1.getX(), cv1.getY(), cv1.getZ());
		glMultMatrixf(rotMatrix.get());
		
		drawSolidCone(croneDiameter, croneHight, 30, 30, false);
		
	glPopMatrix();
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawHighlightEffects() {
/////////////////////////////////////////////////////////////////
	for ( auto it = highlightedClientIds.begin(); it != highlightedClientIds.end(); ++it ) {
		const long clientID	= *it;
		const long pathPos	= cncPath.getOpenGLBufferStore()->findFirstEntryForClientId(clientID);
		
		if ( pathPos >= 0 )
			drawDirectionCone(pathPos);
	}
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawGuidePathes() {
/////////////////////////////////////////////////////////////////
	if ( options.showGuidePathes == false )
		return; 
	
	for ( auto it = guidePathes.begin(); it != guidePathes.end(); ++it ) 
	{
		const GLGuidePath& gp = *it;
		
		if ( gp.size() > 0 )
		{
			glMatrixMode(GL_MODELVIEW);
			
			bool stripple = false;
			switch ( gp.getStyle() ) {
				case wxPENSTYLE_DOT:
				{
					glLineStipple(1, 0xAAAA);
					glEnable(GL_LINE_STIPPLE);
					stripple = true;
					break;
				}
				case wxPENSTYLE_SHORT_DASH :
				{
					glLineStipple(2, 0xAAAA);
					glEnable(GL_LINE_STIPPLE);
					stripple = true;
					break;
				}
				case wxPENSTYLE_LONG_DASH  :
				{
					glLineStipple(4, 0xAAAA);
					glEnable(GL_LINE_STIPPLE);
					stripple = true;
					break;
				}
				default:
				{
					stripple = false;
					break;
				}
			}
			
			glColor4ub(gp.getColour().Red(), gp.getColour().Green(), gp.getColour().Blue(), gp.getColour().Alpha());
			glLineWidth(1);
			
			glBegin(GL_LINE_LOOP);
				
				for ( auto itGp = gp.begin(); itGp != gp.end(); ++itGp ) 
				{
					const CncDoublePosition& p1 = *itGp;
					glVertex3f(p1.getX(), p1.getY(), p1.getZ());
				}
				
			glEnd();
			if ( stripple == true )
				glDisable(GL_LINE_STIPPLE);
		}
	}
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawHardwareBox() {
/////////////////////////////////////////////////////////////////
	if ( options.showHardwareBox == false )
		return; 

	if ( THE_BOUNDS->getHardwareOffset().isValid() == false )
		return;
		
	// evaluate hardware origin as vertex as point
	const float originX		= THE_BOUNDS->getHardwareOffset().getAsStepsX() / THE_CONFIG->getDispFactX3D();
	const float originY		= THE_BOUNDS->getHardwareOffset().getAsStepsY() / THE_CONFIG->getDispFactY3D();
	const float originZ		= THE_BOUNDS->getHardwareOffset().getAsStepsZ() / THE_CONFIG->getDispFactZ3D();
	
	// evaluate hardware dimensions as distance converted to vertex scale
	const float maxDistX	= THE_BOUNDS->getMaxDimensionStepsX() / THE_CONFIG->getDispFactX3D();
	const float maxDistY	= THE_BOUNDS->getMaxDimensionStepsY() / THE_CONFIG->getDispFactY3D();
	const float maxDistZ    = THE_BOUNDS->getMaxDimensionStepsZ() / THE_CONFIG->getDispFactZ3D();

	// The Z origin has to be corrected from max to min because 
	// the hardware reference is located at min(x), min(y) and max(z)
	const float originZCorr	= originZ - maxDistZ;

	//-------------------------------------------------------------
	struct Point 
	{ 
		float x = 0.0; 
		float y = 0.0; 
		float z = 0.0; 
		
		void set(float a, float b, float c)
			{ x = a; y = b; z = c; }
		
	} p1, p2;

	// ensure the right model
	glMatrixMode(GL_MODELVIEW);
	
	// drawing the box
	glLineStipple(4, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);
	
		glColor4ub(options.hardwareBoxColour.Red(), options.hardwareBoxColour.Green(), options.hardwareBoxColour.Blue(), 255);
	
		//-------------------------------------------------------------
		auto drawLine = [&](const Point& p1, const Point& p2) 
		{
			glBegin(GL_LINES);
				glVertex3f(p1.x, p1.y, p1.z);
				glVertex3f(p2.x, p2.y, p2.z);
			glEnd();
			
			glBegin(GL_POINTS);
				glVertex3f(p1.x, p1.y, p1.z);
				glVertex3f(p2.x, p2.y, p2.z);
			glEnd();
		};
		
		// top rectangle
		p1.set(originX, originY, originZCorr); p2.set(originX, originY + maxDistY, originZCorr);
		drawLine(p1, p2);
		
		p1 = p2; p2.set(originX + maxDistX, originY + maxDistY, originZCorr);
		drawLine(p1, p2);
		
		p1 = p2; p2.set(originX + maxDistX, originY, originZCorr);
		drawLine(p1, p2);
		
		p1 = p2; p2.set(originX, originY, originZCorr);
		drawLine(p1, p2);
		
		// bottom rectangle
		p1.set(originX, originY, originZCorr + maxDistZ); p2.set(originX, originY + maxDistY, originZCorr + maxDistZ);
		drawLine(p1, p2);
		
		p1 = p2; p2.set(originX + maxDistX, originY + maxDistY, originZCorr + maxDistZ);
		drawLine(p1, p2);
		
		p1 = p2; p2.set(originX + maxDistX, originY, originZCorr + maxDistZ);
		drawLine(p1, p2);
		
		p1 = p2; p2.set(originX, originY, originZCorr + maxDistZ);
		drawLine(p1, p2);
		
		// connection lines (top to bottom)
		p1.set(originX, originY, originZCorr); p2.set(originX, originY, originZCorr + maxDistZ);
		drawLine(p1, p2);
		
		p1.set(originX, originY + maxDistY, originZCorr); p2.set(originX, originY + maxDistY, originZCorr + maxDistZ);
		drawLine(p1, p2);
		
		p1.set(originX + maxDistX, originY + maxDistY, originZCorr); p2.set(originX + maxDistX, originY + maxDistY, originZCorr + maxDistZ);
		drawLine(p1, p2);
		
		p1.set(originX + maxDistX, originY, originZCorr); p2.set(originX + maxDistX, originY, originZCorr + maxDistZ);
		drawLine(p1, p2);
	
	glDisable(GL_LINE_STIPPLE);
	
	// drawing the hardware reference flag
	glPushMatrix();
		
		const float croneDiameter	= 0.015f;
		const float croneHight		= 0.085f;
	
		float tx, ty, tz;
		const GLContextBase::ViewMode vm = getViewMode();
		switch ( vm ) 
		{
			case V2D_TOP:
			case V2D_BOTTOM:
			case V2D_LEFT:
			case V2D_RIGHT:
			case V2D_FRONT:
			case V2D_REAR:
			case V3D_ISO1:
			case V3D_ISO2: 
			case V3D_ISO3: 
			case V3D_ISO4:
			case V2D_CAM_ROT_XY_ZTOP:
			default:
				tx = originX + croneDiameter;
				ty = originY + croneDiameter; 
				tz = originZ + croneHight * 1.2;
		}
		
		renderBitmapString(tx, ty, tz, GLUT_BITMAP_8_BY_13, "HW Ref");
		
		glTranslatef(originX, originY, originZ + croneHight);
		
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		drawSilhouetteCone(croneDiameter, croneHight, 30, 30);
		
	glPopMatrix();
}
/////////////////////////////////////////////////////////////////
bool GLContextCncPathBase::getBounderies(CncDoubleBoundaries& ret) const {
/////////////////////////////////////////////////////////////////
	ret.reset();
	if ( cncPath.size() > 3 )
	{
		ret.setMinBound(CncDoublePosition(cncPath.getMin().getX(), cncPath.getMin().getY(), cncPath.getMin().getZ()));
		ret.setMaxBound(CncDoublePosition(cncPath.getMax().getX(), cncPath.getMax().getY(), cncPath.getMax().getZ()));
	}
	
	return ret.hasBoundaries();
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawTotalBoundBox() {
/////////////////////////////////////////////////////////////////
	if ( options.showTotalBoundBox == false )
		return; 
		
	// ensure the right model
	glMatrixMode(GL_MODELVIEW);
	typedef GLI::BoundBox BoundBox;
	
	glLineStipple(1, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);
	
	glColor4ub(options.totBoundBoxColour.Red(), options.totBoundBoxColour.Green(), options.totBoundBoxColour.Blue(), 255);
	
	BoundBox bb = cncPath.getBoundBox();
	for ( BoundBox::iterator it=bb.begin(); it!=bb.end(); ++it )
	{
		glBegin(GL_LINES);
			glVertex3f(std::get<0>(*it).getX(), std::get<0>(*it).getY(), std::get<0>(*it).getZ());
			glVertex3f(std::get<1>(*it).getX(), std::get<1>(*it).getY(), std::get<1>(*it).getZ());
		glEnd();
	}
	
	glDisable(GL_LINE_STIPPLE);
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawObjectBoundBox() {
/////////////////////////////////////////////////////////////////
	if ( options.showObjectBoundBox == false )
		return; 
		
	// ensure the right model
	glMatrixMode(GL_MODELVIEW);
	typedef GLI::BoundBox BoundBox;
	
	glLineStipple(3, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);
	
	glColor4ub(options.objBoundBoxColour.Red(), options.objBoundBoxColour.Green(), options.objBoundBoxColour.Blue(), 255);
	
	BoundBox bb = cncPath.getObjectBoundBox();
	for ( BoundBox::iterator it=bb.begin(); it!=bb.end(); ++it )
	{
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
	auto skipIndex = [&](int counter) 
	{
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
	auto drawRulerAxis = [&](const wxColour& colour, const GLI::GLLineCluster& ruler) 
	{
		glColor4ub(colour.Red(), colour.Green(), colour.Blue(), 64);
		
		for ( auto it = ruler.begin(); it != ruler.end(); ++it ) 
		{
			const GLI::GLVectiesTuple vt = *it;
			
			glBegin(GL_LINES);
				glVertex3f(vt.getX1(), vt.getY1(), vt.getZ1());
				glVertex3f(vt.getX2(), vt.getY2(), vt.getZ2());
			glEnd();
		}
	};
	
	//...........................................................
	auto drawHelpLines = [&](const wxColour& colour, const GLI::GLLineCluster& lines) 
	{
		glLineStipple(2, 0x00FF);
		glEnable(GL_LINE_STIPPLE);
		
			glColor4ub(colour.Red(), colour.Green(), colour.Blue(), 32);
			
			int counter = 1;
			for ( auto it = lines.begin(); it != lines.end(); ++it ) 
			{
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
	auto drawRulerLabels = [&](const wxColour& colour, const GLI::GLLabelCluster& lables) 
	{
		glColor4ub(colour.Red(), colour.Green(), colour.Blue(), 255);
		
		int counter = 0;
		for ( auto it = lables.begin(); it != lables.end(); it++ ) 
		{
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
	
	long entry = store->findLastEntryForClientId(currentClientId);
	
	if ( entry < 0 )
		entry = store->findFirstEntryForClientId(currentClientId);
	
	if ( entry < 0 )
		return;
	
	cncPath.setVirtualEnd(entry);
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::drawAnchorPoints() {
/////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->anchorMap == NULL )
		return;
	
	// ensure the right model
	glMatrixMode(GL_MODELVIEW);

	const double factorX( THE_CONFIG->getCalculationFactX() / THE_CONFIG->getDispFactX3D() );
	const double factorY( THE_CONFIG->getCalculationFactY() / THE_CONFIG->getDispFactY3D() );
	const double factorZ( THE_CONFIG->getCalculationFactZ() / THE_CONFIG->getDispFactZ3D() );
	
	const float offset	= 0.001;
	const int alpha		= 200;
	
	glLineStipple(2, 0x00FF);
	
	for ( auto it = THE_CONTEXT->anchorMap->cbegin(); it != THE_CONTEXT->anchorMap->cend(); ++it )
	{
		const CncAnchorInfo& ai = it->second;
		
		if ( ai.show == true ) 
		{
			// abs position can't shown i this case
			if ( ai.absolute && THE_BOUNDS->getHardwareOffset().isValid() == false )
				continue;
				
			const CncDoublePosition pos(ai.absolute ? THE_BOUNDS->getHardwareOffset().transPhy2Log(ai.pos) : ai.pos);
			
			const float x = pos.getX() * factorX;
			const float y = pos.getY() * factorY;
			const float z = pos.getZ() * factorZ;
			
			const bool  bx = ai.hasX();
			const bool  by = ai.hasY();
			const bool  bz = ai.hasZ();
			
			const float ox = bx ? offset : 10.0;
			const float oy = by ? offset : 10.0;
			const float oz = bz ? offset : 10.0;
			
			{
				if ( bx == false ) 
					glEnable(GL_LINE_STIPPLE);
				
				glColor4ub(coordOriginInfo.colours.x.Red(), coordOriginInfo.colours.x.Green(), coordOriginInfo.colours.x.Blue(), alpha * (bx ? 1.0 : 0.5) );
				glBegin(GL_LINES);
					glVertex3f(x - ox, y, z);
					glVertex3f(x + ox, y, z);
				glEnd();
					
				if ( bx == false ) 
					glDisable(GL_LINE_STIPPLE);
			}
			{
				if ( by == false ) 
					glEnable(GL_LINE_STIPPLE);
					
				glColor4ub(coordOriginInfo.colours.y.Red(), coordOriginInfo.colours.y.Green(), coordOriginInfo.colours.y.Blue(), alpha * (by ? 1.0 : 0.5) );
				glBegin(GL_LINES);
					glVertex3f(x, y - oy, z);
					glVertex3f(x, y + oy, z);
				glEnd();
				
				if ( by == false ) 
					glDisable(GL_LINE_STIPPLE);
			}
			{
				if ( bz == false ) 
					glEnable(GL_LINE_STIPPLE);
					
				glColor4ub(coordOriginInfo.colours.z.Red(), coordOriginInfo.colours.z.Green(), coordOriginInfo.colours.z.Blue(), alpha * (bz ? 1.0 : 0.5) );
				glBegin(GL_LINES);
					glVertex3f(x, y, z - oz);
					glVertex3f(x, y, z + oz);
				glEnd();
				
				if ( bz == false ) 
					glDisable(GL_LINE_STIPPLE);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::highlightClientId(long firstClientId, long lastClientId) { 
/////////////////////////////////////////////////////////////////
	if ( firstClientId < 0 && lastClientId < 0 )
		return;
	
	if ( firstClientId < 0 && lastClientId >= 0 )
		firstClientId = lastClientId;
		
	if ( firstClientId >= 0 && lastClientId < 0 )
		lastClientId = firstClientId;
	
	highlightedClientIds.clear();
	for (long id=firstClientId; id <= lastClientId; id++)
		highlightedClientIds.push_back(id);
		
	highlightClientIds(highlightedClientIds);
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::highlightClientIds(cnc::LongValues ids)  {
/////////////////////////////////////////////////////////////////
	if ( ids.size() == 0 )
		return;
		
	GLOpenGLPathBufferStore* store = cncPath.getOpenGLBufferStore();
	GLOpenGLPathBuffer::dimDownColours();
	GLOpenGLPathBuffer::ReconstructOptions opt;
	store->reconstruct(opt);
	
	dimDownGudePathes();
	
	cncPath.setVirtualEndToLast();
	for ( auto it = ids.begin(); it != ids.end(); ++it) {
		long & id = *it;
		
		if ( id >= 0 ) {
			store->dimUpClientID(id);
			highlightGudePathes(id);
		}
	}
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::highlightGudePathes(long cid) {
/////////////////////////////////////////////////////////////////
	for ( auto it = guidePathes.begin(); it != guidePathes.end(); ++it ) {
		GLGuidePath& gp = *it;
		
		if ( gp.hasClientId(cid) )
			gp.dimUp();
	}
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::dimUpGudePathes() {
/////////////////////////////////////////////////////////////////
	for ( auto it = guidePathes.begin(); it != guidePathes.end(); ++it ) {
		GLGuidePath& gp = *it;
		gp.dimUp();
	}
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::dimDownGudePathes() {
/////////////////////////////////////////////////////////////////
	for ( auto it = guidePathes.begin(); it != guidePathes.end(); ++it ) {
		GLGuidePath& gp = *it;
		gp.dimDown();
	}
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::pushProcessMode() {
/////////////////////////////////////////////////////////////////
	processContext.scaleFactor	= getCurrentScaleFactor();
	processContext.wndOrgin		= viewPort? wxPoint(viewPort->getCurrentOriginX(), viewPort->getCurrentOriginY()) : wxPoint(0, 0);

	setFrontCatchingMode(FCM_KEEP_IN_FRAME);
	continiousDirConeFlag = true;
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::popProcessMode() {
/////////////////////////////////////////////////////////////////
	continiousDirConeFlag = false;
	cncPath.activateNotifications();
	
	reshapeAbsolute(processContext.wndOrgin.x, processContext.wndOrgin.y);
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::pushInteractiveProcessMode() {
/////////////////////////////////////////////////////////////////
	setFrontCatchingMode(FCM_KEEP_IN_FRAME);
	continiousDirConeFlag = true;
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::popInteractiveProcessMode() {
/////////////////////////////////////////////////////////////////
	continiousDirConeFlag = false;
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::pushReplayMode() {
/////////////////////////////////////////////////////////////////
	replayContext.scaleFactor	= getCurrentScaleFactor();
	replayContext.wndOrgin		= viewPort? wxPoint(viewPort->getCurrentOriginX(), viewPort->getCurrentOriginY()) : wxPoint(0, 0);
	
	cncPath.deactivateNotifications();
	continiousDirConeFlag = true;
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::popReplayMode() {
/////////////////////////////////////////////////////////////////
	continiousDirConeFlag = false;
	cncPath.activateNotifications();
	
	reshapeAbsolute(replayContext.wndOrgin.x, replayContext.wndOrgin.y);
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::clearPathData() {
/////////////////////////////////////////////////////////////////
	cncPath.clear();
	highlightedClientIds.clear();
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
	highlightedClientIds.clear();
}
/////////////////////////////////////////////////////////////////
void GLContextCncPathBase::clearHighlightEffects() {
/////////////////////////////////////////////////////////////////
	GLOpenGLPathBuffer::ReconstructOptions opt;
	
	if ( highlightedClientIds.size() )
		reconstruct(opt);
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
	
	drawGuidePathes();
	drawRuler();
	
	switch ( drawType ) {
		case DT_POINTS:		cncPath.display(GLOpenGLPathBuffer::DT_DOTS);		break;
		case DT_LINE_STRIP:	cncPath.display(GLOpenGLPathBuffer::DT_STRIPS);		break;
	}
	
	drawDirectionCone();
	drawHighlightEffects();
	
	drawAnchorPoints();
	drawTotalBoundBox();
	drawObjectBoundBox();
	drawHardwareBox();
}
/////////////////////////////////////////////////////////////////
std::ostream& GLContextCncPathBase::traceInformation(std::ostream& o) const {
/////////////////////////////////////////////////////////////////
	GLContextBase::traceInformation(o);
	
	auto traceBounds = [&]()
	{
		if ( cncPath.size() > 2 )
		{
			return wxString::Format("(%.3lf, %.3lf)(%.3lf, %.3lf)(%.3lf, %.3lf)"
									, cncPath.getMin().getX(), cncPath.getMax().getX()
									, cncPath.getMin().getY(), cncPath.getMax().getY()
									, cncPath.getMin().getZ(), cncPath.getMax().getZ()
								   );
		}
		
		return wxString::Format("n/a");
	};
	
	o	<< " Cnc Path Vertex Count           : " << cncPath.size()			<< std::endl
		<< " Cnc Path Boundaries    [vertex] : " << traceBounds()			<< std::endl
	;

	return o;
}