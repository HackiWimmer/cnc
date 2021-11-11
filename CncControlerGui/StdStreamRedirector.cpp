#include "StdStreamRedirector.h"

//////////////////////////////////////////////////
namespace cnc 
{
	extern CncBasicLogStream cex1;
	extern CncBasicLogStream cex2;
	extern CncBasicLogStream cex3;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
StdStreamRedirector::StdStreamRedirector(CncTextCtrl* ctl)
: psbufCout		(NULL)
, sbOldCout		(NULL)
, psbufClog		(NULL)
, sbOldClog		(NULL)
, psbufCerr		(NULL)
, sbOldCerr		(NULL)
, psbufCex1		(NULL)
, sbOldCex1		(NULL)
, psbufCex2		(NULL)
, sbOldCex2		(NULL)
, psbufCex3		(NULL)
, sbOldCex3		(NULL)
//////////////////////////////////////////////////
{
	if ( ctl != NULL )
	{
		//redirect std::cout
		psbufCout = new CncCoutBuf(ctl);
		sbOldCout = (CncCoutBuf*)std::cout.rdbuf();
		std::cout.rdbuf(psbufCout);
		
		//redirect std::clog
		psbufClog = new CncClogBuf(ctl);
		sbOldClog = (CncClogBuf*)std::clog.rdbuf();
		std::clog.rdbuf(psbufClog);
		
		//redirect std::cerr
		psbufCerr = new CncCerrBuf(ctl);
		sbOldCerr = (CncCerrBuf*)std::cerr.rdbuf();
		std::cerr.rdbuf(psbufCerr);
		
		//redirect cnc::cex1
		psbufCex1 = new CncCex1Buf(ctl);
		sbOldCex1 = (CncCex1Buf*)cnc::cex1.rdbuf();
		cnc::cex1.rdbuf(psbufCex1);
		
		//redirect cnc::cex2
		psbufCex2 = new CncCex2Buf(ctl);
		sbOldCex2 = (CncCex2Buf*)cnc::cex2.rdbuf();
		cnc::cex2.rdbuf(psbufCex2);
		
		//redirect cnc::cex3
		psbufCex3 = new CncCex3Buf(ctl);
		sbOldCex3 = (CncCex3Buf*)cnc::cex3.rdbuf();
		cnc::cex3.rdbuf(psbufCex3);
	}
}
//////////////////////////////////////////////////
StdStreamRedirector::~StdStreamRedirector() {
//////////////////////////////////////////////////
	// restore
	if ( psbufCout )
	{
		std::cout.rdbuf(sbOldCout);
		delete psbufCout;
	}
	
	if ( psbufClog )
	{
		std::clog.rdbuf(sbOldClog);
		delete psbufClog;
	}
	
	if ( psbufCerr )
	{
		std::cerr.rdbuf(sbOldCerr);
		delete psbufCerr;
	}
	
	if ( psbufCex1 )
	{
		cnc::cex1.rdbuf(sbOldCex1);
		delete psbufCex1;
	}
	
	if ( psbufCex2 )
	{
		cnc::cex2.rdbuf(sbOldCex2);
		delete psbufCex2;
	}
	
	if ( psbufCex3 )
	{
		cnc::cex3.rdbuf(sbOldCex3);
		delete psbufCex3;
	}
}