#ifndef STD_STREAM_REDIRECTOR_H
#define STD_STREAM_REDIRECTOR_H

#include <iostream>
#include <wx/textctrl.h>
#include "CncStreamBuffers.h"

//////////////////////////////////////////////////////////
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
		
	public:
		//////////////////////////////////////////////////
		StdStreamRedirector(wxTextCtrl* ctl) {
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
		}
};

#endif