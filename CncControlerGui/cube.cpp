///////////////////////////////////////////////////////////////////////////////
// Name:        cube.cpp
// Purpose:     wxGLCanvas demo program
// Author:      Julian Smart
// Modified by: Vadim Zeitlin to use new wxGLCanvas API (2007-04-09)
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "cube.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// MyApp: the application object
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

// ----------------------------------------------------------------------------
bool MyApp::OnInit() {
// ----------------------------------------------------------------------------
    if ( !wxApp::OnInit() )
        return false;

    new MyFrame();

    return true;
}
// ----------------------------------------------------------------------------
int MyApp::OnExit() {
// ----------------------------------------------------------------------------

    return wxApp::OnExit();
}

enum { MID_TEST1, MID_TEST2, MID_TEST3, MID_TEST4, MID_TEST5, 
	   MID_FRONT, MID_REAR, MID_TOP, MID_BOTTOM, MID_LEFT, MID_RIGHT, MID_3D,
	   MID_CLEAR
	 };


// ----------------------------------------------------------------------------
// MyFrame: main application window
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_CLOSE, MyFrame::OnClose)

    EVT_MENU(MID_TEST1, 	MyFrame::Test1)
    EVT_MENU(MID_TEST2, 	MyFrame::Test2)
    EVT_MENU(MID_TEST3, 	MyFrame::Test3)
    EVT_MENU(MID_TEST4, 	MyFrame::Test4)
    EVT_MENU(MID_TEST5, 	MyFrame::Test5)
    	
    EVT_MENU(MID_CLEAR, 	MyFrame::Clear)
    	
    EVT_MENU(MID_FRONT, 	MyFrame::Front)
    EVT_MENU(MID_REAR, 		MyFrame::Rear)
    EVT_MENU(MID_TOP, 		MyFrame::Top)
    EVT_MENU(MID_BOTTOM, 	MyFrame::Bottom)
    EVT_MENU(MID_LEFT, 		MyFrame::Left)
    EVT_MENU(MID_RIGHT, 	MyFrame::Right)
    	
    EVT_MENU(MID_3D, 		MyFrame::V3D)	
    	
wxEND_EVENT_TABLE()

MyFrame::MyFrame()
: wxFrame(NULL, wxID_ANY, wxT("OpenGL"))
, drawePane3D(new CncOpenGLDrawPane(this, NULL))
{
    drawePane3D->determineDisplayAngles(30.0, 30.0, 30.0);
    test1();

    SetIcon(wxICON(sample));

    // Make a menubar
    wxMenu *main = new wxMenu;
    
    main->Append(MID_TEST1, 	"Test1");
    main->Append(MID_TEST2, 	"Test2");
    main->Append(MID_TEST3, 	"Test3");
    main->Append(MID_TEST4, 	"Test4");
    main->Append(MID_TEST5, 	"Test5");
    
    main->AppendSeparator();
    main->Append(MID_CLEAR, 	"Clear");
    
    main->AppendSeparator();
    main->Append(wxID_CLOSE);
    
    wxMenu *view = new wxMenu;
    
  	view->Append(MID_FRONT, 	"Front");
    view->Append(MID_REAR, 		"Rear");
    view->Append(MID_TOP, 		"Top");
    view->Append(MID_BOTTOM, 	"Bottom");
    view->Append(MID_LEFT, 	 	"Left");
    view->Append(MID_RIGHT, 	"Right");

    view->AppendSeparator();
    view->Append(MID_3D, 		"3D");
      
    
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(main, wxT("&Menu"));
    menuBar->Append(view, wxT("&View"));

    SetMenuBar(menuBar);
    CreateStatusBar();
	
    SetClientSize(800, 800);
    Show();
}
// ----------------------------------------------------------------------------
void MyFrame::Test1(wxCommandEvent& WXUNUSED(event)) {
// ----------------------------------------------------------------------------
	test1();
}
// ----------------------------------------------------------------------------
void MyFrame::Test2(wxCommandEvent& WXUNUSED(event)) {
// ----------------------------------------------------------------------------
	test2();
}
// ----------------------------------------------------------------------------
void MyFrame::Test3(wxCommandEvent& WXUNUSED(event)) {
// ----------------------------------------------------------------------------
	test3();
}
// ----------------------------------------------------------------------------
void MyFrame::Test4(wxCommandEvent& WXUNUSED(event)) {
// ----------------------------------------------------------------------------
	test4();
}
// ----------------------------------------------------------------------------
void MyFrame::Test5(wxCommandEvent& WXUNUSED(event)) {
// ----------------------------------------------------------------------------
	test5();
}
// ----------------------------------------------------------------------------
void MyFrame::Clear(wxCommandEvent& WXUNUSED(event)) {
// ----------------------------------------------------------------------------
	drawePane3D->clearDataVector();
}
// ----------------------------------------------------------------------------
void MyFrame::OnClose(wxCommandEvent& WXUNUSED(event)) {
// ----------------------------------------------------------------------------
    // true is to force the frame to close
    Close(true);
}
// ----------------------------------------------------------------------------
void MyFrame::test1() {
// ----------------------------------------------------------------------------
	
	DrawPaneData& dpd = drawePane3D->clearDataVector();
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

}
// ----------------------------------------------------------------------------
void MyFrame::test2() {
// ----------------------------------------------------------------------------
	// tbd
}
// ----------------------------------------------------------------------------
void MyFrame::test3() {
// ----------------------------------------------------------------------------
	// tbd
}
// ----------------------------------------------------------------------------
void MyFrame::test4() {
// ----------------------------------------------------------------------------
	// tbd
}
// ----------------------------------------------------------------------------
void MyFrame::test5() {
// ----------------------------------------------------------------------------
	// tbd
}
// ----------------------------------------------------------------------------
void MyFrame::Front(wxCommandEvent& WXUNUSED(event)) {
// ----------------------------------------------------------------------------
	drawePane3D->viewFront();
}
// ----------------------------------------------------------------------------
void MyFrame::Rear(wxCommandEvent& WXUNUSED(event)) {
// ----------------------------------------------------------------------------
	drawePane3D->viewRear();
}
// ----------------------------------------------------------------------------
void MyFrame::Top(wxCommandEvent& WXUNUSED(event)) {
// ----------------------------------------------------------------------------
	drawePane3D->viewTop();
}
// ----------------------------------------------------------------------------
void MyFrame::Bottom(wxCommandEvent& WXUNUSED(event)) {
// ----------------------------------------------------------------------------
	drawePane3D->viewBottom();
}
// ----------------------------------------------------------------------------
void MyFrame::Left(wxCommandEvent& WXUNUSED(event)) {
// ----------------------------------------------------------------------------
	drawePane3D->viewLeft();
}
// ----------------------------------------------------------------------------
void MyFrame::Right(wxCommandEvent& WXUNUSED(event)) {
// ----------------------------------------------------------------------------
	drawePane3D->viewRight();
}
// ----------------------------------------------------------------------------
void MyFrame::V3D(wxCommandEvent& WXUNUSED(event)) {
// ----------------------------------------------------------------------------
	drawePane3D->view3D();
}

