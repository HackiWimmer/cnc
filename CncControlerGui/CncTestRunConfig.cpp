#include "MainFrame.h"
#include "GlobalFunctions.h"
#include "CncConfig.h"
#include "CncCommon.h"
#include "CncFileNameService.h"
#include "CncTemplateContext.h"
#include "CncTestRunConfig.h"

/////////////////////////////////////////////////////////////
CncTestRunConfig::CncTestRunConfig(wxWindow* parent)
: CncTestRunConfigBase	(parent)
/////////////////////////////////////////////////////////////
{
	wxString dirName; 
	THE_CONFIG->getDefaultTestTplDir(dirName);
	m_dirSelection->SetPath(dirName);
}
/////////////////////////////////////////////////////////////
CncTestRunConfig::~CncTestRunConfig() {
/////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////
void CncTestRunConfig::onCancel(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////
	EndModal(wxID_CANCEL);
}
/////////////////////////////////////////////////////////////
void CncTestRunConfig::onRun(wxCommandEvent& event) {
/////////////////////////////////////////////////////////////
	const unsigned int sel = m_lbTestCase->GetSelection();
	
	switch ( sel )
	{
		case 0:		EndModal(wxID_OK);
					runLoop();
					break;
					
		case 1:		EndModal(wxID_OK);
					runDir();
					break;
					
		default:	std::cerr << CNC_LOG_FUNCT_A(": Unknown test case! Selection = %u \n", sel);
					onCancel(event);
	}
}
/////////////////////////////////////////////////////////////
bool CncTestRunConfig::runLoop() {
/////////////////////////////////////////////////////////////
	const long loopCount = m_loopCount->GetValue();
	
	if ( loopCount <= 0 )
		return false;
	
	// loop
	bool ret		= true;
	long duration	= 0;
	
	// always do this to get a fresh CncInstruction workflow
	THE_TPL_CTX->unregisterCncInterface();
	
	for ( long i = 0; i < loopCount; i++ )
	{
		if ( THE_APP->getCncControl() && THE_APP->getCncControl()->isInterrupted() )
			break;
		
		// select the different panels to have a paint event
		// before each template run
		THE_APP->selectMonitorBookCncPanel();
		
		{
			if ( m_cbUseCncInstructions->GetValue() == false && i > 0)
				THE_TPL_CTX->unregisterCncInterface();
			
			bool ret = THE_APP->processTemplate( i == 0 );
			duration += THE_CONTEXT->timestamps.getTotalDurationMillis();

			cnc::trc.logInfoMessage(wxString::Format("Loop Counter : % 6d [#]; AVG duration: % 10ld [ms]", 
										i + 1, duration / ( i + 1 )));

			if ( ret == false )
			{
				CNC_CERR_FUNCT_A("Call of processTemplate() failed")
				if ( m_cbBreakOnError->GetValue() == true )
				{
					ret = false;
					break;
				}
			}
		}
			
		THE_APP->selectMonitorBookTemplatePanel();
		THE_APP->waitActive(100);
	}
	
	THE_APP->selectMonitorBookCncPanel();
	
	// summary
	cnc::trc.logInfoMessage("");
	std::clog << wxString::Format("Loop Repeat Test Summary: Count: % 6d [#]; AVG Duration: % 10ld [ms]", 
									loopCount, duration / loopCount) << std::endl;
	
	return ret;
}
/////////////////////////////////////////////////////////////
bool CncTestRunConfig::runDir() {
/////////////////////////////////////////////////////////////
	// ----------------------------------------------------------
	class AllFiles : public wxDirTraverser
	{
		public:
		
			unsigned int	cntTotalDirs;
			unsigned int	cntTotalFiles;
			unsigned int	cntFailed;
			
			bool			skipIgnoreNames;
			wxString		testOutputDir;

			// --------------------------------------------------
			AllFiles() 
			: wxDirTraverser	()
			, cntTotalDirs		(0)
			, cntTotalFiles		(0)
			, cntFailed			(0)
			, skipIgnoreNames	(true)
			, testOutputDir		(CncFileNameService::getTempDirSession())
			{}
			// --------------------------------------------------
			virtual ~AllFiles()
			{}
			// --------------------------------------------------
			virtual wxDirTraverseResult OnDir(const wxString& dirname)
			{
				if ( skipIgnoreNames == true )
				{
					if ( dirname.Upper().Contains("IGNORE") )
						return wxDIR_IGNORE;
				}
				
				cntTotalDirs++;
				return wxDIR_CONTINUE;
			}
			// --------------------------------------------------
			virtual wxDirTraverseResult OnFile(const wxString& filename)
			{
				if ( THE_APP->getCncControl() && THE_APP->getCncControl()->isInterrupted() )
					return wxDIR_STOP;

				bool error = false;
				wxFileName fn(filename);
				
				if ( skipIgnoreNames == true )
				{
					if ( filename.Upper().Contains("IGNORE") )
						return wxDIR_IGNORE;
				}
				
				const wxString testCaseIndex(wxString::Format("%05u", cntTotalFiles));
				
				CNC_CLOG_A("\n~~~ Start Test for '%s'", fn.GetFullPath())
				CNC_CLOG_A(" Test Case Index: %s", testCaseIndex)
				
				INC_LOGGER_INDENT
				
				if ( THE_APP->openTemplateFile(fn) )
				{
					if ( THE_APP->processTemplate() == false )
					{
						// the errors are already present
						cntFailed++;
						error = true;
					}
				}
				else
				{
					CNC_CERR_A("Error while open '%s'", fn.GetFullPath())
					cntFailed++;
					error = true;
				}
				
				// ------------------------------------------------------------------
				// summarize the test 
				// first request additional information
				ContextInterface::ProcessResult res;
				THE_TPL_CTX->analizeContextEntries(res);
				
				const bool tplCtxErr  = THE_TPL_CTX->hasErrors();
				const bool result    = ( error == false && tplCtxErr == false );

				// build a test directory
				const wxString testCaseDir(wxString::Format("%s@TC%s_%s.%s%c",
											testOutputDir,
											testCaseIndex,
											fn.GetFullName(),
											(result ? "OK" : "FAILED"),
											char(wxFileName::GetPathSeparator())
										   )
				);
				
				if ( wxFileName::Mkdir(testCaseDir) == false )
				{
					CNC_CERR_FUNCT_A("Can't create '%s'", testCaseDir)
				}
				else
				{
					// store template context content
					wxFileName ctxFn(wxString::Format("%sTemplateContext.txt", testCaseDir));
					THE_TPL_CTX->save(ctxFn);
						
					// make a optical short-cut of the result
					wxFileName bmpFn(wxString::Format("%sTemplatePicture.bmp", testCaseDir));
					THE_APP->getMotionMonitor()->saveContentAsBitmap(bmpFn);
				}
				
				// log result information
				DEC_LOGGER_INDENT
				if ( result )	CNC_CLOG_A("=== Finish Test successfully for '%s'", fn.GetFullPath())
				else			CNC_CERR_A("=== Finish Test with errors for '%s'",  fn.GetFullPath())
				
				if ( THE_APP->getCncControl() && THE_APP->getCncControl()->isInterrupted() )
					return wxDIR_STOP;
					
				cntTotalFiles++;
				return wxDIR_CONTINUE;
			}
	};
	
	// ----------------------------------------------------------
	THE_APP->getLogger()->getListCtrl()->clearAll();
	
	wxString dirName(m_dirSelection->GetPath());
	dirName.append(wxFileName::GetPathSeparator());
	
	wxDir dir(dirName);
	if ( !dir.IsOpened() )
	{
		CNC_CERR_FUNCT_A("Can't open '%s'", dirName)
		return false;
	}
	
	AllFiles allFiles;
	allFiles.skipIgnoreNames = m_cbSkipIgnore->GetValue();
	allFiles.testOutputDir.assign(wxString::Format("%sTemplateTestResults-%s%c", 
													CncFileNameService::getTempDirSession(), 
													wxDateTime::Now().Format("%Y-%m-%d.%H-%M-%S"),
													char(wxFileName::GetPathSeparator())
								 )
	);
	
	// create a test result directory
	if ( wxFileName::Mkdir(allFiles.testOutputDir) == false )
	{
		CNC_CERR_FUNCT_A("Can't create '%s'", allFiles.testOutputDir)
		return false;
	}
	
	dir.Traverse(allFiles);
	
	CNC_CEX2_A("Test execution Summary: *********************************************")
	INC_LOGGER_INDENT
	
	CNC_CEX2_A("Total count test cases: %4u", allFiles.cntTotalFiles)
	CNC_CEX2_A("          Successfully: %4u", allFiles.cntTotalFiles - allFiles.cntFailed)
	CNC_CEX2_A("                Failed: %4u", allFiles.cntFailed)
	
	DEC_LOGGER_INDENT
	CNC_CEX2_A("*********************************************************************")

	// save logger output
	wxFileName loggerSummary(wxString::Format("%sLoggerSummary.txt", allFiles.testOutputDir));
	THE_APP->getLogger()->getListCtrl()->writeToFile(loggerSummary, true);
	
	// open test result directory
	wxString fileBrowswer;
	if ( GblFunc::executeExternalProgram(THE_CONFIG->getFileBrowser(fileBrowswer), allFiles.testOutputDir, true) == false )
		CNC_CERR_FUNCT_A(": Can't open test result directory: %s", allFiles.testOutputDir)

	return true;
}
