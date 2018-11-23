#ifndef CNC_SOURCE_EDITOR_H
#define CNC_SOURCE_EDITOR_H

#include "CncBaseEditor.h"

class CncSourceEditor : public CncBaseEditor {

	public:
		CncSourceEditor(wxWindow *parent);
		virtual ~CncSourceEditor();
		
	protected:
		virtual void initialize();
};

#endif