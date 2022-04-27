//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxCrafterReferencePosition.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CNCGUICONTROLLER_CNCCONTROLERGUI_WXCRAFTERREFERENCEPOSITION_BASE_CLASSES_H
#define _CNCGUICONTROLLER_CNCCONTROLERGUI_WXCRAFTERREFERENCEPOSITION_BASE_CLASSES_H

// clang-format off
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/dialog.h>
#include <wx/iconbndl.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/combobox.h>
#include <wx/arrstr.h>
#include <wx/checkbox.h>
#include <wx/infobar.h>
#include <wx/notebook.h>
#include <wx/imaglist.h>
#include <wx/radiobox.h>
#include <wx/timer.h>
#include <wx/listbook.h>
#include <wx/bmpbuttn.h>
#include <wx/tglbtn.h>
#if wxVERSION_NUMBER >= 2900
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/treebook.h>
#endif

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif

// clang-format on

class CncStartPositionResolverBase : public wxDialog
{
protected:
    wxStaticBitmap* m_staticBitmap7832;
    wxStaticText* m_staticText78442;
    wxTextCtrl* m_currentPositionX;
    wxTextCtrl* m_currentPositionY;
    wxTextCtrl* m_currentPositionZ;
    wxStaticText* m_staticText7874;
    wxStaticText* m_staticText78464;
    wxTextCtrl* m_referencePositionX;
    wxTextCtrl* m_referencePositionY;
    wxTextCtrl* m_referencePositionZ;
    wxStaticText* m_staticText7879;
    wxStaticText* m_staticText78486;
    wxTextCtrl* m_distanceX;
    wxTextCtrl* m_distanceY;
    wxTextCtrl* m_distanceZ;
    wxStaticText* m_staticText7885;
    wxStaticText* m_staticText7857;
    wxListBox* m_sequenceList;
    wxStaticLine* m_staticLine7836;
    wxButton* m_button7828;
    wxButton* m_button7830;

protected:
    virtual void onOk(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void onCancel(wxCommandEvent& event)
    {
	event.Skip();
    }

public:
    wxStaticBitmap* GetStaticBitmap7832()
    {
	return m_staticBitmap7832;
    }
    wxStaticText* GetStaticText78442()
    {
	return m_staticText78442;
    }
    wxTextCtrl* GetCurrentPositionX()
    {
	return m_currentPositionX;
    }
    wxTextCtrl* GetCurrentPositionY()
    {
	return m_currentPositionY;
    }
    wxTextCtrl* GetCurrentPositionZ()
    {
	return m_currentPositionZ;
    }
    wxStaticText* GetStaticText7874()
    {
	return m_staticText7874;
    }
    wxStaticText* GetStaticText78464()
    {
	return m_staticText78464;
    }
    wxTextCtrl* GetReferencePositionX()
    {
	return m_referencePositionX;
    }
    wxTextCtrl* GetReferencePositionY()
    {
	return m_referencePositionY;
    }
    wxTextCtrl* GetReferencePositionZ()
    {
	return m_referencePositionZ;
    }
    wxStaticText* GetStaticText7879()
    {
	return m_staticText7879;
    }
    wxStaticText* GetStaticText78486()
    {
	return m_staticText78486;
    }
    wxTextCtrl* GetDistanceX()
    {
	return m_distanceX;
    }
    wxTextCtrl* GetDistanceY()
    {
	return m_distanceY;
    }
    wxTextCtrl* GetDistanceZ()
    {
	return m_distanceZ;
    }
    wxStaticText* GetStaticText7885()
    {
	return m_staticText7885;
    }
    wxStaticText* GetStaticText7857()
    {
	return m_staticText7857;
    }
    wxListBox* GetSequenceList()
    {
	return m_sequenceList;
    }
    wxStaticLine* GetStaticLine7836()
    {
	return m_staticLine7836;
    }
    wxButton* GetButton7828()
    {
	return m_button7828;
    }
    wxButton* GetButton7830()
    {
	return m_button7830;
    }
    CncStartPositionResolverBase(wxWindow* parent,
        wxWindowID id = wxID_ANY,
        const wxString& title = _("Cnc Start Position Resolver"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(-1, -1),
        long style = wxDEFAULT_DIALOG_STYLE);
    virtual ~CncStartPositionResolverBase();
};

class CncAnchorPositionBase : public wxDialog
{
protected:
    wxPanel* m_anchorListPlaceholder;
    wxButton* m_btAdd;
    wxButton* m_btMod;
    wxButton* m_btDel;
    wxStaticLine* m_staticLine585;
    wxButton* m_btDel9;
    wxStaticLine* m_staticLine577;
    wxStaticText* m_staticText555;
    wxTextCtrl* m_valN;
    wxStaticText* m_staticText593;
    wxComboBox* m_valC;
    wxStaticText* m_staticText579;
    wxCheckBox* m_valS;
    wxStaticText* m_staticText4711;
    wxComboBox* m_valT;
    wxStaticText* m_staticText5392;
    wxTextCtrl* m_valX;
    wxStaticText* m_staticText5396;
    wxTextCtrl* m_valY;
    wxStaticText* m_staticText539;
    wxTextCtrl* m_valZ;
    wxStaticText* m_staticText543;

protected:
    virtual void onCloseWindow(wxCloseEvent& event)
    {
	event.Skip();
    }
    virtual void onInitDialog(wxInitDialogEvent& event)
    {
	event.Skip();
    }
    virtual void onShow(wxShowEvent& event)
    {
	event.Skip();
    }
    virtual void onAdd(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void onMod(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void onDel(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void onClose(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void onUpdateName(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void onSelectType(wxCommandEvent& event)
    {
	event.Skip();
    }

public:
    wxPanel* GetAnchorListPlaceholder()
    {
	return m_anchorListPlaceholder;
    }
    wxButton* GetBtAdd()
    {
	return m_btAdd;
    }
    wxButton* GetBtMod()
    {
	return m_btMod;
    }
    wxButton* GetBtDel()
    {
	return m_btDel;
    }
    wxStaticLine* GetStaticLine585()
    {
	return m_staticLine585;
    }
    wxButton* GetBtDel9()
    {
	return m_btDel9;
    }
    wxStaticLine* GetStaticLine577()
    {
	return m_staticLine577;
    }
    wxStaticText* GetStaticText555()
    {
	return m_staticText555;
    }
    wxTextCtrl* GetValN()
    {
	return m_valN;
    }
    wxStaticText* GetStaticText593()
    {
	return m_staticText593;
    }
    wxComboBox* GetValC()
    {
	return m_valC;
    }
    wxStaticText* GetStaticText579()
    {
	return m_staticText579;
    }
    wxCheckBox* GetValS()
    {
	return m_valS;
    }
    wxStaticText* GetStaticText4711()
    {
	return m_staticText4711;
    }
    wxComboBox* GetValT()
    {
	return m_valT;
    }
    wxStaticText* GetStaticText5392()
    {
	return m_staticText5392;
    }
    wxTextCtrl* GetValX()
    {
	return m_valX;
    }
    wxStaticText* GetStaticText5396()
    {
	return m_staticText5396;
    }
    wxTextCtrl* GetValY()
    {
	return m_valY;
    }
    wxStaticText* GetStaticText539()
    {
	return m_staticText539;
    }
    wxTextCtrl* GetValZ()
    {
	return m_valZ;
    }
    wxStaticText* GetStaticText543()
    {
	return m_staticText543;
    }
    CncAnchorPositionBase(wxWindow* parent,
        wxWindowID id = wxID_ANY,
        const wxString& title = _("Cnc Anchor Position"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(-1, -1),
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~CncAnchorPositionBase();
};

class CncReferencePositionBase : public wxDialog
{
protected:
    wxInfoBar* m_infobar;
    wxNotebook* m_notebooMain;
    wxPanel* m_panelRef;
    wxPanel* m_evaluateReferencePlaceholder;
    wxPanel* m_panelNav;
    wxStaticText* m_staticText5505;
    wxStaticLine* m_staticLine5357;
    wxStaticText* m_staticText53555;
    wxRadioBox* m_rbStepSensitivity;
    wxStaticText* m_staticText5355;
    wxRadioBox* m_rbStepMode;
    wxPanel* m_navigationPanelPlaceholder;
    wxStaticLine* m_staticLine518;
    wxButton* m_btSet;
    wxButton* m_btCancel;
    wxTimer* m_infoTimer;

protected:
    virtual void init(wxInitDialogEvent& event)
    {
	event.Skip();
    }
    virtual void show(wxShowEvent& event)
    {
	event.Skip();
    }
    virtual void selectStepSensitivity(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void onSetCtrlFocus(wxFocusEvent& event)
    {
	event.Skip();
    }
    virtual void onKillCtrlFocus(wxFocusEvent& event)
    {
	event.Skip();
    }
    virtual void selectStepMode(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void set(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void cancel(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void onInfoTimer(wxTimerEvent& event)
    {
	event.Skip();
    }

public:
    wxInfoBar* GetInfobar()
    {
	return m_infobar;
    }
    wxPanel* GetEvaluateReferencePlaceholder()
    {
	return m_evaluateReferencePlaceholder;
    }
    wxPanel* GetPanelRef()
    {
	return m_panelRef;
    }
    wxStaticText* GetStaticText5505()
    {
	return m_staticText5505;
    }
    wxStaticLine* GetStaticLine5357()
    {
	return m_staticLine5357;
    }
    wxStaticText* GetStaticText53555()
    {
	return m_staticText53555;
    }
    wxRadioBox* GetRbStepSensitivity()
    {
	return m_rbStepSensitivity;
    }
    wxStaticText* GetStaticText5355()
    {
	return m_staticText5355;
    }
    wxRadioBox* GetRbStepMode()
    {
	return m_rbStepMode;
    }
    wxPanel* GetNavigationPanelPlaceholder()
    {
	return m_navigationPanelPlaceholder;
    }
    wxPanel* GetPanelNav()
    {
	return m_panelNav;
    }
    wxNotebook* GetNotebooMain()
    {
	return m_notebooMain;
    }
    wxStaticLine* GetStaticLine518()
    {
	return m_staticLine518;
    }
    wxButton* GetBtSet()
    {
	return m_btSet;
    }
    wxButton* GetBtCancel()
    {
	return m_btCancel;
    }
    wxTimer* GetInfoTimer()
    {
	return m_infoTimer;
    }
    CncReferencePositionBase(wxWindow* parent,
        wxWindowID id = wxID_ANY,
        const wxString& title = _("CNC Reference Position (0, 0, 0)"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(-1, -1),
        long style = wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);
    virtual ~CncReferencePositionBase();
};

class CncReferenceEvaluationBase : public wxPanel
{
protected:
    wxStaticText* m_staticText534011;
    wxListbook* m_notebookEvalMode;
    wxPanel* m_panelTouchblock;
    wxStaticText* m_staticText470;
    wxBitmapButton* m_btTouchCorner;
    wxStaticText* m_staticText1695;
    wxStaticText* m_staticText1717;
    wxTextCtrl* m_touchDiameter;
    wxStaticText* m_staticText289;
    wxToggleButton* m_tbPrevTest;
    wxStaticLine* m_staticLine3106;
    wxStaticText* m_staticText304;
    wxButton* m_btTouchTest;
    wxStaticLine* m_staticLine310;
    wxStaticText* m_staticText306;
    wxButton* m_btTouchZ;
    wxStaticLine* m_staticLine312;
    wxStaticText* m_staticText308;
    wxButton* m_btTouchXYZ;
    wxPanel* m_panelCamera;
    wxPanel* m_panelCameraArea;
    wxStaticText* m_staticText849;
    wxPanel* m_cameraCapturePlaceholder;
    wxStaticLine* m_staticLine513;
    wxBitmapButton* m_btDetachCamera;
    wxBitmapToggleButton* m_btShowCrossHV;
    wxBitmapToggleButton* m_btShowCrossDD;
    wxBitmapToggleButton* m_btShowRect;
    wxPanel* m_panelByEye;
    wxStaticText* m_staticText5340;
    wxStaticText* m_staticText5460;
    wxBitmapToggleButton* m_btMode1;
    wxStaticText* m_staticText5467;
    wxBitmapToggleButton* m_btMode2;
    wxStaticText* m_staticText5469;
    wxBitmapToggleButton* m_btMode3;
    wxStaticText* m_staticText5471;
    wxBitmapToggleButton* m_btMode4;
    wxStaticText* m_staticText5473;
    wxBitmapToggleButton* m_btMode5;
    wxStaticText* m_staticText5475;
    wxBitmapToggleButton* m_btMode6;
    wxStaticLine* m_staticLine4525;
    wxStaticText* m_modeSelected;
    wxStaticText* m_modeText;
    wxStaticLine* m_staticLine452;
    wxStaticText* m_staticText5486;
    wxTextCtrl* m_workpiceThickness;
    wxStaticText* m_staticText5497;
    wxStaticText* m_staticText5488;
    wxTextCtrl* m_measuremetOffsetX;
    wxTextCtrl* m_measuremetOffsetY;
    wxTextCtrl* m_measuremetOffsetZ;
    wxStaticText* m_staticText5499;
    wxStaticLine* m_staticLine825;
    wxPanel* m_panel521;
    wxStaticText* m_staticText132;
    wxStaticText* m_staticText137;
    wxTextCtrl* m_previewX;
    wxStaticText* m_staticText1371;
    wxTextCtrl* m_previewY;
    wxStaticText* m_staticText13712;
    wxTextCtrl* m_previewZ;
    wxStaticText* m_staticText137123;
    wxStaticLine* m_staticLine74384;
    wxStaticText* m_staticText7442;
    wxStaticText* m_staticText7444;
    wxStaticText* m_staticText7446;
    wxBitmapToggleButton* m_btZeroX;
    wxBitmapToggleButton* m_btZeroY;
    wxBitmapToggleButton* m_btZeroZ;
    wxStaticLine* m_staticLine7438;
    wxTimer* m_continuousTimer;

protected:
    virtual void onSelectEvaluationMode(wxListbookEvent& event)
    {
	event.Skip();
    }
    virtual void onSelectTouchCorner(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void onTouchDiameterKeyDown(wxKeyEvent& event)
    {
	event.Skip();
    }
    virtual void onTouchDiameterLeftDown(wxMouseEvent& event)
    {
	event.Skip();
    }
    virtual void onTogglePrevTest(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void onTouchTest(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void onTouchZ(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void onTouchXYZ(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void detachCameraCapture(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void updateCameraParameter(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void mode1(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void mode2(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void mode3(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void mode4(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void mode5(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void mode6(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void updateParameter(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void toggleZeroX(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void toggleZeroY(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void toggleZeroZ(wxCommandEvent& event)
    {
	event.Skip();
    }
    virtual void onContinuousTimer(wxTimerEvent& event)
    {
	event.Skip();
    }

public:
    wxStaticText* GetStaticText534011()
    {
	return m_staticText534011;
    }
    wxStaticText* GetStaticText470()
    {
	return m_staticText470;
    }
    wxBitmapButton* GetBtTouchCorner()
    {
	return m_btTouchCorner;
    }
    wxStaticText* GetStaticText1695()
    {
	return m_staticText1695;
    }
    wxStaticText* GetStaticText1717()
    {
	return m_staticText1717;
    }
    wxTextCtrl* GetTouchDiameter()
    {
	return m_touchDiameter;
    }
    wxStaticText* GetStaticText289()
    {
	return m_staticText289;
    }
    wxToggleButton* GetTbPrevTest()
    {
	return m_tbPrevTest;
    }
    wxStaticLine* GetStaticLine3106()
    {
	return m_staticLine3106;
    }
    wxStaticText* GetStaticText304()
    {
	return m_staticText304;
    }
    wxButton* GetBtTouchTest()
    {
	return m_btTouchTest;
    }
    wxStaticLine* GetStaticLine310()
    {
	return m_staticLine310;
    }
    wxStaticText* GetStaticText306()
    {
	return m_staticText306;
    }
    wxButton* GetBtTouchZ()
    {
	return m_btTouchZ;
    }
    wxStaticLine* GetStaticLine312()
    {
	return m_staticLine312;
    }
    wxStaticText* GetStaticText308()
    {
	return m_staticText308;
    }
    wxButton* GetBtTouchXYZ()
    {
	return m_btTouchXYZ;
    }
    wxPanel* GetPanelTouchblock()
    {
	return m_panelTouchblock;
    }
    wxStaticText* GetStaticText849()
    {
	return m_staticText849;
    }
    wxPanel* GetCameraCapturePlaceholder()
    {
	return m_cameraCapturePlaceholder;
    }
    wxStaticLine* GetStaticLine513()
    {
	return m_staticLine513;
    }
    wxBitmapButton* GetBtDetachCamera()
    {
	return m_btDetachCamera;
    }
    wxBitmapToggleButton* GetBtShowCrossHV()
    {
	return m_btShowCrossHV;
    }
    wxBitmapToggleButton* GetBtShowCrossDD()
    {
	return m_btShowCrossDD;
    }
    wxBitmapToggleButton* GetBtShowRect()
    {
	return m_btShowRect;
    }
    wxPanel* GetPanelCameraArea()
    {
	return m_panelCameraArea;
    }
    wxPanel* GetPanelCamera()
    {
	return m_panelCamera;
    }
    wxStaticText* GetStaticText5340()
    {
	return m_staticText5340;
    }
    wxStaticText* GetStaticText5460()
    {
	return m_staticText5460;
    }
    wxBitmapToggleButton* GetBtMode1()
    {
	return m_btMode1;
    }
    wxStaticText* GetStaticText5467()
    {
	return m_staticText5467;
    }
    wxBitmapToggleButton* GetBtMode2()
    {
	return m_btMode2;
    }
    wxStaticText* GetStaticText5469()
    {
	return m_staticText5469;
    }
    wxBitmapToggleButton* GetBtMode3()
    {
	return m_btMode3;
    }
    wxStaticText* GetStaticText5471()
    {
	return m_staticText5471;
    }
    wxBitmapToggleButton* GetBtMode4()
    {
	return m_btMode4;
    }
    wxStaticText* GetStaticText5473()
    {
	return m_staticText5473;
    }
    wxBitmapToggleButton* GetBtMode5()
    {
	return m_btMode5;
    }
    wxStaticText* GetStaticText5475()
    {
	return m_staticText5475;
    }
    wxBitmapToggleButton* GetBtMode6()
    {
	return m_btMode6;
    }
    wxStaticLine* GetStaticLine4525()
    {
	return m_staticLine4525;
    }
    wxStaticText* GetModeSelected()
    {
	return m_modeSelected;
    }
    wxStaticText* GetModeText()
    {
	return m_modeText;
    }
    wxStaticLine* GetStaticLine452()
    {
	return m_staticLine452;
    }
    wxPanel* GetPanelByEye()
    {
	return m_panelByEye;
    }
    wxListbook* GetNotebookEvalMode()
    {
	return m_notebookEvalMode;
    }
    wxStaticText* GetStaticText5486()
    {
	return m_staticText5486;
    }
    wxTextCtrl* GetWorkpiceThickness()
    {
	return m_workpiceThickness;
    }
    wxStaticText* GetStaticText5497()
    {
	return m_staticText5497;
    }
    wxStaticText* GetStaticText5488()
    {
	return m_staticText5488;
    }
    wxTextCtrl* GetMeasuremetOffsetX()
    {
	return m_measuremetOffsetX;
    }
    wxTextCtrl* GetMeasuremetOffsetY()
    {
	return m_measuremetOffsetY;
    }
    wxTextCtrl* GetMeasuremetOffsetZ()
    {
	return m_measuremetOffsetZ;
    }
    wxStaticText* GetStaticText5499()
    {
	return m_staticText5499;
    }
    wxStaticLine* GetStaticLine825()
    {
	return m_staticLine825;
    }
    wxPanel* GetPanel521()
    {
	return m_panel521;
    }
    wxStaticText* GetStaticText132()
    {
	return m_staticText132;
    }
    wxStaticText* GetStaticText137()
    {
	return m_staticText137;
    }
    wxTextCtrl* GetPreviewX()
    {
	return m_previewX;
    }
    wxStaticText* GetStaticText1371()
    {
	return m_staticText1371;
    }
    wxTextCtrl* GetPreviewY()
    {
	return m_previewY;
    }
    wxStaticText* GetStaticText13712()
    {
	return m_staticText13712;
    }
    wxTextCtrl* GetPreviewZ()
    {
	return m_previewZ;
    }
    wxStaticText* GetStaticText137123()
    {
	return m_staticText137123;
    }
    wxStaticLine* GetStaticLine74384()
    {
	return m_staticLine74384;
    }
    wxStaticText* GetStaticText7442()
    {
	return m_staticText7442;
    }
    wxStaticText* GetStaticText7444()
    {
	return m_staticText7444;
    }
    wxStaticText* GetStaticText7446()
    {
	return m_staticText7446;
    }
    wxBitmapToggleButton* GetBtZeroX()
    {
	return m_btZeroX;
    }
    wxBitmapToggleButton* GetBtZeroY()
    {
	return m_btZeroY;
    }
    wxBitmapToggleButton* GetBtZeroZ()
    {
	return m_btZeroZ;
    }
    wxStaticLine* GetStaticLine7438()
    {
	return m_staticLine7438;
    }
    wxTimer* GetContinuousTimer()
    {
	return m_continuousTimer;
    }
    CncReferenceEvaluationBase(wxWindow* parent,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(-1, -1),
        long style = wxTAB_TRAVERSAL);
    virtual ~CncReferenceEvaluationBase();
};

#endif
