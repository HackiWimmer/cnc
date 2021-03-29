#include <wx/fileconf.h>
#include "GlobalFunctions.h"
#include "CncConfig.h"
#include "CncContext.h"
#include "CncFloatingPointValidator.h"
#include "CncFilenameService.h"
#include "CncAnchorPosition.h"

///////////////////////////////////////////////////////////////////
CncAnchorPosition::CncAnchorPosition(wxWindow* parent)
: CncAnchorPositionBase				(parent)
, CncAnchorPosListCtrl::Interface	()
, anchorList						(NULL) 
, canAdd							(true)
, canMod							(false)
, canDel							(false)
///////////////////////////////////////////////////////////////////
{
	// Controllers message history
	anchorList = new CncAnchorPosListCtrl(this, wxLC_SINGLE_SEL); 
	GblFunc::replaceControl(m_anchorListPlaceholder, anchorList);
	
	anchorList->setObserver(this);
	
	m_valN->ChangeValue("New Anchor");
	m_valS->SetValue(true);
	m_valX->ChangeValue(wxString::Format("%.3lf", 0.0));
	m_valY->ChangeValue(wxString::Format("%.3lf", 0.0));
	m_valZ->ChangeValue(wxString::Format("%.3lf", 0.0));
	
	wxFloatingPointValidator<float> validator(3, NULL, wxNUM_VAL_DEFAULT);
	validator.SetRange(-1000.0, 1000.0);
	m_valX->SetValidator(validator);
	m_valY->SetValidator(validator);
	m_valZ->SetValidator(validator);
	m_valT->Select(0);
	
	load();
	anchorList->selAnchor("");
}
///////////////////////////////////////////////////////////////////
CncAnchorPosition::~CncAnchorPosition() {
///////////////////////////////////////////////////////////////////
	wxDELETE( anchorList );
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::load() {
///////////////////////////////////////////////////////////////////
	wxString dbName(CncFileNameService::getDatabaseDir());
	CncFileNameService::ensureEndWithPathSep(dbName);
	dbName.append("StdAnchor.list");
	
	wxFileName fn(dbName);
	if ( fn.Exists() == false )
		return;
	
	wxFileConfig db(wxT("StdAnchor"), 
					wxEmptyString, 
					dbName, 
					dbName, 
					wxCONFIG_USE_RELATIVE_PATH | wxCONFIG_USE_NO_ESCAPE_CHARACTERS
	);
	
	// store all anchors  ...
	wxArrayString	anchorNames;
	wxString		anchorName;
	long			anchorIndex;
	
	bool bAnchor = db.GetFirstGroup(anchorName, anchorIndex);
	while ( bAnchor ) {
		anchorNames.Add(anchorName);
		bAnchor = db.GetNextGroup(anchorName, anchorIndex);
	}
	
	// over all anchors
	for ( unsigned int i=0; i<anchorNames.GetCount(); i++ ) {
		
		anchorName.assign(anchorNames.Item(i));
		db.SetPath(wxString::Format("/%s", anchorName));
		
		CncAnchorPosListCtrl::AnchorInfo ai;
		ai.name		= wxString::Format("*%s", anchorNames.Item(i));
		ai.type		= db.Read("T", "XYZ");
		ai.fixed	= true;
		ai.show		= db.ReadBool("D", true);
		ai.pos		= { db.ReadDouble("X", 0.0), db.ReadDouble("Y", 0.0), db.ReadDouble("Z", 0.0) };
		anchorList->addAnchor(ai);
	}
	
	update();
	provide();
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::provide() {
///////////////////////////////////////////////////////////////////
	if ( THE_CONTEXT->anchorMap == NULL )
		return;
		
	CncAnchorMap& cam = *THE_CONTEXT->anchorMap;
	const CncAnchorPosListCtrl::AnchorMap& am = anchorList->getAnchorMap();

	for (auto it = am.begin(); it != am.end(); ++it ) {
		const CncAnchorPosListCtrl::AnchorInfo& sai = it->second;
		CncAnchorInfo nai;
		
		nai.show	= sai.show; 
		nai.fixed	= sai.fixed;
		nai.name	= sai.name;
		nai.type	= sai.type;
		nai.pos		= sai.pos;
		
		cam[nai.name] = nai;
	}
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::onInitDialog(wxInitDialogEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::onShow(wxShowEvent& event) {
///////////////////////////////////////////////////////////////////
	event.Skip();
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::onUpdateName(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	canAdd = anchorList->hasAnchor(m_valN->GetValue()) == false;
	canMod = !canAdd;
	canDel = !canAdd;
	update();
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::notifySelection(const CncAnchorPosListCtrl::AnchorInfo& ai) {
///////////////////////////////////////////////////////////////////
	m_valN->ChangeValue(ai.name);
	m_valT->SetValue(ai.type);
	m_valS->SetValue(ai.show);
	m_valX->ChangeValue(wxString::Format("%.3lf", ai.pos.getX()));
	m_valY->ChangeValue(wxString::Format("%.3lf", ai.pos.getY()));
	m_valZ->ChangeValue(wxString::Format("%.3lf", ai.pos.getZ()));
	
	canAdd = false;
	canMod = (ai.fixed == false);
	canDel = canMod;
	
	update();
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::notifyActivation(const CncAnchorPosListCtrl::AnchorInfo& ai) {
///////////////////////////////////////////////////////////////////
	// nothing to do
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::update() {
///////////////////////////////////////////////////////////////////
	m_btAdd->Enable(canAdd);
	m_btMod->Enable(canMod);
	m_btDel->Enable(canDel);
	
	// let m_valN enabled to provide the add path
	m_valT->Enable(canMod);
	m_valS->Enable(canMod);
	m_valX->Enable(canMod);
	m_valY->Enable(canMod);
	m_valZ->Enable(canMod);
	
	if ( canMod == true )
		processType();
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::onAdd(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	onMod(event);
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::onMod(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	double x; m_valX->GetValue().ToDouble(&x);
	double y; m_valY->GetValue().ToDouble(&y);
	double z; m_valZ->GetValue().ToDouble(&z);
	
	CncAnchorPosListCtrl::AnchorInfo ai;
	ai.name	= m_valN->GetValue();
	ai.pos	= { x, y, z };
	ai.show = m_valS->GetValue();
	ai.type	= m_valT->GetValue();
	
	ai.name.Replace("*", "");
	
	anchorList->modAnchor(ai);
	update();
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::onDel(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	anchorList->delAnchor(m_valN->GetValue());
	update();
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::close() {
///////////////////////////////////////////////////////////////////
	provide();
	Show(false);
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::onClose(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	close();
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::onCloseWindow(wxCloseEvent& event) {
///////////////////////////////////////////////////////////////////
	close();
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::onSelectType(wxCommandEvent& event) {
///////////////////////////////////////////////////////////////////
	processType();
}
///////////////////////////////////////////////////////////////////
void CncAnchorPosition::processType() {
///////////////////////////////////////////////////////////////////
	const wxString& t = m_valT->GetValue();
	
	auto enableXYZ = [&](bool x, bool y, bool z) {
		m_valX->Enable(x);
		m_valY->Enable(y);
		m_valZ->Enable(z);
		
		if ( x == false )	m_valX->ChangeValue("");
		if ( y == false )	m_valY->ChangeValue("");
		if ( z == false )	m_valZ->ChangeValue("");
	};
	
	if      ( t.IsSameAs("XYZ") )	{ enableXYZ(true,  true,  true ); }
	else if ( t.IsSameAs("XY")  )	{ enableXYZ(true,  true,  false); }
	else if ( t.IsSameAs("XZ")  )	{ enableXYZ(true,  false, true ); }
	else if ( t.IsSameAs("YZ")  )	{ enableXYZ(false, true,  true ); }
	else if ( t.IsSameAs("X")   )	{ enableXYZ(true,  false, false); }
	else if ( t.IsSameAs("Y")   )	{ enableXYZ(false, true,  false); }
	else if ( t.IsSameAs("Z")   )	{ enableXYZ(false, false, true ); }
	else							{ enableXYZ(false, false, false); }
}

