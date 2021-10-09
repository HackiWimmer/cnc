#ifndef CNCUSBCONNECTIONDETECTEDBASECNCUSBCONNECTIONDETECTED_H
#define CNCUSBCONNECTIONDETECTEDBASECNCUSBCONNECTIONDETECTED_H
#include "wxCrafterBaseDialogs.h"

class CncUsbConnectionDetectedBaseCncUsbConnectionDetected : public CncUsbConnectionDetectedBase
{
public:
    CncUsbConnectionDetectedBaseCncUsbConnectionDetected(wxWindow* parent);
    virtual ~CncUsbConnectionDetectedBaseCncUsbConnectionDetected();
protected:
    virtual void connect(wxCommandEvent& event);
    virtual void ignore(wxCommandEvent& event);
    virtual void initDilaog(wxInitDialogEvent& event);
};
#endif // CNCUSBCONNECTIONDETECTEDBASECNCUSBCONNECTIONDETECTED_H
