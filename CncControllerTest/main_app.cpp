#include <iostream>
#include "impl.h"
#include "main_app.h"

IMPLEMENT_APP_CONSOLE(MainApp)

MainApp::MainApp()
{
}

MainApp::~MainApp()
{
}

int MainApp::OnExit()
{
    return TRUE;
}

bool MainApp::OnInit()
{
    wxCmdLineParser parser(wxAppConsole::argc, wxAppConsole::argv);
    if ( !DoParseCommandLine(parser) ) {
        return false;
    }
	
	Implementation impl;
	impl.run();
    
    // place your initialization code here
    return true;
}

bool MainApp::DoParseCommandLine(wxCmdLineParser& parser)
{
    wxUnusedVar(parser);
    return true;
}
