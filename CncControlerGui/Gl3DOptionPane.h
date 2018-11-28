#ifndef GL3DOPTIONPANE_H
#define GL3DOPTIONPANE_H
#include "wxcrafter.h"

class GL3DOptionPane : public GL3DOptionPaneBase
{
public:
    GL3DOptionPane(wxWindow* parent);
    virtual ~GL3DOptionPane();
protected:
    virtual void propertyChanged(wxPropertyGridEvent& event);
    virtual void propertyChanging(wxPropertyGridEvent& event);
};
#endif // GL3DOPTIONPANE_H
