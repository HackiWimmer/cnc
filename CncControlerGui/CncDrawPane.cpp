#include <iostream>

#include <wx/dcclient.h>
#include <wx/app.h> 

#ifdef USE_WXCRAFTER
	#include "wxcrafter.h"
#endif

#ifdef __DARWIN__
    #include <OpenGL/glu.h>
#else
    #include <GL/glu.h>
#endif

#include "trackball.h"
#include "CncCommon.h"
#include "CncDrawPane.h"

CncOpenGLDrawPaneContext* CncOpenGLDrawPane::globalContext = NULL;

// ----------------------------------------------------------------------------
// CncOpenGLDrawPane Event Table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(CncOpenGLDrawPane, wxGLCanvas)
    EVT_PAINT(CncOpenGLDrawPane::OnPaint)
    EVT_MOUSE_EVENTS(CncOpenGLDrawPane::OnMouse)
    EVT_SIZE(CncOpenGLDrawPane::OnSize)
	EVT_ERASE_BACKGROUND(CncOpenGLDrawPane::OnEraseBackground)
    EVT_KEY_DOWN(CncOpenGLDrawPane::OnKeyDown)
    EVT_TIMER(SpinTimer, CncOpenGLDrawPane::OnSpinTimer)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////
CncOpenGLDrawPane::CncOpenGLDrawPane(wxWindow *parent, int *attribList)
/////////////////////////////////////////////////////////////////////
// With perspective OpenGL graphics, the wxFULL_REPAINT_ON_RESIZE style
// flag should always be set, because even making the canvas smaller should
// be followed by a paint event that updates the entire canvas with new
// viewport settings.
: wxGLCanvas(parent, wxID_ANY, attribList, wxDefaultPosition, wxDefaultSize, 
	         wxFULL_REPAINT_ON_RESIZE)
