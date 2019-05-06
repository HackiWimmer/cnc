#ifndef CNC_STARTPOSITION_RESOLVER_H
#define CNC_STARTPOSITION_RESOLVER_H

#include "wxcrafter.h"

class CncStartPositionResolver : public CncStartPositionResolverBase {
	
	public:
		CncStartPositionResolver(wxWindow* parent);
		virtual ~CncStartPositionResolver();
		
		static const CncDoublePosition& getReferencePosition() { return ReferencePosition; }
		
	protected:
		static const CncDoublePosition ReferencePosition;
		
		CncDoublePosition distance;
		
		virtual void onCancel(wxCommandEvent& event);
		virtual void onOk(wxCommandEvent& event);
};
#endif // CNCSTARTPOSITIONRESOLVER_H
