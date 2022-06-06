#ifndef CNCTESTRUNCONFIG_H
#define CNCTESTRUNCONFIG_H

#include "wxCrafterUnitTest.h"

class CncTestRunConfig : public CncTestRunConfigBase
{
	public:
		
		CncTestRunConfig(wxWindow* parent);
		virtual ~CncTestRunConfig();
		
	protected:
		
		virtual void onCancel(wxCommandEvent& event);
		virtual void onRun(wxCommandEvent& event);
		
	private:
		
		bool runLoop();
		bool runDir();
};
#endif // CNCTESTRUNCONFIG_H
