//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxCrafterSecurePanel.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CNCGUICONTROLLER_CNCCONTROLERGUI_WXCRAFTERSECUREPANEL_BASE_CLASSES_H
#define _CNCGUICONTROLLER_CNCCONTROLERGUI_WXCRAFTERSECUREPANEL_BASE_CLASSES_H

#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/panel.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statbmp.h>
#include <wx/statline.h>
#include <wx/listbook.h>
#include <wx/imaglist.h>
#include <wx/simplebook.h>
#include <wx/button.h>
#include <wx/infobar.h>
#include <wx/tglbtn.h>
#include <wx/textctrl.h>
#include <wx/dialog.h>
#include <wx/iconbndl.h>
#include <wx/bitmap.h>
#include <map>
#include <wx/icon.h>
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


class CncSecureCtrlPanelBase : public wxPanel
{
protected:
    wxStaticText* m_staticText433;
    wxStaticText* m_portName;
    wxStaticBitmap* m_bmpConnectionStateSecure;
    wxStaticLine* m_staticLine501;
    wxListbook* m_leftBook;
    wxPanel* m_lpConnect;
    wxPanel* m_lpLoad;
    wxPanel* m_lpRef;
    wxPanel* m_lpManually;
    wxPanel* m_lpRun;
    wxPanel* m_lpCtrl;
    wxPanel* m_plPodest;
    wxPanel* m_lpMisc;
    wxStaticText* m_rightHeadline;
    wxSimplebook* m_rightBook;
    wxPanel* m_rpConnect;
    wxStaticText* m_staticText71;
    wxPanel* m_portSelectorPlaceholder;
    wxButton* m_btConnectSec;
    wxButton* m_btResetSec;
    wxStaticLine* m_staticLine446;
    wxButton* m_btHardwareRefSec;
    wxPanel* m_rpLoad;
    wxStaticText* m_staticText711;
    wxPanel* m_transferDirPlaceholder;
    wxPanel* m_lruFilePlaceholder;
    wxButton* m_btLoadTemplate;
    wxPanel* m_rpRef;
    wxInfoBar* m_referenceInfobar;
    wxPanel* m_evaluateReferencePlaceholder;
    wxButton* m_btSet;
    wxPanel* m_rpManually;
    wxListbook* m_listbook435;
    wxPanel* m_panelCMD;
    wxPanel* m_manuallyMovePlaceholder;
    wxPanel* m_panelIM;
    wxStaticText* m_staticText521;
    wxStaticText* m_staticText523;
    wxStaticText* m_staticText525;
    wxPanel* m_interactiveMoveXPlaceholder;
    wxPanel* m_interactiveMoveYPlaceholder;
    wxPanel* m_interactiveMoveZPlaceholder;
    wxStaticText* m_staticText5211;
    wxStaticText* m_staticText5233;
    wxStaticText* m_staticText5254;
    wxPanel* m_speedSliderPlaceholder;
    wxPanel* m_panelITP;
    wxToggleButton* m_toggleButton554;
    wxPanel* m_interactiveTouchpadXYZ;
    wxPanel* m_panelPD;
    wxPanel* m_predefinedPositionsPlaceholder;
    wxPanel* m_panelPA;
    wxStaticText* m_staticText600;
    wxPanel* m_panelQ1;
    wxPanel* m_panelQ2;
    wxPanel* m_panelQ3;
    wxPanel* m_panelQ4;
    wxPanel* m_panelQ5;
    wxPanel* m_panelQ6;
    wxPanel* m_rpRun;
    wxButton* m_rcRunSec;
    wxButton* m_rcPauseSec;
    wxButton* m_rcStopSec;
    wxStaticText* m_staticText90;
    wxButton* m_btnEmergenyStopSec;
    wxPanel* m_rpCtrl;
    wxButton* m_resolveLimit;
    wxPanel* m_rpPodest;
    wxPanel* m_podestPlaceholder;
    wxPanel* m_rpMisc;
    wxButton* m_btOSEnvrionment;
    wxButton* m_btSessionDir;
    wxButton* m_btSerialSpy;
    wxButton* m_btStackTrace;
    wxButton* m_btTestFunct;
    wxButton* m_btAnchorSec;
    wxStaticLine* m_staticLine5011;

protected:
    virtual void onLeftBookPageChanged(wxListbookEvent& event) { event.Skip(); }
    virtual void onConnectSec(wxCommandEvent& event) { event.Skip(); }
    virtual void onResetSec(wxCommandEvent& event) { event.Skip(); }
    virtual void onEvaluateHardwareReference(wxCommandEvent& event) { event.Skip(); }
    virtual void onOpenTemplateSec(wxCommandEvent& event) { event.Skip(); }
    virtual void onReferenceSet(wxCommandEvent& event) { event.Skip(); }
    virtual void onToggleTouchpadPane(wxCommandEvent& event) { event.Skip(); }
    virtual void onRunSec(wxCommandEvent& event) { event.Skip(); }
    virtual void onPauseSec(wxCommandEvent& event) { event.Skip(); }
    virtual void onStopSec(wxCommandEvent& event) { event.Skip(); }
    virtual void onEmergencySec(wxCommandEvent& event) { event.Skip(); }
    virtual void requestResolveLimitStates(wxCommandEvent& event) { event.Skip(); }
    virtual void onAppEnvironmentSec(wxCommandEvent& event) { event.Skip(); }
    virtual void onSessionDirSec(wxCommandEvent& event) { event.Skip(); }
    virtual void onSerialSpySec(wxCommandEvent& event) { event.Skip(); }
    virtual void onStackTraceStoreSec(wxCommandEvent& event) { event.Skip(); }
    virtual void onTestFunctionSec(wxCommandEvent& event) { event.Skip(); }
    virtual void onDetermineAnchorPositionsSec(wxCommandEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticText433() { return m_staticText433; }
    wxStaticText* GetPortName() { return m_portName; }
    wxStaticBitmap* GetBmpConnectionStateSecure() { return m_bmpConnectionStateSecure; }
    wxStaticLine* GetStaticLine501() { return m_staticLine501; }
    wxPanel* GetLpConnect() { return m_lpConnect; }
    wxPanel* GetLpLoad() { return m_lpLoad; }
    wxPanel* GetLpRef() { return m_lpRef; }
    wxPanel* GetLpManually() { return m_lpManually; }
    wxPanel* GetLpRun() { return m_lpRun; }
    wxPanel* GetLpCtrl() { return m_lpCtrl; }
    wxPanel* GetPlPodest() { return m_plPodest; }
    wxPanel* GetLpMisc() { return m_lpMisc; }
    wxListbook* GetLeftBook() { return m_leftBook; }
    wxStaticText* GetRightHeadline() { return m_rightHeadline; }
    wxStaticText* GetStaticText71() { return m_staticText71; }
    wxPanel* GetPortSelectorPlaceholder() { return m_portSelectorPlaceholder; }
    wxButton* GetBtConnectSec() { return m_btConnectSec; }
    wxButton* GetBtResetSec() { return m_btResetSec; }
    wxStaticLine* GetStaticLine446() { return m_staticLine446; }
    wxButton* GetBtHardwareRefSec() { return m_btHardwareRefSec; }
    wxPanel* GetRpConnect() { return m_rpConnect; }
    wxStaticText* GetStaticText711() { return m_staticText711; }
    wxPanel* GetTransferDirPlaceholder() { return m_transferDirPlaceholder; }
    wxPanel* GetLruFilePlaceholder() { return m_lruFilePlaceholder; }
    wxButton* GetBtLoadTemplate() { return m_btLoadTemplate; }
    wxPanel* GetRpLoad() { return m_rpLoad; }
    wxInfoBar* GetReferenceInfobar() { return m_referenceInfobar; }
    wxPanel* GetEvaluateReferencePlaceholder() { return m_evaluateReferencePlaceholder; }
    wxButton* GetBtSet() { return m_btSet; }
    wxPanel* GetRpRef() { return m_rpRef; }
    wxPanel* GetManuallyMovePlaceholder() { return m_manuallyMovePlaceholder; }
    wxPanel* GetPanelCMD() { return m_panelCMD; }
    wxStaticText* GetStaticText521() { return m_staticText521; }
    wxStaticText* GetStaticText523() { return m_staticText523; }
    wxStaticText* GetStaticText525() { return m_staticText525; }
    wxPanel* GetInteractiveMoveXPlaceholder() { return m_interactiveMoveXPlaceholder; }
    wxPanel* GetInteractiveMoveYPlaceholder() { return m_interactiveMoveYPlaceholder; }
    wxPanel* GetInteractiveMoveZPlaceholder() { return m_interactiveMoveZPlaceholder; }
    wxStaticText* GetStaticText5211() { return m_staticText5211; }
    wxStaticText* GetStaticText5233() { return m_staticText5233; }
    wxStaticText* GetStaticText5254() { return m_staticText5254; }
    wxPanel* GetSpeedSliderPlaceholder() { return m_speedSliderPlaceholder; }
    wxPanel* GetPanelIM() { return m_panelIM; }
    wxToggleButton* GetToggleButton554() { return m_toggleButton554; }
    wxPanel* GetInteractiveTouchpadXYZ() { return m_interactiveTouchpadXYZ; }
    wxPanel* GetPanelITP() { return m_panelITP; }
    wxPanel* GetPredefinedPositionsPlaceholder() { return m_predefinedPositionsPlaceholder; }
    wxPanel* GetPanelPD() { return m_panelPD; }
    wxStaticText* GetStaticText600() { return m_staticText600; }
    wxPanel* GetPanelQ1() { return m_panelQ1; }
    wxPanel* GetPanelQ2() { return m_panelQ2; }
    wxPanel* GetPanelQ3() { return m_panelQ3; }
    wxPanel* GetPanelQ4() { return m_panelQ4; }
    wxPanel* GetPanelQ5() { return m_panelQ5; }
    wxPanel* GetPanelQ6() { return m_panelQ6; }
    wxPanel* GetPanelPA() { return m_panelPA; }
    wxListbook* GetListbook435() { return m_listbook435; }
    wxPanel* GetRpManually() { return m_rpManually; }
    wxButton* GetRcRunSec() { return m_rcRunSec; }
    wxButton* GetRcPauseSec() { return m_rcPauseSec; }
    wxButton* GetRcStopSec() { return m_rcStopSec; }
    wxStaticText* GetStaticText90() { return m_staticText90; }
    wxButton* GetBtnEmergenyStopSec() { return m_btnEmergenyStopSec; }
    wxPanel* GetRpRun() { return m_rpRun; }
    wxButton* GetResolveLimit() { return m_resolveLimit; }
    wxPanel* GetRpCtrl() { return m_rpCtrl; }
    wxPanel* GetPodestPlaceholder() { return m_podestPlaceholder; }
    wxPanel* GetRpPodest() { return m_rpPodest; }
    wxButton* GetBtOSEnvrionment() { return m_btOSEnvrionment; }
    wxButton* GetBtSessionDir() { return m_btSessionDir; }
    wxButton* GetBtSerialSpy() { return m_btSerialSpy; }
    wxButton* GetBtStackTrace() { return m_btStackTrace; }
    wxButton* GetBtTestFunct() { return m_btTestFunct; }
    wxButton* GetBtAnchorSec() { return m_btAnchorSec; }
    wxPanel* GetRpMisc() { return m_rpMisc; }
    wxSimplebook* GetRightBook() { return m_rightBook; }
    wxStaticLine* GetStaticLine5011() { return m_staticLine5011; }
    CncSecureCtrlPanelBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxTAB_TRAVERSAL);
    virtual ~CncSecureCtrlPanelBase();
};


