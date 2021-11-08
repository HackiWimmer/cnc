#ifndef STD_STREAM_REDIRECTOR_H
#define STD_STREAM_REDIRECTOR_H

#include <iostream>
#include "CncStreamBuffers.h"

//////////////////////////////////////////////////////////
class CncTextCtrl;
class StdStreamRedirector {
	
	protected:
		// to redirect std::cout
		CncCoutBuf* psbufCout;
		CncCoutBuf* sbOldCout;

		// to redirect std::clog
		CncClogBuf* psbufClog;
		CncClogBuf* sbOldClog;

		// to redirect std::cerr
		CncCerrBuf* psbufCerr; 
		CncCerrBuf* sbOldCerr;
		
		CncCex1Buf* psbufCex1;
		CncCex1Buf* sbOldCex1;
		
		CncCex2Buf* psbufCex2;
		CncCex2Buf* sbOldCex2;
		
		CncCex3Buf* psbufCex3;
		CncCex3Buf* sbOldCex3;
		
	public:
		
		StdStreamRedirector(CncTextCtrl* ctl);
		virtual ~StdStreamRedirector();
};

#endif