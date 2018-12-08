#ifndef CNC_SOURCE_EDITOR_H
#define CNC_SOURCE_EDITOR_H

#include "CncBaseEditor.h"

class CncSourceEditor : public CncBaseEditor {

	public:
		CncSourceEditor(wxWindow *parent);
		virtual ~CncSourceEditor();
		virtual bool Enable(bool enable = true);
		virtual void decorateParentTabName(bool changed);
		
	protected:
		virtual void initialize();
		virtual bool hasEditMode();
};

#endif