class CncSecureManuallyMovePanelBase : public wxPanel
{
protected:
    wxToggleButton* m_btX;
    wxTextCtrl* m_valueX;
    wxTextCtrl* m_unitX;
    wxToggleButton* m_btY;
    wxTextCtrl* m_valueY;
    wxTextCtrl* m_unitY;
    wxToggleButton* m_btZ;
    wxTextCtrl* m_valueZ;
    wxTextCtrl* m_unitZ;
    wxToggleButton* m_btF;
    wxTextCtrl* m_valueF;
    wxTextCtrl* m_unitF;
    wxStaticLine* m_staticLine2823;
    wxToggleButton* m_bt1D;
    wxToggleButton* m_bt2D;
    wxToggleButton* m_bt3D;
    wxStaticText* m_staticText619;
    wxButton* m_btMove;
    wxStaticText* m_staticText622;

protected:
    virtual void onAxis(wxCommandEvent& event) { event.Skip(); }
    virtual void onClearX(wxMouseEvent& event) { event.Skip(); }
    virtual void onLeftDownResultValue(wxMouseEvent& event) { event.Skip(); }
    virtual void onClearY(wxMouseEvent& event) { event.Skip(); }
    virtual void onClearZ(wxMouseEvent& event) { event.Skip(); }
    virtual void onClearF(wxMouseEvent& event) { event.Skip(); }
    virtual void onSetDimMode(wxCommandEvent& event) { event.Skip(); }
    virtual void onMove(wxCommandEvent& event) { event.Skip(); }

public:
    wxToggleButton* GetBtX() { return m_btX; }
    wxTextCtrl* GetValueX() { return m_valueX; }
    wxTextCtrl* GetUnitX() { return m_unitX; }
    wxToggleButton* GetBtY() { return m_btY; }
    wxTextCtrl* GetValueY() { return m_valueY; }
    wxTextCtrl* GetUnitY() { return m_unitY; }
    wxToggleButton* GetBtZ() { return m_btZ; }
    wxTextCtrl* GetValueZ() { return m_valueZ; }
    wxTextCtrl* GetUnitZ() { return m_unitZ; }
    wxToggleButton* GetBtF() { return m_btF; }
    wxTextCtrl* GetValueF() { return m_valueF; }
    wxTextCtrl* GetUnitF() { return m_unitF; }
    wxStaticLine* GetStaticLine2823() { return m_staticLine2823; }
    wxToggleButton* GetBt1D() { return m_bt1D; }
    wxToggleButton* GetBt2D() { return m_bt2D; }
    wxToggleButton* GetBt3D() { return m_bt3D; }
    wxStaticText* GetStaticText619() { return m_staticText619; }
    wxButton* GetBtMove() { return m_btMove; }
    wxStaticText* GetStaticText622() { return m_staticText622; }
    CncSecureManuallyMovePanelBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxTAB_TRAVERSAL);
    virtual ~CncSecureManuallyMovePanelBase();
};


