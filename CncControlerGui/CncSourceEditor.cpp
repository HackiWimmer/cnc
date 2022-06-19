#include <iostream>
#include "SvgEditPopup.h"
#include "MainFrame.h"
#include "CncTemplateObserver.h"
#include "CncTemplateContext.h"
#include "CncSourceEditor.h"

///////////////////////////////////////////////////////////////////
CncSourceEditor::CncSourceEditor(wxWindow *parent) 
: CncBaseEditor(parent)
///////////////////////////////////////////////////////////////////
{
	initialize();
}
///////////////////////////////////////////////////////////////////
CncSourceEditor::~CncSourceEditor() {
///////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::initialize() {
///////////////////////////////////////////////////////////////////
	flags.handleBreakpoints		= true;
	flags.handleKeyCommands		= true;

	svgPopupMenu				= SvgEditPopup::createMenu(this, svgPopupMenu, true);
	ctlEditMode					= THE_APP->GetEditMode();
	ctlColunmPostion			= THE_APP->GetFilePosition();
	ctlStatus					= THE_APP->GetSourceEditStatus();
}
///////////////////////////////////////////////////////////////////
bool CncSourceEditor::Enable(bool enable) {
///////////////////////////////////////////////////////////////////
	bool ret = CncBaseEditor::Enable(enable);
	
	THE_APP->GetOpenSvgExtern()->Enable(enable);
	THE_APP->GetOpenSourceExtern()->Enable(enable);
	THE_APP->GetSaveTemplate()->Enable(enable);
	THE_APP->GetReloadTemplate()->Enable(enable);
	THE_APP->GetRenameTemplate()->Enable(enable);
	THE_APP->GetRemoveTemplate()->Enable(enable);
	
	THE_APP->enableSourceEditorMenuItems(enable);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
bool CncSourceEditor::hasEditMode() {
///////////////////////////////////////////////////////////////////
	if ( THE_APP->templateObserver != NULL)
		return !THE_APP->templateObserver->GetObservationMode()->GetValue();
		
	return true;
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::decorateParentTabName(bool changed) {
///////////////////////////////////////////////////////////////////
	const wxString indicator("*");
	wxString name(THE_APP->GetTemplateNotebook()->GetPageText(TemplateBookSelection::VAL::SOURCE_PANEL));
	
	if ( changed == false )
	{
		if ( name.StartsWith(indicator) == true )	name.Replace(indicator, "");
		else										return;
	}
	else
	{
		if ( IsEditable() == true ) 
		{
			if ( name.StartsWith(indicator) == false )
			{
				name.Prepend(indicator);
				
				THE_APP->clearMotionMonitor();
				THE_APP->clearPositionSpy();
			}
			else
			{
				return;
			}
		}
	}
	
	THE_APP->GetTemplateNotebook()->SetPageText(TemplateBookSelection::VAL::SOURCE_PANEL, name);
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::notifyChange() {
///////////////////////////////////////////////////////////////////
	if ( THE_TPL_CTX->getModifyFlag() != GetModify() )
		THE_TPL_CTX->setModifyFlag(GetModify());
}
///////////////////////////////////////////////////////////////////
void CncSourceEditor::notifySave() {
///////////////////////////////////////////////////////////////////
	THE_TPL_CTX->setModifyFlag(GetModify());
}


