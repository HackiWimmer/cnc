#ifndef CNC_SOURCE_EDITOR_H
#define CNC_SOURCE_EDITOR_H

#include "CncBaseEditor.h"

class CncSourceEditor : public CncBaseEditor {

	public:
		CncSourceEditor(wxWindow *parent);
		virtual ~CncSourceEditor();
		
		virtual bool Enable(bool enable = true)				override;
		virtual void decorateParentTabName(bool changed)	override;
		
	protected:
		virtual void initialize()							override;
		virtual bool hasEditMode()							override;
		virtual void notifyChange()							override;
		virtual void notifySave()							override;
};

#endif