, spinTimerInterval(DEFAULT_SPIN_TIMER_INTERVAL)
, scaleX(1.0f)
, scaleY(1.0f)
, scaleZ(1.0f)
, viewPortX(0)
, viewPortY(0)
, spinTimer(this,SpinTimer)
, displayAngels(0.0, 0.0, 0.0, -120.0, 10.0, 1.0)
, playButton(NULL)
, traceCtrl(NULL)
, spinAngleX(NULL)
, spinAngleY(NULL)
, spinAngleZ(NULL)
{
	trackball(globalData.quat, 0.0f, 0.0f, 0.0f, 0.0f);
	
	// select a inital view, this also initializes viewPort*
	view3D();
}
/////////////////////////////////////////////////////////////////////
CncOpenGLDrawPane::~CncOpenGLDrawPane() {
/////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::trace(const wxString& msg) {
/////////////////////////////////////////////////////////////////////
	if ( traceCtrl != NULL ) {
		traceCtrl->ChangeValue(msg);
	} else {
		std::cout << msg << endl;
	}
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::runOpenGLTest() {
/////////////////////////////////////////////////////////////////////
	DrawPaneData& dpd = clearDataVector();
	DoublePointPair3D pp;
	
	double vertices[8][3] = {
		{ 1.0f, 1.0f, 1.0f}, //0
		{ 1.0f, 1.0f,-1.0f}, //1
		{ 1.0f,-1.0f,-1.0f}, //2
		{-1.0f,-1.0f,-1.0f}, //3
		{-1.0f,-1.0f, 1.0f}, //4
		{-1.0f, 1.0f, 1.0f}, //5
		{-1.0f, 1.0f,-1.0f}, //6
		{ 1.0f,-1.0f, 1.0f}  //7
	};

	for ( int i=0; i<8; i+=1 ) {
		for (int j=0; j<8; j+=1) {
			
			dpd.push_back(pp.set(vertices[i][0] / 10, vertices[i][1] / 10, vertices[i][2] / 10,
								 vertices[j][0] / 10, vertices[j][1] / 10, vertices[j][2] / 10));
		}
	}
	
	Refresh();
	view3D();
	
	startSpinTimer();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::CheckGLError() {
/////////////////////////////////////////////////////////////////////
	GLenum errLast = GL_NO_ERROR;

	for ( ;; )
	{
		GLenum err = glGetError();
		if ( err == GL_NO_ERROR )
			return;

		// normally the error is reset by the call to glGetError() but if
		// glGetError() itself returns an error, we risk looping forever here
		// so check that we get a different error than the last time
		if ( err == errLast )
		{
			wxLogError(wxT("OpenGL error state couldn't be reset."));
			return;
		}

		errLast = err;

		wxLogError(wxT("OpenGL error %d"), err);
	}
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::clear3D() {
/////////////////////////////////////////////////////////////////////
	clearDataVector();
	stopSpinTimer();
	
	if ( IsShown() ) {
		Refresh();
		view3D();
	}
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::animate3D() {
/////////////////////////////////////////////////////////////////////
	switchSpinTimer();
}
/////////////////////////////////////////////////////////////////////
CncOpenGLDrawPaneContext& CncOpenGLDrawPane::initGlobalContext(wxGLCanvas *canvas) {
/////////////////////////////////////////////////////////////////////
	wxASSERT( canvas != NULL );

	// Init the context with the first canvas
	if ( CncOpenGLDrawPane::globalContext == NULL )
		CncOpenGLDrawPane::globalContext = new CncOpenGLDrawPaneContext(canvas);

	globalContext->SetCurrent(*canvas);
	return *globalContext;
}
/////////////////////////////////////////////////////////////////////
DrawPaneData& CncOpenGLDrawPane::getDataVector() {
/////////////////////////////////////////////////////////////////////
	return data;
}
/////////////////////////////////////////////////////////////////////
DrawPaneData& CncOpenGLDrawPane::clearDataVector() {
/////////////////////////////////////////////////////////////////////
	data.clear();
	return data;
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::displayDataVector() {
/////////////////////////////////////////////////////////////////////
	//Releaes the repaining
	Refresh();
	Update();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::determineDisplayAngles(float ax, float ay, float az) {
/////////////////////////////////////////////////////////////////////
	displayAngels.setX(ax);
	displayAngels.setY(ay);
	displayAngels.setZ(az);
	
	displayAngels.setDefaultX(ax);
	displayAngels.setDefaultY(ay);
	displayAngels.setDefaultZ(az);
	
	// initial rotation 
	rotate();   
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::rotate() {
/////////////////////////////////////////////////////////////////////
	glRotatef(displayAngels.getX(), 1.0f, 0.0f, 0.0f);
	glRotatef(displayAngels.getY(), 0.0f, 1.0f, 0.0f);
	glRotatef(displayAngels.getZ(), 0.0f, 0.0f, 1.0f);
	
	if ( spinAngleX != NULL )
		spinAngleX->SetValue(displayAngels.getX());
		
	if ( spinAngleY != NULL )
		spinAngleY->SetValue(displayAngels.getY());
		
	if ( spinAngleZ != NULL )
		spinAngleZ->SetValue(displayAngels.getZ());
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::initializeOpenGL() {
/////////////////////////////////////////////////////////////////////
	static const GLfloat light0_pos[4]   = { -50.0f, 50.0f, 0.0f, 0.0f };

	// white light
	static const GLfloat light0_color[4] = { 0.6f, 0.6f, 0.6f, 1.0f };

	// ????
	static const GLfloat light1_pos[4]   = {  50.0f, 50.0f, 0.0f, 0.0f };

	// cold blue light
	static const GLfloat light1_color[4] = { 0.4f, 0.4f, 1.0f, 1.0f };

	// remove back faces 
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// speedups
	glEnable(GL_DITHER);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

	// light
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_color);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_color);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::resetProjectionMode() {
/////////////////////////////////////////////////////////////////////
	if ( !IsShownOnScreen() )
		return;

	// This is normally only necessary if there is more than one wxGLCanvas
	// or more than one wxGLContext in the application.
	SetCurrent(*globalContext);

	int w, h;
	GetClientSize(&w, &h);

	// It's up to the application code to update the OpenGL viewport settings.
	// In order to avoid extensive context switching, consider doing this in
	// OnPaint() rather than here, though.
	glViewport(viewPortX, viewPortY, (GLint) w, (GLint) h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(20.0f, (GLfloat)w/h, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::spinPaneFromKeyboard(float xSpin, float ySpin) {
/////////////////////////////////////////////////////////////////////
	displayAngels.incX(xSpin);
	displayAngels.incY(ySpin);
	//displayAngels.incZ(zSpin);

	Refresh();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
/////////////////////////////////////////////////////////////////////
	// Do nothing, to avoid flashing on MSW
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::OnSize(wxSizeEvent& event) {
/////////////////////////////////////////////////////////////////////
	// Reset the OpenGL view aspect.
	// This is OK only because there is only one canvas that uses the context.
	// See the cube sample for that case that multiple canvases are made current with one context.
	resetProjectionMode();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::OnPaint(wxPaintEvent& WXUNUSED(event)) {
/////////////////////////////////////////////////////////////////////
	// This is required even though dc is not used otherwise.
	wxPaintDC dc(this);

	// Set the OpenGL viewport according to the client size of this canvas.
	// This is done here rather than in a wxSizeEvent handler because our
	// OpenGL rendering context (and thus viewport setting) is used with
	// multiple canvases: If we updated the viewport in the wxSizeEvent
	// handler, changing the size of one canvas causes a viewport setting that
	// is wrong when next another canvas is repainted.
	const wxSize ClientSize = GetClientSize();
	glViewport(viewPortX, viewPortY, ClientSize.x, ClientSize.y);

	CncOpenGLDrawPaneContext& dpc = CncOpenGLDrawPane::initGlobalContext(this);
	SetCurrent(*globalContext);
		
	// Initialize the global OpenGl data with the first call of OnPaint
	if ( globalData.isInitialized() == false ) {
		initializeOpenGL();
		resetProjectionMode();
		globalData.initialize();
	}   

	// Transformations
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, 0.0f);

	GLfloat m[4][4];
	build_rotmatrix(m, globalData.quat);
	glMultMatrixf( &m[0][0] );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -2.0f);
	glScalef(scaleX, scaleY, scaleZ);

	rotate();
	dpc.displayDataVector(data);
		
	SwapBuffers();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::OnMouse(wxMouseEvent& event) {
/////////////////////////////////////////////////////////////////////
	static int dragging = 0;
	static float last_x, last_y;
	
	this->SetFocusFromKbd();

	// Allow default processing to happen, or else the canvas cannot gain focus
	// (for key events).
	event.Skip();

	if ( event.LeftIsDown() ) {
		
		if ( event.ControlDown() == true ) {
			// move origin
			const wxSize cs = GetClientSize();
			int originX = cs.GetWidth()/2;
			int originY = cs.GetHeight()/2;
			
			viewPortX = (originX - event.GetX()) * -1;
			viewPortY = (originY - event.GetY()) * +1;
			
			/*
			std::stringstream ss;
			ss << " ViewPort: " << viewPortX << ", " << viewPortY;
			ss << " Mouse Pos: " << event.GetX() << ", "<< event.GetY();
			ss << " Origin: " << originX << ", "<< originY << std::endl;
			trace(ss);
			*/
			Refresh();
			
			event.Skip(false);
		} else {
			// rotate 3D objects
			if ( !dragging ) { 
				dragging = 1;
			} else {
				displayAngels.incX((event.GetX() - last_x) * +1.0);
				displayAngels.incY((event.GetY() - last_y) * +1.0);
				Refresh();
			}
			
			last_x = event.GetX();
			last_y = event.GetY();
			event.Skip(false);
		}
	} else {
		
		dragging = 0;
		event.Skip();
	} 

	// resize 3D objects
	int rot = event.GetWheelRotation();
	if ( rot != 0 ) {
		scaleX += ( rot > 0 ? 0.1f : -0.1f );
		scaleY += ( rot > 0 ? 0.1f : -0.1f );
		scaleZ += ( rot > 0 ? 0.1f : -0.1f );
		Refresh();
		
		event.Skip(false);
	}
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::OnKeyDown(wxKeyEvent& event) {
/////////////////////////////////////////////////////////////////////
	if ( event.ControlDown() == true ) {

		const int dist = 10;
		int originX = viewPortX;
		int originY = viewPortY;
		
		switch ( event.GetKeyCode() ) {
			case WXK_RIGHT:
				viewPortX = originX + dist;
				break;

			case WXK_LEFT:
				viewPortX = originX - dist;
				break;

			case WXK_DOWN:
				viewPortY = originY - dist;
				break;

			case WXK_UP:
				viewPortY = originY + dist;
				break;
		}
		
		Refresh();
		
	} else {
		
		float angle = 5.0;
		switch ( event.GetKeyCode() ) {
			case WXK_RIGHT:
				spinPaneFromKeyboard( 0.0, -angle );
				break;

			case WXK_LEFT:
				spinPaneFromKeyboard( 0.0, angle );
				break;

			case WXK_DOWN:
				spinPaneFromKeyboard( -angle, 0.0 );
				break;

			case WXK_UP:
				spinPaneFromKeyboard( angle, 0.0 );
				break;

			case WXK_SPACE:
				switchSpinTimer();
				break;
				
			default: ;// do nothing
		}
	}
	event.Skip();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::startSpinTimer() {
/////////////////////////////////////////////////////////////////////
	if ( spinTimer.IsRunning() == false )
		spinTimer.Start(spinTimerInterval);
		
	decoratePlayButton(spinTimer.IsRunning());
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::stopSpinTimer() {
/////////////////////////////////////////////////////////////////////
	if ( spinTimer.IsRunning() )
		spinTimer.Stop();
		
	decoratePlayButton(spinTimer.IsRunning());
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::switchSpinTimer() {
/////////////////////////////////////////////////////////////////////
	if ( spinTimer.IsRunning() )	spinTimer.Stop();
	else 							spinTimer.Start(spinTimerInterval);
	
	decoratePlayButton(spinTimer.IsRunning());
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::decoratePlayButton(bool state) {
/////////////////////////////////////////////////////////////////////
	if ( playButton == NULL )
		return;
		
	wxBitmap bmp;

#ifdef USE_WXCRAFTER
	if ( state == false )	bmp = ImageLib16().Bitmap("BMP_PLAY_3D"); 
	else 					bmp = ImageLib16().Bitmap("BMP_STOP_3D");
#endif

	if ( state == true ) {
		playButton->SetToolTip("Stop Animated 3D View");
	} else {
		playButton->SetToolTip("Animate 3D View");
	}
	
	playButton->SetBitmap(bmp);
	playButton->Refresh();
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::setPlayButton(wxButton* pb) {
/////////////////////////////////////////////////////////////////////
	playButton = pb;
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::setTraceCtrl(wxTextCtrl* tr) {
/////////////////////////////////////////////////////////////////////
	traceCtrl = tr;
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::setSpinCtrls(wxSpinCtrl* sx, wxSpinCtrl* sy, wxSpinCtrl* sz) {
/////////////////////////////////////////////////////////////////////
	spinAngleX = sx;
	spinAngleY = sy;
	spinAngleZ = sz;
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::OnSpinTimer(wxTimerEvent& WXUNUSED(event)) {
/////////////////////////////////////////////////////////////////////
	spinPaneFromKeyboard(0.0, 4.0);
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::setOrigin(DrawPaneOrigin dpo) {
/////////////////////////////////////////////////////////////////////
	const wxSize cs = GetClientSize();
	const int border = 10;
	int originX = cs.GetWidth()/2;
	int originY = cs.GetHeight()/2;
	
	switch ( dpo ) {
		case DPO_TOP_LEFT:		viewPortX = (originX - border)     * -1;
								viewPortY = (originY - border)     * +1;
								break;
								
		case DPO_TOP_RIGHT:		viewPortX = (originX - border)     * +1;
								viewPortY = (originY - border)     * +1;
								break;
								
		case DPO_BOTTOM_LEFT:	viewPortX = (originX - border) 	   * -1;
								viewPortY = (originY - border)     * -1;
								break;
								
		case DPO_BOTTOM_RIGHT:	viewPortX = (originX - border)     * +1;
								viewPortY = (originY - border)     * -1;
								break;
								
		default:				viewPortX = 0;
								viewPortY = 0;
	}
}
/////////////////////////////////////////////////////////////////////
void CncOpenGLDrawPane::view(DrawPaneViewType view) {
/////////////////////////////////////////////////////////////////////

	switch ( view ) {
		case DPVT_Top:	 	setOriginTL();
							displayAngels.setX(+180.0f);
							displayAngels.setY(+0.0f);
							displayAngels.setZ(+0.0f);
							break;
							
		case DPVT_Bottom: 	setOriginBL();
							displayAngels.setX(+0.0f);
							displayAngels.setY(-360.0f);
							displayAngels.setZ(+0.0f);
							break;

		case DPVT_Front: 	setOriginBL();
							displayAngels.setX(-90.0f);
							displayAngels.setY(+0.0f);
							displayAngels.setZ(+0.0f);
							break;	
							
		case DPVT_Rear: 	setOriginBR();
							displayAngels.setX(+90.0f);
							displayAngels.setY(+180.0f);
							displayAngels.setZ(+0.0f);
							break;
							
							
		case DPVT_Left:	 	setOriginBL();
							displayAngels.setX(-90.0f);
							displayAngels.setY(+0.0f);
							displayAngels.setZ(-90.0f);
							break;	
							
		case DPVT_Right: 	setOriginBR();
							displayAngels.setX(-90.0f);
							displayAngels.setY(+0.0f);
							displayAngels.setZ(-270.0f);
							break;	

		case DPVT_3D: 		setOriginCenter();
							displayAngels.setX(displayAngels.getDefaultX());
							displayAngels.setY(displayAngels.getDefaultY());
							displayAngels.setZ(displayAngels.getDefaultZ());
							break;	
							

		default: ;//Do nothing
	}

	Refresh();
}
