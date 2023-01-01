#ifndef CNCUNITTESTS_H
#define CNCUNITTESTS_H
#include "wxCrafterUnitTest.h"

class CncUnitTests : public CncUnitTestsBase
{
public:
    CncUnitTests(wxWindow* parent);
    virtual ~CncUnitTests();
protected:
    virtual void clearView(wxCommandEvent& event);
    virtual void onShow(wxShowEvent& event);
    virtual void onStartupTimer(wxTimerEvent& event);
    virtual void selectTest(wxCommandEvent& event);
};
#endif // CNCUNITTESTS_H
