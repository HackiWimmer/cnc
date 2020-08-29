//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxCrafterSpeedMonitor.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _CNCGUICONTROLLER_CNCCONTROLERGUI_WXCRAFTERSPEEDMONITOR_BASE_CLASSES_H
#define _CNCGUICONTROLLER_CNCCONTROLERGUI_WXCRAFTERSPEEDMONITOR_BASE_CLASSES_H

#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/panel.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/tglbtn.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/bmpbuttn.h>
#include <wx/combobox.h>
#include <wx/arrstr.h>
#include <wx/stattext.h>
#include <wx/slider.h>
#include <wx/simplebook.h>
#include <wx/imaglist.h>
#include <wx/scrolbar.h>
#include <wx/frame.h>
#include <wx/iconbndl.h>
#include <wx/splitter.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/toolbook.h>
#include <wx/listctrl.h>
#include <wx/timer.h>
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


class CncSpeedMonitorBase : public wxPanel
{
protected:
    wxBitmapToggleButton* m_btToggleConnection;
    wxStaticLine* m_staticLine75573;
    wxBitmapToggleButton* m_btToggleMeasurePointsAxis;
    wxBitmapToggleButton* m_btToggleConfiguredAxis;
    wxBitmapToggleButton* m_btToggleReceivedSpeedAxis;
    wxStaticLine* m_staticLine7557;
    wxBitmapButton* m_btClear;
    wxBitmapButton* m_btSave;
    wxComboBox* m_cbTimeCompression;
    wxStaticText* m_staticText7534;
    wxSlider* m_intervalSlider;
    wxStaticText* m_staticText7614;
    wxSimplebook* m_drawingAreaBook;
    wxPanel* m_horizontalPanel;
    wxPanel* m_panel401;
    wxPanel* m_darwingAreaH;
    wxPanel* m_leftAxisH;
    wxPanel* m_rightAxisH;
    wxScrollBar* m_scrollBarH;
    wxPanel* m_verticalPanel;
    wxPanel* m_panel4011;
    wxPanel* m_darwingAreaV;
    wxPanel* m_topAxisV;
    wxPanel* m_bottomAxisV;
    wxScrollBar* m_scrollBarV;

protected:
    virtual void onToggleConnection(wxCommandEvent& event) { event.Skip(); }
    virtual void onToggleMeasurePointsAxis(wxCommandEvent& event) { event.Skip(); }
    virtual void onToggleConfiguredAxis(wxCommandEvent& event) { event.Skip(); }
    virtual void onToggleReceivedSpeedAxis(wxCommandEvent& event) { event.Skip(); }
    virtual void onClear(wxCommandEvent& event) { event.Skip(); }
    virtual void onSave(wxCommandEvent& event) { event.Skip(); }
    virtual void onChangeTimeCompression(wxCommandEvent& event) { event.Skip(); }
    virtual void onChangeIntervalSlider(wxScrollEvent& event) { event.Skip(); }
    virtual void onSize(wxSizeEvent& event) { event.Skip(); }
    virtual void onPaint(wxPaintEvent& event) { event.Skip(); }
    virtual void onMouseMotion(wxMouseEvent& event) { event.Skip(); }
    virtual void onLeftDown(wxMouseEvent& event) { event.Skip(); }
    virtual void onLeftUp(wxMouseEvent& event) { event.Skip(); }
    virtual void onPaintLeftAxis(wxPaintEvent& event) { event.Skip(); }
    virtual void onPaintRightAxis(wxPaintEvent& event) { event.Skip(); }
    virtual void onChangeScrollBarH(wxScrollEvent& event) { event.Skip(); }
    virtual void onChangeScrollBarV(wxScrollEvent& event) { event.Skip(); }

public:
    wxBitmapToggleButton* GetBtToggleConnection() { return m_btToggleConnection; }
    wxStaticLine* GetStaticLine75573() { return m_staticLine75573; }
    wxBitmapToggleButton* GetBtToggleMeasurePointsAxis() { return m_btToggleMeasurePointsAxis; }
    wxBitmapToggleButton* GetBtToggleConfiguredAxis() { return m_btToggleConfiguredAxis; }
    wxBitmapToggleButton* GetBtToggleReceivedSpeedAxis() { return m_btToggleReceivedSpeedAxis; }
    wxStaticLine* GetStaticLine7557() { return m_staticLine7557; }
    wxBitmapButton* GetBtClear() { return m_btClear; }
    wxBitmapButton* GetBtSave() { return m_btSave; }
    wxComboBox* GetCbTimeCompression() { return m_cbTimeCompression; }
    wxStaticText* GetStaticText7534() { return m_staticText7534; }
    wxSlider* GetIntervalSlider() { return m_intervalSlider; }
    wxStaticText* GetStaticText7614() { return m_staticText7614; }
    wxPanel* GetDarwingAreaH() { return m_darwingAreaH; }
    wxPanel* GetLeftAxisH() { return m_leftAxisH; }
    wxPanel* GetRightAxisH() { return m_rightAxisH; }
    wxScrollBar* GetScrollBarH() { return m_scrollBarH; }
    wxPanel* GetPanel401() { return m_panel401; }
    wxPanel* GetHorizontalPanel() { return m_horizontalPanel; }
    wxPanel* GetDarwingAreaV() { return m_darwingAreaV; }
    wxPanel* GetTopAxisV() { return m_topAxisV; }
    wxPanel* GetBottomAxisV() { return m_bottomAxisV; }
    wxScrollBar* GetScrollBarV() { return m_scrollBarV; }
    wxPanel* GetPanel4011() { return m_panel4011; }
    wxPanel* GetVerticalPanel() { return m_verticalPanel; }
    wxSimplebook* GetDrawingAreaBook() { return m_drawingAreaBook; }
    CncSpeedMonitorBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(600,300), long style = wxTAB_TRAVERSAL);
    virtual ~CncSpeedMonitorBase();
};


