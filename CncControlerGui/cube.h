/////////////////////////////////////////////////////////////////////////////
// Name:        cube.h
// Purpose:     wxGLCanvas demo program
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CUBE_H_
#define _WX_CUBE_H_

#include "wx/glcanvas.h"
#include "CncDrawPane.h"
#include "CncDrawPaneContext.h"


// Define a new application type
class MyApp : public wxApp
{
	public:
	    MyApp() { 
	    }
	
	    // virtual wxApp methods
	    virtual bool OnInit() wxOVERRIDE;
	    virtual int OnExit() wxOVERRIDE;
	
	private:
};

// Define a new frame type
class MyFrame : public wxFrame
{
	public:
	    MyFrame();
	
	private:
		
		CncOpenGLDrawPane* drawePane3D;
		
	    void OnClose(wxCommandEvent& event);
	    
	    void Test1(wxCommandEvent& event);
	    void Test2(wxCommandEvent& event);
	    void Test3(wxCommandEvent& event);
	    void Test4(wxCommandEvent& event);
	    void Test5(wxCommandEvent& event);
	    
	    void Clear(wxCommandEvent& event);
	    
	    void Front(wxCommandEvent& event);
	    void Rear(wxCommandEvent& event);
	    void Top(wxCommandEvent& event);
	    void Bottom(wxCommandEvent& event);
	    void Left(wxCommandEvent& event);
	    void Right(wxCommandEvent& event);
	    void V3D(wxCommandEvent& event);
	    
	    void test1();
	    void test2();
	    void test3();
	    void test4();
	    void test5();
	
    	wxDECLARE_EVENT_TABLE();
};



#endif // _WX_CUBE_H_
