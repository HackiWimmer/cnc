#ifndef CNC_OUTBOUND_EDITOR_H
#define CNC_OUTBOUND_EDITOR_H

#include "CncBaseEditor.h"

class CncOutboundEditor : public CncBaseEditor {

	public:
		CncOutboundEditor(wxWindow *parent);
		virtual ~CncOutboundEditor();
		
	protected:
		virtual void initialize();
		virtual bool hasEditMode() { return false; }

};

#endif