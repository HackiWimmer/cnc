#include <sstream>
#include <wx/app.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/config.h>
#include <wx/splash.h>
#include <wx/dcmemory.h>
#include <wx/cmdline.h>
#include <wx/intl.h>
#include "CncFileNameService.h"
#include "CncStreamBuffers.h"
#include "CncTimeFunctions.h"
#include "MainFrame.h"

extern const char* _programVersion;
extern const char* _copyRight;

///////////////////////////////////////////////////////////////////
static const wxCmdLineEntryDesc cmdLineDesc[] = {
///////////////////////////////////////////////////////////////////
    { wxCMD_LINE_SWITCH , "g", "dbg", "Start CncController in debug mode", wxCMD_LINE_VAL_NONE , wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_NONE }
};

///////////////////////////////////////////////////////////////////

// redirect std::cout
CncCoutBuf*  psbufCout;
std::streambuf *sbOldCout;

// redirect std::clog
CncClogBuf*  psbufClog;
std::streambuf *sbOldClog;

// redirect std::cerr
CncCerrBuf*  psbufCerr; 
std::streambuf *sbOldCerr;

// redirect cnc::cex1
CncCex1Buf*  psbufCex1;
std::streambuf *sbOldCex1;

// redirect cnc::trc
CncCtrcBuf*  psbufCtrc;
std::streambuf *sbOldCtrc;

// redirect cnc::trc
CncCmsgBuf*  psbufCmsg;
std::streambuf *sbOldCmsg;

// redirect cnc::trc
CncCspyBuf*  psbufCspy;
std::streambuf *sbOldCspy;

namespace cnc {
	CncSerialSpyStream spy;
	CncMsgLogStream msg;
	CncTraceLogStream trc;
	CncBasicLogStream cex1;
};
	
///////////////////////////////////////////////////////////////////
void installStreamRedirection(MainFrame* mainFrame) {
///////////////////////////////////////////////////////////////////
	//redirect std::cout
	psbufCout = new CncCoutBuf(mainFrame->getLogger());
	sbOldCout = std::cout.rdbuf();
	std::cout.rdbuf(psbufCout);
	
	//redirect std::clog
	psbufClog = new CncClogBuf(mainFrame->getLogger());
	sbOldClog = std::clog.rdbuf();
	std::clog.rdbuf(psbufClog);
	
	//redirect std::cerr
	psbufCerr = new CncCerrBuf(mainFrame->getLogger());
	sbOldCerr = std::cerr.rdbuf();
	std::cerr.rdbuf(psbufCerr);
	
	// redirect ext1 buffer
	psbufCex1 = new CncCex1Buf(mainFrame->getLogger());
	sbOldCex1 = cnc::cex1.rdbuf();
	((iostream*)&cnc::cex1)->rdbuf(psbufCex1);
	cnc::cex1.setLogStreamBuffer(psbufCex1);
	
	// redirect trace buffer
	psbufCtrc = new CncCtrcBuf(mainFrame->getTrace());
	sbOldCtrc = cnc::trc.rdbuf();
	((iostream*)&cnc::trc)->rdbuf(psbufCtrc);
	cnc::trc.setLogStreamBuffer(psbufCtrc);
	
	// redirect controller message buffer
	psbufCmsg = new CncCmsgBuf(mainFrame->getCtrlMessageHistory());
	sbOldCmsg = cnc::msg.rdbuf();
	((iostream*)&cnc::msg)->rdbuf(psbufCmsg);
	cnc::msg.setLogStreamBuffer(psbufCmsg);
	
	// redirect serial spy buffer
	psbufCspy = new CncCspyBuf(mainFrame->getCtrlSerialSpy());
	sbOldCspy = cnc::spy.rdbuf();
	((iostream*)&cnc::spy)->rdbuf(psbufCspy);
	cnc::spy.setLogStreamBuffer(psbufCspy);
}

///////////////////////////////////////////////////////////////////
void resetStreamRedirection() {
///////////////////////////////////////////////////////////////////
	// deconstruct redirecting
	std::cout.rdbuf(sbOldCout);
	std::cerr.rdbuf(sbOldClog);
	std::cerr.rdbuf(sbOldCerr);
	((iostream*)&cnc::cex1)->rdbuf(sbOldCex1);
	((iostream*)&cnc::trc)->rdbuf(sbOldCtrc);
	((iostream*)&cnc::msg)->rdbuf(sbOldCmsg);
	((iostream*)&cnc::spy)->rdbuf(sbOldCspy);
	
	// delete stream buffers
	delete psbufCout;
	delete psbufClog;
	delete psbufCerr;
	delete psbufCex1;
	delete psbufCtrc;
	delete psbufCmsg;
	delete psbufCspy;
}