class CncSpeedPlaygroundBase : public wxFrame
{
protected:
    wxPanel* m_tollbarPanel;
    wxButton* m_button2618;
    wxStaticLine* m_staticLine263;
    wxButton* m_button261;
    wxSplitterWindow* m_mainSplitter;
    wxPanel* m_splTopPage;
    wxStaticText* m_staticText108;
    wxStaticLine* m_staticLine110;
    wxPanel* m_panel98;
    wxStaticText* m_staticText652;
    wxTextCtrl* m_valF;
    wxStaticText* m_staticText694;
    wxStaticText* m_staticMinF;
    wxSlider* m_sliderF;
    wxStaticText* m_staticMaxF;
    wxStaticText* m_staticText65;
    wxTextCtrl* m_valX;
    wxStaticText* m_staticText69;
    wxStaticText* m_staticMinX;
    wxSlider* m_sliderX;
    wxStaticText* m_staticMaxX;
    wxStaticText* m_staticText658;
    wxTextCtrl* m_valY;
    wxStaticText* m_staticText6910;
    wxStaticText* m_staticMinY;
    wxSlider* m_sliderY;
    wxStaticText* m_staticMaxY;
    wxStaticText* m_staticText65814;
    wxTextCtrl* m_valZ;
    wxStaticText* m_staticText691016;
    wxStaticText* m_staticMinZ;
    wxSlider* m_sliderZ;
    wxStaticText* m_staticMaxZ;
    wxStaticText* m_staticText313;
    wxStaticLine* m_staticLine14126;
    wxStaticText* m_staticText318;
    wxTextCtrl* m_ebSpecialTriggerImpluse1;
    wxStaticText* m_staticText320;
    wxTextCtrl* m_ebSpecialTriggerSpeed1;
    wxStaticText* m_staticText326;
    wxStaticText* m_staticText3188;
    wxTextCtrl* m_ebSpecialTriggerImpluse2;
    wxStaticText* m_staticText32010;
    wxTextCtrl* m_ebSpecialTriggerSpeed2;
    wxStaticText* m_staticText32613;
    wxStaticText* m_staticText318815;
    wxTextCtrl* m_ebSpecialTriggerImpluse3;
    wxStaticText* m_staticText3201018;
    wxTextCtrl* m_ebSpecialTriggerSpeed3;
    wxStaticText* m_staticText3261320;
    wxStaticText* m_staticText14711;
    wxStaticLine* m_staticLine1412;
    wxStaticText* m_staticText230385;
    wxTextCtrl* m_valAA;
    wxStaticText* m_staticText23038;
    wxTextCtrl* m_valAB;
    wxStaticText* m_staticText23010;
    wxTextCtrl* m_valAC;
    wxStaticText* m_staticText230103;
    wxStaticText* m_staticText1471;
    wxStaticLine* m_staticLine14122;
    wxStaticText* m_staticText23036;
    wxTextCtrl* m_valDA;
    wxStaticText* m_staticText2303;
    wxTextCtrl* m_valDB;
    wxStaticText* m_staticText230;
    wxTextCtrl* m_valDC;
    wxStaticText* m_staticText2304;
    wxStaticText* m_staticText147;
    wxStaticLine* m_staticLine141;
    wxCheckBox* m_cbWithAcceleration;
    wxCheckBox* m_cbSpeedRelative;
    wxCheckBox* m_cbEnhancedTracing;
    wxCheckBox* m_cbRealtime;
    wxCheckBox* m_cbSpecialSpeedDisplayHeight;
    wxTextCtrl* m_ebSpecialSpeedDisplayHeightValue;
    wxStaticText* m_staticText302;
    wxCheckBox* m_cbSpecialAccelDisplayWidth;
    wxTextCtrl* m_ebSpecialAccelDisplayWidthValue;
    wxStaticText* m_staticText3025;
    wxStaticLine* m_staticLine294;
    wxStaticText* m_staticText195;
    wxBitmapToggleButton* m_btExpandImpulse;
    wxPanel* m_panelStreach;
    wxStaticText* m_staticText246;
    wxComboBox* m_cbDisplayTypeV;
    wxStaticLine* m_staticLine204;
    wxPanel* m_accelerationDiagramPlaceholder;
    wxPanel* m_deaccelerationDiagramPlaceholder;
    wxPanel* m_splBottomPage;
    wxSplitterWindow* m_splitterBottom;
    wxPanel* m_splitterPageLogger;
    wxStaticText* m_staticText198;
    wxStaticLine* m_staticLine202;
    wxTextCtrl* m_loggerPlaceholder;
    wxPanel* m_splitterPageParameter;
    wxStaticText* m_staticText114;
    wxStaticLine* m_staticLine116;
    wxToolbook* m_nbCalculations;
    wxPanel* m_panel169;
    wxListCtrl* m_listCtrlTestData;
    wxPanel* m_panelAxes;
    wxStaticText* m_staticText161;
    wxListCtrl* m_listCtrlAxisX;
    wxStaticText* m_staticText1612733;
    wxListCtrl* m_listCtrlAxisY;
    wxStaticText* m_staticText16127;
    wxListCtrl* m_listCtrlAxisZ;
    wxPanel* m_statusPanel;
    wxPanel* m_panel283;
    wxStaticText* m_staticText278;
    wxTextCtrl* m_lastTimestamp;
    wxTimer* m_continuousTimer;

protected:
    virtual void onActivate(wxActivateEvent& event) { event.Skip(); }
    virtual void onRun(wxCommandEvent& event) { event.Skip(); }
    virtual void onClearLogger(wxCommandEvent& event) { event.Skip(); }
    virtual void updateValueF(wxCommandEvent& event) { event.Skip(); }
    virtual void onValKeyDown(wxKeyEvent& event) { event.Skip(); }
    virtual void changeSliderF(wxScrollEvent& event) { event.Skip(); }
    virtual void updateValueX(wxCommandEvent& event) { event.Skip(); }
    virtual void changeSliderX(wxScrollEvent& event) { event.Skip(); }
    virtual void updateValueY(wxCommandEvent& event) { event.Skip(); }
    virtual void changeSliderY(wxScrollEvent& event) { event.Skip(); }
    virtual void updateValueZ(wxCommandEvent& event) { event.Skip(); }
    virtual void changeSliderZ(wxScrollEvent& event) { event.Skip(); }
    virtual void updateTriggerI1(wxCommandEvent& event) { event.Skip(); }
    virtual void updateTriggerF1(wxCommandEvent& event) { event.Skip(); }
    virtual void updateTriggerI2(wxCommandEvent& event) { event.Skip(); }
    virtual void updateTriggerF2(wxCommandEvent& event) { event.Skip(); }
    virtual void updateTriggerI3(wxCommandEvent& event) { event.Skip(); }
    virtual void updateTriggerF3(wxCommandEvent& event) { event.Skip(); }
    virtual void enableSpecialSpeedDisplayHeight(wxCommandEvent& event) { event.Skip(); }
    virtual void updateSpecialSpeedDisplayHeightValue(wxCommandEvent& event) { event.Skip(); }
    virtual void enableSpecialAccelDisplayWidth(wxCommandEvent& event) { event.Skip(); }
    virtual void updateSpecialAccelDisplayWidthValue(wxCommandEvent& event) { event.Skip(); }
    virtual void onChangeDisplayType(wxCommandEvent& event) { event.Skip(); }
    virtual void onTimer(wxTimerEvent& event) { event.Skip(); }

public:
    wxButton* GetButton2618() { return m_button2618; }
    wxStaticLine* GetStaticLine263() { return m_staticLine263; }
    wxButton* GetButton261() { return m_button261; }
    wxPanel* GetTollbarPanel() { return m_tollbarPanel; }
    wxStaticText* GetStaticText108() { return m_staticText108; }
    wxStaticLine* GetStaticLine110() { return m_staticLine110; }
    wxStaticText* GetStaticText652() { return m_staticText652; }
    wxTextCtrl* GetValF() { return m_valF; }
    wxStaticText* GetStaticText694() { return m_staticText694; }
    wxStaticText* GetStaticMinF() { return m_staticMinF; }
    wxSlider* GetSliderF() { return m_sliderF; }
    wxStaticText* GetStaticMaxF() { return m_staticMaxF; }
    wxStaticText* GetStaticText65() { return m_staticText65; }
    wxTextCtrl* GetValX() { return m_valX; }
    wxStaticText* GetStaticText69() { return m_staticText69; }
    wxStaticText* GetStaticMinX() { return m_staticMinX; }
    wxSlider* GetSliderX() { return m_sliderX; }
    wxStaticText* GetStaticMaxX() { return m_staticMaxX; }
    wxStaticText* GetStaticText658() { return m_staticText658; }
    wxTextCtrl* GetValY() { return m_valY; }
    wxStaticText* GetStaticText6910() { return m_staticText6910; }
    wxStaticText* GetStaticMinY() { return m_staticMinY; }
    wxSlider* GetSliderY() { return m_sliderY; }
    wxStaticText* GetStaticMaxY() { return m_staticMaxY; }
    wxStaticText* GetStaticText65814() { return m_staticText65814; }
    wxTextCtrl* GetValZ() { return m_valZ; }
    wxStaticText* GetStaticText691016() { return m_staticText691016; }
    wxStaticText* GetStaticMinZ() { return m_staticMinZ; }
    wxSlider* GetSliderZ() { return m_sliderZ; }
    wxStaticText* GetStaticMaxZ() { return m_staticMaxZ; }
    wxStaticText* GetStaticText313() { return m_staticText313; }
    wxStaticLine* GetStaticLine14126() { return m_staticLine14126; }
    wxStaticText* GetStaticText318() { return m_staticText318; }
    wxTextCtrl* GetEbSpecialTriggerImpluse1() { return m_ebSpecialTriggerImpluse1; }
    wxStaticText* GetStaticText320() { return m_staticText320; }
    wxTextCtrl* GetEbSpecialTriggerSpeed1() { return m_ebSpecialTriggerSpeed1; }
    wxStaticText* GetStaticText326() { return m_staticText326; }
    wxStaticText* GetStaticText3188() { return m_staticText3188; }
    wxTextCtrl* GetEbSpecialTriggerImpluse2() { return m_ebSpecialTriggerImpluse2; }
    wxStaticText* GetStaticText32010() { return m_staticText32010; }
    wxTextCtrl* GetEbSpecialTriggerSpeed2() { return m_ebSpecialTriggerSpeed2; }
    wxStaticText* GetStaticText32613() { return m_staticText32613; }
    wxStaticText* GetStaticText318815() { return m_staticText318815; }
    wxTextCtrl* GetEbSpecialTriggerImpluse3() { return m_ebSpecialTriggerImpluse3; }
    wxStaticText* GetStaticText3201018() { return m_staticText3201018; }
    wxTextCtrl* GetEbSpecialTriggerSpeed3() { return m_ebSpecialTriggerSpeed3; }
    wxStaticText* GetStaticText3261320() { return m_staticText3261320; }
    wxStaticText* GetStaticText14711() { return m_staticText14711; }
    wxStaticLine* GetStaticLine1412() { return m_staticLine1412; }
    wxStaticText* GetStaticText230385() { return m_staticText230385; }
    wxTextCtrl* GetValAA() { return m_valAA; }
    wxStaticText* GetStaticText23038() { return m_staticText23038; }
    wxTextCtrl* GetValAB() { return m_valAB; }
    wxStaticText* GetStaticText23010() { return m_staticText23010; }
    wxTextCtrl* GetValAC() { return m_valAC; }
    wxStaticText* GetStaticText230103() { return m_staticText230103; }
    wxStaticText* GetStaticText1471() { return m_staticText1471; }
    wxStaticLine* GetStaticLine14122() { return m_staticLine14122; }
    wxStaticText* GetStaticText23036() { return m_staticText23036; }
    wxTextCtrl* GetValDA() { return m_valDA; }
    wxStaticText* GetStaticText2303() { return m_staticText2303; }
    wxTextCtrl* GetValDB() { return m_valDB; }
    wxStaticText* GetStaticText230() { return m_staticText230; }
    wxTextCtrl* GetValDC() { return m_valDC; }
    wxStaticText* GetStaticText2304() { return m_staticText2304; }
    wxStaticText* GetStaticText147() { return m_staticText147; }
    wxStaticLine* GetStaticLine141() { return m_staticLine141; }
    wxCheckBox* GetCbWithAcceleration() { return m_cbWithAcceleration; }
    wxCheckBox* GetCbSpeedRelative() { return m_cbSpeedRelative; }
    wxCheckBox* GetCbEnhancedTracing() { return m_cbEnhancedTracing; }
    wxCheckBox* GetCbRealtime() { return m_cbRealtime; }
    wxCheckBox* GetCbSpecialSpeedDisplayHeight() { return m_cbSpecialSpeedDisplayHeight; }
    wxTextCtrl* GetEbSpecialSpeedDisplayHeightValue() { return m_ebSpecialSpeedDisplayHeightValue; }
    wxStaticText* GetStaticText302() { return m_staticText302; }
    wxCheckBox* GetCbSpecialAccelDisplayWidth() { return m_cbSpecialAccelDisplayWidth; }
    wxTextCtrl* GetEbSpecialAccelDisplayWidthValue() { return m_ebSpecialAccelDisplayWidthValue; }
    wxStaticText* GetStaticText3025() { return m_staticText3025; }
    wxPanel* GetPanel98() { return m_panel98; }
    wxStaticLine* GetStaticLine294() { return m_staticLine294; }
    wxStaticText* GetStaticText195() { return m_staticText195; }
    wxBitmapToggleButton* GetBtExpandImpulse() { return m_btExpandImpulse; }
    wxPanel* GetPanelStreach() { return m_panelStreach; }
    wxStaticText* GetStaticText246() { return m_staticText246; }
    wxComboBox* GetCbDisplayTypeV() { return m_cbDisplayTypeV; }
    wxStaticLine* GetStaticLine204() { return m_staticLine204; }
    wxPanel* GetAccelerationDiagramPlaceholder() { return m_accelerationDiagramPlaceholder; }
    wxPanel* GetDeaccelerationDiagramPlaceholder() { return m_deaccelerationDiagramPlaceholder; }
    wxPanel* GetSplTopPage() { return m_splTopPage; }
    wxStaticText* GetStaticText198() { return m_staticText198; }
    wxStaticLine* GetStaticLine202() { return m_staticLine202; }
    wxTextCtrl* GetLoggerPlaceholder() { return m_loggerPlaceholder; }
    wxPanel* GetSplitterPageLogger() { return m_splitterPageLogger; }
    wxStaticText* GetStaticText114() { return m_staticText114; }
    wxStaticLine* GetStaticLine116() { return m_staticLine116; }
    wxListCtrl* GetListCtrlTestData() { return m_listCtrlTestData; }
    wxPanel* GetPanel169() { return m_panel169; }
    wxStaticText* GetStaticText161() { return m_staticText161; }
    wxListCtrl* GetListCtrlAxisX() { return m_listCtrlAxisX; }
    wxStaticText* GetStaticText1612733() { return m_staticText1612733; }
    wxListCtrl* GetListCtrlAxisY() { return m_listCtrlAxisY; }
    wxStaticText* GetStaticText16127() { return m_staticText16127; }
    wxListCtrl* GetListCtrlAxisZ() { return m_listCtrlAxisZ; }
    wxPanel* GetPanelAxes() { return m_panelAxes; }
    wxToolbook* GetNbCalculations() { return m_nbCalculations; }
    wxPanel* GetSplitterPageParameter() { return m_splitterPageParameter; }
    wxSplitterWindow* GetSplitterBottom() { return m_splitterBottom; }
    wxPanel* GetSplBottomPage() { return m_splBottomPage; }
    wxSplitterWindow* GetMainSplitter() { return m_mainSplitter; }
    wxPanel* GetPanel283() { return m_panel283; }
    wxStaticText* GetStaticText278() { return m_staticText278; }
    wxTextCtrl* GetLastTimestamp() { return m_lastTimestamp; }
    wxPanel* GetStatusPanel() { return m_statusPanel; }
    wxTimer* GetContinuousTimer() { return m_continuousTimer; }
    CncSpeedPlaygroundBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("CNC Speed & Acceleration Playground"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1000,1200), long style = wxDEFAULT_FRAME_STYLE);
    virtual ~CncSpeedPlaygroundBase();
};

#endif
