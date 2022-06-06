#include "MainFrame.h"
#include "CncConfig.h"
#include "CncCommon.h"
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
	
	for ( long i=0; i < loopCount; i++ )
	{
		if ( THE_APP->getCncControl() && THE_APP->getCncControl()->isInterrupted() )
			break;
		
		// select the different panels to have a paint event
		// before each template run
		THE_APP->selectMonitorBookCncPanel();
		
		{
			bool ret = THE_APP->processTemplateWrapper( i == 0 );
			duration += THE_CONTEXT->timestamps.getTotalDurationMillis();

			cnc::trc.logInfoMessage(wxString::Format("Loop Counter : % 6d [#]; AVG duration: % 10ld [ms]", 
										i + 1, duration / ( i + 1 )));

			if ( ret == false )
			{
				CNC_CERR_FUNCT_A("Call of processTemplateWrapper() failed")
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
	class AllFiles : public wxDirTraverser {
		
		public:
		
			unsigned int cntTotalDirs;
			unsigned int cntTotalFiles;
			unsigned int cntFailed;

			// --------------------------------------------------
			AllFiles() 
			: wxDirTraverser()
			, cntTotalDirs	(0)
			, cntTotalFiles	(0)
			, cntFailed		(0)
			{}
			// --------------------------------------------------
			virtual ~AllFiles()
			{}
			// --------------------------------------------------
			virtual wxDirTraverseResult OnDir(const wxString& dirname)
			{
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
				
				CNC_CLOG_A("\n~~~ Start Test for '%s'", fn.GetFullPath())
				INC_LOGGER_INDENT
				
				if ( THE_APP->openTemplateFile(fn) )
				{
					if ( THE_APP->processTemplateWrapper() == false )
					{
						// the errors are already present
						//CNC_CERR_A("Error while processing '%s'", fn.GetFullPath())
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
				
				DEC_LOGGER_INDENT
				if ( error )	CNC_CERR_A("=== Finish Test with errors for '%s'",  fn.GetFullPath())
				else			CNC_CLOG_A("=== Finish Test successfully for '%s'", fn.GetFullPath())
				
				cntTotalFiles++;
				return wxDIR_CONTINUE;
			}
	};
	
	// ----------------------------------------------------------
	wxString dirName(m_dirSelection->GetPath());
	dirName.append(wxFileName::GetPathSeparator());
	
	wxDir dir(dirName);
	if ( !dir.IsOpened() )
	{
		CNC_CERR_FUNCT_A("Can't open '%s'", dirName)
		return false;
	}
	
	AllFiles allFiles;
	dir.Traverse(allFiles);
	
	CNC_CEX2_A("Test execution Summary: *********************************************")
	INC_LOGGER_INDENT
	
	CNC_CEX2_A("Total count test cases: %4u", allFiles.cntTotalFiles)
	CNC_CEX2_A("          Successfully: %4u", allFiles.cntTotalFiles - allFiles.cntFailed)
	CNC_CEX2_A("                Failed: %4u", allFiles.cntFailed)
	
	DEC_LOGGER_INDENT
	CNC_CEX2_A("*********************************************************************")

	return true;
}
