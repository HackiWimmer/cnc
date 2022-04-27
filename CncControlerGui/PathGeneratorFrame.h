#ifndef PATHGENERATORFRAME_H
#define PATHGENERATORFRAME_H
#include "wxCrafterPathGenerator.h"

class PathGeneratorFrame : public PathGeneratorFrameBase
{
public:
    PathGeneratorFrame(wxWindow* parent);
    virtual ~PathGeneratorFrame();
protected:
    virtual void checkAutoGenerate(wxCommandEvent& event);
    virtual void clearView(wxCommandEvent& event);
    virtual void copyPath(wxCommandEvent& event);
    virtual void generatePath(wxCommandEvent& event);
    virtual void keyDownTreeSearch(wxKeyEvent& event);
    virtual void mainShashPositionChanged( wxSplitterEvent& event);
    virtual void mainShashPositionChanging( wxSplitterEvent& event);
    virtual void onActivate(wxActivateEvent& event);
    virtual void onCloseWindow(wxCloseEvent& event);
    virtual void onCloseWindowFromButton(wxCommandEvent& event);
    virtual void onMaximize(wxMaximizeEvent& event);
    virtual void onShow(wxShowEvent& event);
    virtual void onSize(wxSizeEvent& event);
    virtual void pgMainBookChanged(wxNotebookEvent& event);
    virtual void propertyChanged(wxPropertyGridEvent& event);
    virtual void propertyChanging(wxPropertyGridEvent& event);
    virtual void relaceEditControlSelection(wxCommandEvent& event);
    virtual void saveTemplate(wxCommandEvent& event);
    virtual void selectDisplayMode(wxCommandEvent& event);
    virtual void selectEditMode(wxCommandEvent& event);
    virtual void selectPathSelector(wxCommandEvent& event);
    virtual void selectTemplateTree(wxTreeEvent& event);
    virtual void toogleSize(wxCommandEvent& event);
    virtual void toogleTreeSize(wxCommandEvent& event);
    virtual void treeSearch(wxCommandEvent& event);
    virtual void updatePreview(wxCommandEvent& event);
    virtual void updateTreeSearch(wxCommandEvent& event);
};
#endif // PATHGENERATORFRAME_H
