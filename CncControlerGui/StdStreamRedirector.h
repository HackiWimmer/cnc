#ifndef STD_STREAM_REDIRECTOR_H
#define STD_STREAM_REDIRECTOR_H

#include <iostream>
#include "CncStreamBuffers.h"
//#include "CncCommon.h"

namespace cnc {
	extern CncBasicLogStream  cex1;
}

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
		
	public:
		//////////////////////////////////////////////////
		StdStreamRedirector(CncTextCtrl* ctl) {
			if ( ctl != NULL ) {
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
				((std::iostream*)&cnc::cex1)->rdbuf(psbufCex1);
			}
		}
		//////////////////////////////////////////////////
		virtual ~StdStreamRedirector() {
			// restore
			if ( psbufCout ) {
				std::cout.rdbuf(sbOldCout);
				delete psbufCout;
			}
			
			if ( psbufClog ) {
				std::clog.rdbuf(sbOldClog);
				delete psbufClog;
			}
			
			if ( psbufCerr ) {
				std::cerr.rdbuf(sbOldCerr);
				delete psbufCerr;
			}
			
			if ( psbufCex1 ) {
				((std::iostream*)&cnc::cex1)->rdbuf(sbOldCex1);
				delete psbufCex1;
			}
		}
};

#endif