class CncSecureNumpadBase : public wxPanel
{
protected:
    wxStaticText* m_infoText;
    wxStaticText* m_intervalText;
    wxTextCtrl* m_signValue;
    wxTextCtrl* m_textResult;
    wxButton* m_bt7;
    wxButton* m_bt8;
    wxButton* m_bt9;
    wxButton* m_btLeft;
    wxButton* m_bt4;
    wxButton* m_bt5;
    wxButton* m_bt6;
    wxButton* m_btRight;
    wxButton* m_bt3;
    wxButton* m_bt2;
    wxButton* m_bt1;
    wxButton* m_btClear;
    wxButton* m_bt0;
    wxButton* m_btDot;
    wxButton* m_btSign;
    wxButton* m_btBks;

protected:
    virtual void onCharResult(wxKeyEvent& event) { event.Skip(); }
    virtual void onTextUpdatedResult(wxCommandEvent& event) { event.Skip(); }
    virtual void onContextMenuResult(wxContextMenuEvent& event) { event.Skip(); }
    virtual void onNumber(wxCommandEvent& event) { event.Skip(); }
    virtual void onLeft(wxCommandEvent& event) { event.Skip(); }
    virtual void onRight(wxCommandEvent& event) { event.Skip(); }
    virtual void onClear(wxCommandEvent& event) { event.Skip(); }
    virtual void onSign(wxCommandEvent& event) { event.Skip(); }
    virtual void onBackspace(wxCommandEvent& event) { event.Skip(); }

public:
    wxStaticText* GetInfoText() { return m_infoText; }
    wxStaticText* GetIntervalText() { return m_intervalText; }
    wxTextCtrl* GetSignValue() { return m_signValue; }
    wxTextCtrl* GetTextResult() { return m_textResult; }
    wxButton* GetBt7() { return m_bt7; }
    wxButton* GetBt8() { return m_bt8; }
    wxButton* GetBt9() { return m_bt9; }
    wxButton* GetBtLeft() { return m_btLeft; }
    wxButton* GetBt4() { return m_bt4; }
    wxButton* GetBt5() { return m_bt5; }
    wxButton* GetBt6() { return m_bt6; }
    wxButton* GetBtRight() { return m_btRight; }
    wxButton* GetBt3() { return m_bt3; }
    wxButton* GetBt2() { return m_bt2; }
    wxButton* GetBt1() { return m_bt1; }
    wxButton* GetBtClear() { return m_btClear; }
    wxButton* GetBt0() { return m_bt0; }
    wxButton* GetBtDot() { return m_btDot; }
    wxButton* GetBtSign() { return m_btSign; }
    wxButton* GetBtBks() { return m_btBks; }
    CncSecureNumpadBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500,300), long style = wxTAB_TRAVERSAL);
    virtual ~CncSecureNumpadBase();
};