///////////////////////////////////////////////////////////////////
class MainApp : public wxApp {
///////////////////////////////////////////////////////////////////
	private:
		wxCmdLineParser parser;
		wxLocale locale;
		wxFileConfig* globalFileConfig;
		
	
	public:
		///////////////////////////////////////////////////////////
		MainApp() 
		//: locale(wxLANGUAGE_DEFAULT)
		: globalFileConfig(NULL)
		{
		///////////////////////////////////////////////////////////
			// init file and dir names
			CncFileNameService::init();
			CncTimeFunctions::init();
			globalFileConfig = new wxFileConfig(wxT("CncController"), wxEmptyString, CncFileNameService::getConfigFileName(), CncFileNameService::getConfigFileName(), wxCONFIG_USE_RELATIVE_PATH | wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
			
			// determine assert handler
			// this show a lot of sizer assert
			//wxSetDefaultAssertHandler();
		}
		
		///////////////////////////////////////////////////////////
		virtual ~MainApp() {
		///////////////////////////////////////////////////////////
			resetStreamRedirection();
			
			if ( globalFileConfig != NULL )
				;//delete globalFileConfig;
		}
		
		///////////////////////////////////////////////////////////////////
		void printUsage(wxCmdLineParser& parser) {
		//////////////////////////////////////////////////////////////////
			#ifdef __WXMSW__
				parser.Usage();
			#else
				wxString usageString = parser.GetUsageString();
				std::cout << usageString.mb_str(wxConvUTF8).data() << std::endl;
			#endif
		}
		
		///////////////////////////////////////////////////////////////////
		bool getSplashImage(wxBitmap& bmp) {
		///////////////////////////////////////////////////////////////////
			//return false;
			
			wxFileName fn("splash.bmp");
			if ( fn.Exists() ) {
				if ( bmp.LoadFile(wxT("splash.bmp"), wxBITMAP_TYPE_BMP) )
					return true;
			}
			
			fn.SetPath("..\\");
			if ( fn.Exists() ) {
				if ( bmp.LoadFile(wxT("..\\splash.bmp"), wxBITMAP_TYPE_BMP) )
					return true;
			}

			return false;
		}
		
		///////////////////////////////////////////////////////////////////
		void displaySplashImage(MainFrame* mainFrame) {
		///////////////////////////////////////////////////////////////////
//			return;
			#ifdef APP_USE_SPLASH
				wxBitmap bmp;
				wxBitmap b;
				if ( getSplashImage(bmp) ) {
					
					{
						wxMemoryDC mdc(bmp);
						mdc.SetFont(wxFontInfo(18).FaceName("Helvetica").Bold());
						mdc.SetTextForeground(wxColor(255,255,255));
						mdc.DrawText(_programVersion, {50,235});
						
						/*
						//todo
						mdc.SetFont(wxFontInfo(9).FaceName("Helvetica").Italic().Bold());
						mdc.SetTextForeground(wxColor(0,174,239));
						mdc.DrawText(_copyRight, {12,338});
						*/
						
						bmp = mdc.GetAsBitmap();
					}
					
					new wxSplashScreen( bmp,
										wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
										3400, mainFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize,
										wxSIMPLE_BORDER|wxSTAY_ON_TOP);
				}
			#endif

			#if !defined(__WXGTK20__)
					// we don't need it at least on wxGTK with GTK+ 2.12.9
					wxYield();
			#endif
		}
		
		///////////////////////////////////////////////////////////
		bool initializeCmdLine() {
		///////////////////////////////////////////////////////////
			parser.SetDesc(cmdLineDesc);
			parser.SetCmdLine(wxAppBase::argc, wxAppBase::argv);
			
			if( parser.Parse(false) != 0 ) {
				printUsage(parser);
				return false;
			}
			
			return true;
		}
		
		///////////////////////////////////////////////////////////
		bool getCmdLineParameter(const char* param) {
		///////////////////////////////////////////////////////////
			if( parser.Found(param) )
				return true;
				
			return false;
		}
		
		///////////////////////////////////////////////////////////
		bool getCmdLineParameter(const char* param, wxString& value) {
		///////////////////////////////////////////////////////////
			if( parser.Found(param, &value) )
				return true;
				
			return false;
		}
		
		///////////////////////////////////////////////////////////
		virtual int OnRun() {
		///////////////////////////////////////////////////////////
			return wxApp::OnRun();
			//return 0;
		}
		
		///////////////////////////////////////////////////////////
		virtual bool OnInit() {
		///////////////////////////////////////////////////////////
			//setlocale(LC_NUMERIC, "");
			wxLocale(LC_NUMERIC);
			
			// Add the common image handlers
			wxImage::AddHandler( new wxPNGHandler );
			wxImage::AddHandler( new wxJPEGHandler );
			wxInitAllImageHandlers();
			
			wxValidator::SuppressBellOnError(true);
			
			// build and decorate the main frame
			MainFrame *mainFrame = new MainFrame(NULL, globalFileConfig);
			
			// command line handling
			if ( initializeCmdLine() == false )
				return false;
			
			// splash screen handling
			if( getCmdLineParameter("g") == false )
				displaySplashImage(mainFrame);
				
			// redirect std streams
			installStreamRedirection(mainFrame);
			
			// last but not least call initialize
			mainFrame->initialize();
			
			SetTopWindow(mainFrame);
			return GetTopWindow()->Show();
		}
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)
