#include <sstream>
#include <wx/app.h>
#include <wx/log.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/config.h>
#include <wx/splash.h>
#include <wx/dcmemory.h>
#include <wx/cmdline.h>
#include <wx/fileconf.h>
#include <wx/intl.h>
#include <wx/propgrid/property.h>
#include "OSD/CncTimeFunctions.h"
#include "CncTextCtrl.h"
#include "CncLoggerProxy.h"
#include "CncConfigProperty.h"
#include "CncFileNameService.h"
#include "CncStreamBuffers.h"
#include "CncContext.h"
#include "CncCommon.h"
#include "GlobalStrings.h"
#include "GlobalFunctions.h"
#include "MainFrame.h"

////////////////////////////////////////////////////////////////////
GlobalConstStringDatabase globalStrings;

///////////////////////////////////////////////////////////////////
static const wxCmdLineEntryDesc cmdLineDesc[] = {
///////////////////////////////////////////////////////////////////
    { wxCMD_LINE_SWITCH , "g", "dbg", 		"Start CncController in debug mode", 	wxCMD_LINE_VAL_NONE, 	wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH , "s", "secure", 	"Start CncController in secure mode",	wxCMD_LINE_VAL_NONE, 	wxCMD_LINE_PARAM_OPTIONAL },
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
	
///////////////////////////////////////////////////////////////////////////
namespace GlobalStreamRedirection {
	
	enum State { UNKNOWN, PREINSTALLED, INSTALLED };
	
	State streamRedirectionState = UNKNOWN;
	
	///////////////////////////////////////////////////////////////////
	void preInstall() {
	///////////////////////////////////////////////////////////////////
		if ( streamRedirectionState != UNKNOWN )
			return;
		
		//redirect std::cout
		psbufCout = new CncCoutBuf(NULL);
		sbOldCout = std::cout.rdbuf();
		std::cout.rdbuf(psbufCout);
		
		//redirect std::clog
		psbufClog = new CncClogBuf(NULL);
		sbOldClog = std::clog.rdbuf();
		std::clog.rdbuf(psbufClog);
		
		//redirect std::cerr
		psbufCerr = new CncCerrBuf(NULL);
		sbOldCerr = std::cerr.rdbuf();
		std::cerr.rdbuf(psbufCerr);
		
		// redirect ext1 buffer
		psbufCex1 = new CncCex1Buf(NULL);
		sbOldCex1 = cnc::cex1.rdbuf();
		((std::iostream*)&cnc::cex1)->rdbuf(psbufCex1);
		cnc::cex1.setLogStreamBuffer(psbufCex1);
		
		streamRedirectionState = PREINSTALLED;
	}
		
	///////////////////////////////////////////////////////////////////
	void install(MainFrame* mainFrame) {
		if ( streamRedirectionState != PREINSTALLED )
			return;
			
		// perform startup trace
		StartupBuffer::trace(mainFrame->getLoggerView());
		
		// set the logger control
		psbufCout->setTextControl(mainFrame->getLogger());
		psbufClog->setTextControl(mainFrame->getLogger());
		psbufCerr->setTextControl(mainFrame->getLogger());
		psbufCex1->setTextControl(mainFrame->getLogger());
		
		// redirect trace buffer
		psbufCtrc = new CncCtrcBuf(mainFrame->getTrace());
		sbOldCtrc = cnc::trc.rdbuf();
		((std::iostream*)&cnc::trc)->rdbuf(psbufCtrc);
		cnc::trc.setLogStreamBuffer(psbufCtrc);
		
		// redirect controller message buffer
		psbufCmsg = new CncCmsgBuf(mainFrame->getCtrlMessageHistory());
		sbOldCmsg = cnc::msg.rdbuf();
		((std::iostream*)&cnc::msg)->rdbuf(psbufCmsg);
		cnc::msg.setLogStreamBuffer(psbufCmsg);
		
		// redirect serial spy buffer
		psbufCspy = new CncCspyBuf(mainFrame->getCtrlSerialSpy());
		sbOldCspy = cnc::spy.rdbuf();
		((std::iostream*)&cnc::spy)->rdbuf(psbufCspy);
		cnc::spy.setLogStreamBuffer(psbufCspy);
		
		streamRedirectionState = INSTALLED;
	}
		
	///////////////////////////////////////////////////////////////////
	void reset() {
		if ( streamRedirectionState != INSTALLED )
			return;			
		// deconstruct redirecting
		
		// ungregister text controls
		psbufCout->ungregisterTextControl();
		psbufClog->ungregisterTextControl();
		psbufCerr->ungregisterTextControl();
		psbufCex1->ungregisterTextControl();
		psbufCtrc->ungregisterTextControl();
		psbufCmsg->ungregisterTextControl();
		psbufCspy->ungregisterTextControl();
		
		// redirect to old buf
		std::cout.rdbuf(sbOldCout);
		std::cerr.rdbuf(sbOldClog);
		std::cerr.rdbuf(sbOldCerr);
		((std::iostream*)&cnc::cex1)->rdbuf(sbOldCex1);
		((std::iostream*)&cnc::trc)->rdbuf(sbOldCtrc);
		((std::iostream*)&cnc::msg)->rdbuf(sbOldCmsg);
		((std::iostream*)&cnc::spy)->rdbuf(sbOldCspy);
		
		// delete stream buffers
		delete psbufCout;
		delete psbufClog;
		delete psbufCerr;
		delete psbufCex1;
		delete psbufCtrc;
		delete psbufCmsg;
		delete psbufCspy;
		
		APPEND_LOCATION_TO_STACK_TRACE_FILE;
	}
};
void GlobalStreamRedirectionReset() { GlobalStreamRedirection::reset(); }
	
///////////////////////////////////////////////////////////////////
class MainLogger : public wxLog {
///////////////////////////////////////////////////////////////////
	
	public:
		
		MainLogger() : wxLog() {}
		virtual ~MainLogger()  { APPEND_LOCATION_TO_STACK_TRACE_FILE; }
		
	protected:
		
		const char* getLogLevelAsStr(wxLogLevel level) {
			switch ( level ) {
				case wxLOG_FatalError:	return "FatalError";
				case wxLOG_Error:		return "Error     ";
				case wxLOG_Warning:		return "Warning   ";
				case wxLOG_Message:		return "Message   ";
				case wxLOG_Status:		return "Status    ";
				case wxLOG_Info:		return "Info      ";
				case wxLOG_Debug:		return "Debug     ";
				case wxLOG_Trace:		return "Trace     ";
				case wxLOG_Progress:	return "Progress  ";
				case wxLOG_User:		return "User      ";
				default:				return "????      ";
			}
			
			return "????";
		}
		
		void DoLogRecord(wxLogLevel level, const wxString &msg, const wxLogRecordInfo &info) {
			
			switch ( level ) {
				case wxLOG_Warning:		cnc::cex1 << msg << std::endl;
										break;
				case wxLOG_Message:
				case wxLOG_Info:
				case wxLOG_Status:		std::cout << msg << std::endl;
										break;
										
				default:				std::cerr << "wxLog::DoLogRecord(" << wxString(getLogLevelAsStr(level)).Trim() << ") occured!" << std::endl;
										std::cerr << msg << std::endl;
										GblFunc::stacktrace(std::cerr);
			}
		}
};

///////////////////////////////////////////////////////////////////
class MainApp : public wxApp {
///////////////////////////////////////////////////////////////////
	private:
		
		wxCmdLineParser*	parser;
		//wxLocale			locale;
		wxFileConfig*		globalFileConfig;
		
	public:
		///////////////////////////////////////////////////////////
		MainApp() 
		//: locale(wxLANGUAGE_DEFAULT)
		: parser(new wxCmdLineParser())
		, globalFileConfig(NULL)
		///////////////////////////////////////////////////////////
		{
			GlobalStreamRedirection::preInstall();
			
			// init file and dir names
			CncFileNameService::init();
			CncTimeFunctions::init();
			CncTextCtrlEditor::init();

			GblFunc::appendToStackTraceFile("Starting up ......................");
			globalFileConfig = new wxFileConfig(wxT("CncController"), 
												wxEmptyString, 
												CncFileNameService::getConfigFileName(), 
												CncFileNameService::getConfigFileName(), 
												wxCONFIG_USE_RELATIVE_PATH | wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
			
			// determine assert handler
			// this show a lot of sizer assert
			wxSetDefaultAssertHandler();
		}
		
		///////////////////////////////////////////////////////////
		virtual ~MainApp() {
		///////////////////////////////////////////////////////////
			// the ownership was taken over by the GlobalConfigManager 
			// and will be deleted there, dont do it here
			// delete globalFileConfig;
				
			APPEND_LOCATION_TO_STACK_TRACE_FILE_A("Finalized . . . ")
			GblFunc::appendToStackTraceFile("The End ..........................");
		}
		
		///////////////////////////////////////////////////////////////////
		virtual int FilterEvent(wxEvent& event) {
		///////////////////////////////////////////////////////////////////
			return wxApp::FilterEvent(event);
		} 
		
		///////////////////////////////////////////////////////////////////
		void printUsage() {
		//////////////////////////////////////////////////////////////////
			wxASSERT ( parser );
			#ifdef __WXMSW__
				parser->Usage();
			#else
				wxString usageString = parser->GetUsageString();
				std::cout << usageString.mb_str(wxConvUTF8).data() << std::endl;
			#endif
		}
		
		///////////////////////////////////////////////////////////////////
		bool getSplashImage(wxBitmap& bmp) {
		///////////////////////////////////////////////////////////////////
			if ( globalFileConfig == NULL )
				return false;
				
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
		void displaySplashImage(wxFrame* mainFrame) {
		///////////////////////////////////////////////////////////////////
			#ifdef APP_USE_SPLASH
				wxBitmap bmp;
				wxBitmap b;
				if ( getSplashImage(bmp) ) {
					
					{
						wxMemoryDC mdc(bmp);
						mdc.SetFont(wxFontInfo(18).FaceName("Helvetica").Bold());
						mdc.SetTextForeground(wxColor(255,255,255));
						mdc.DrawText(globalStrings.programVersion, {50,235});
						
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
			wxASSERT ( parser );
			parser->SetDesc(cmdLineDesc);
			parser->SetCmdLine(wxAppBase::argc, wxAppBase::argv);
			
			if ( parser->Parse(false) != 0 ) {
				printUsage();
				return false;
			}
			
			return true;
		}
		
		///////////////////////////////////////////////////////////
		bool getCmdLineParameter(const char* param) {
		///////////////////////////////////////////////////////////
			if ( parser->Found(param) )
				return true;
				
			return false;
		}
		
		///////////////////////////////////////////////////////////
		bool getCmdLineParameter(const char* param, wxString& value) {
		///////////////////////////////////////////////////////////
			if ( parser->Found(param, &value) )
				return true;
				
			return false;
		}
		
		///////////////////////////////////////////////////////////
		virtual bool OnInit() {
		///////////////////////////////////////////////////////////
			APPEND_LOCATION_TO_STACK_TRACE_FILE_A("Entry . . .");
			
			//setlocale(LC_NUMERIC, "");
			wxLocale(LC_NUMERIC);
			
			// Add the common image handlers
			wxInitAllImageHandlers();
			
			wxValidator::SuppressBellOnError(true);
			
			// build and decorate the main frame
			MainFrame* mainFrame = new MainFrame(NULL, globalFileConfig);
			
			// command line handling
			if ( initializeCmdLine() == false )
				return false;
			
			// splash screen handling
			if( getCmdLineParameter("g") == false ) {
				if ( THE_CONFIG && THE_CONFIG->getSplashScreenFlag() == true )
					displaySplashImage(mainFrame);
			}
			
			// redirect std streams
			wxLog::SetActiveTarget(new MainLogger());
			GlobalStreamRedirection::install(mainFrame);
			
			// last but not least call initialize
			mainFrame->initialize();
			
			// setup some context information
			if ( getCmdLineParameter("s") == true )	{
				THE_CONTEXT->secureModeInfo.isActive 				= true;
				THE_CONTEXT->secureModeInfo.isActivatedByStartup 	= true;
			}
			
			// start the main frame
			SetTopWindow(mainFrame);
			int ret = GetTopWindow()->Show();
			
			APPEND_LOCATION_TO_STACK_TRACE_FILE_A(wxString::Format("Result = %d", ret));
			return ret;
		}
		
		///////////////////////////////////////////////////////////
		void OnAssertFailure(const wxChar *file, int line, const wxChar *func, const wxChar *cond, const wxChar *msg) {
		///////////////////////////////////////////////////////////
			std::stringstream ss;
			ss        << std::endl
					  << "Assertion: " 
					  << wxString(file) 					<< ": "
					  << wxString(func) 					<< ": "
					  << wxString::Format("%d", line) 		<< ": "
															<< std::endl
					  
					  << "  Condition : " << wxString(cond) << std::endl
					  << "  Message   : " << wxString(msg) 	<< std::endl
					  << "  Stacktrace:"  					<< std::endl;
					
			std::cerr << ss.str();
			GblFunc::appendToStackTraceFile(ss.str().c_str());
			GblFunc::stacktraceOnlyApp(std::cerr, true);
		}
		
		///////////////////////////////////////////////////////////
		virtual int OnRun() {
		///////////////////////////////////////////////////////////
			APPEND_LOCATION_TO_STACK_TRACE_FILE_A("Entry . . .");
			
			int ret = wxApp::OnRun();
			APPEND_LOCATION_TO_STACK_TRACE_FILE_A(wxString::Format("Result = %d", ret));
			return ret;
		}
		
		///////////////////////////////////////////////////////////
		virtual int OnExit() {
		///////////////////////////////////////////////////////////
			int ret = wxApp::OnExit();
			
			APPEND_LOCATION_TO_STACK_TRACE_FILE_A(wxString::Format("Result = %d", ret));
			return ret;
		}
		
		///////////////////////////////////////////////////////////
		virtual bool OnExceptionInMainLoop() {
		///////////////////////////////////////////////////////////
			APPEND_LOCATION_TO_STACK_TRACE_FILE;
			//return wxApp::OnExceptionInMainLoop();
			//throw;
			return true;
		}
		
		/*
		///////////////////////////////////////////////////////////
		virtual void OnUnhandledException() {
		///////////////////////////////////////////////////////////
			wxApp::OnUnhandledException();
		}
		
		///////////////////////////////////////////////////////////
		virtual void OnFatalException() {
		///////////////////////////////////////////////////////////
			wxApp::OnFatalException();
		}
		 */
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)