class CncSecureSlidepadBase : public wxPanel
{
protected:
    wxStaticText* m_infoText;
    wxTextCtrl* m_textResult;
    wxButton* m_btMin;
    wxPanel* m_gesturePanelPlaceholder;
    wxButton* m_btMax;

protected:
    virtual void onLeftDownResult(wxMouseEvent& event) { event.Skip(); }
    virtual void onSkipToMin(wxCommandEvent& event) { event.Skip(); }
    virtual void onSkipToMax(wxCommandEvent& event) { event.Skip(); }

public:
    wxStaticText* GetInfoText() { return m_infoText; }
    wxTextCtrl* GetTextResult() { return m_textResult; }
    wxButton* GetBtMin() { return m_btMin; }
    wxPanel* GetGesturePanelPlaceholder() { return m_gesturePanelPlaceholder; }
    wxButton* GetBtMax() { return m_btMax; }
    CncSecureSlidepadBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxTAB_TRAVERSAL);
    virtual ~CncSecureSlidepadBase();
};


class CncSecureRotateModelPanelBase : public wxPanel
{
protected:
    wxStaticText* m_staticText58036;
    wxPanel* m_zoomPlaceholder;
    wxStaticText* m_staticText5803;
    wxPanel* m_rotateXPlaceholder;
    wxStaticText* m_staticText58035;
    wxPanel* m_rotateYPlaceholder;
    wxStaticText* m_staticText580359;
    wxPanel* m_rotateZPlaceholder;

protected:

public:
    wxStaticText* GetStaticText58036() { return m_staticText58036; }
    wxPanel* GetZoomPlaceholder() { return m_zoomPlaceholder; }
    wxStaticText* GetStaticText5803() { return m_staticText5803; }
    wxPanel* GetRotateXPlaceholder() { return m_rotateXPlaceholder; }
    wxStaticText* GetStaticText58035() { return m_staticText58035; }
    wxPanel* GetRotateYPlaceholder() { return m_rotateYPlaceholder; }
    wxStaticText* GetStaticText580359() { return m_staticText580359; }
    wxPanel* GetRotateZPlaceholder() { return m_rotateZPlaceholder; }
    CncSecureRotateModelPanelBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(80,300), long style = wxTAB_TRAVERSAL);
    virtual ~CncSecureRotateModelPanelBase();
};


class CncSecureNumpadDialogBase : public wxDialog
{
protected:
    wxPanel* m_numpadPlaceholder;
    wxStaticLine* m_staticLine383;
    wxButton* m_btCancel;
    wxButton* m_btOk;

protected:
    virtual void onCloseWindow(wxCloseEvent& event) { event.Skip(); }
    virtual void onCancel(wxCommandEvent& event) { event.Skip(); }
    virtual void onOk(wxCommandEvent& event) { event.Skip(); }

public:
    wxPanel* GetNumpadPlaceholder() { return m_numpadPlaceholder; }
    wxStaticLine* GetStaticLine383() { return m_staticLine383; }
    wxButton* GetBtCancel() { return m_btCancel; }
    wxButton* GetBtOk() { return m_btOk; }
    CncSecureNumpadDialogBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Cnc Numpad"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP);
    virtual ~CncSecureNumpadDialogBase();
};


class CncSecureSlidepadDialogBase : public wxDialog
{
protected:
    wxPanel* m_slidepadPlaceholder;
    wxStaticLine* m_staticLine383;
    wxButton* m_btCancel;
    wxButton* m_btOk;

protected:
    virtual void onCloseWindow(wxCloseEvent& event) { event.Skip(); }
    virtual void onCancel(wxCommandEvent& event) { event.Skip(); }
    virtual void onOk(wxCommandEvent& event) { event.Skip(); }

public:
    wxPanel* GetSlidepadPlaceholder() { return m_slidepadPlaceholder; }
    wxStaticLine* GetStaticLine383() { return m_staticLine383; }
    wxButton* GetBtCancel() { return m_btCancel; }
    wxButton* GetBtOk() { return m_btOk; }
    CncSecureSlidepadDialogBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Cnc Sliderpad"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE);
    virtual ~CncSecureSlidepadDialogBase();
};


class ImageLibSecure : public wxImageList
{
protected:
    // Maintain a map of all bitmaps representd by their name
    std::map<wxString, wxBitmap> m_bitmaps;
    // The requested image resolution (can be one of @2x, @1.5x, @1.25x or an empty string (the default)
    wxString m_resolution;
    int m_imagesWidth;
    int m_imagesHeight;


protected:

public:
    ImageLibSecure();
    const wxBitmap& Bitmap(const wxString &name) const {
        if ( !m_bitmaps.count(name + m_resolution) )
            return wxNullBitmap;
        return m_bitmaps.find(name + m_resolution)->second;
    }

    void SetBitmapResolution(const wxString &res = wxEmptyString) {
        m_resolution = res;
    }

    virtual ~ImageLibSecure();
};

#endif
