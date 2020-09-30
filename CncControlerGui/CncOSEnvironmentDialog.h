#ifndef CNCOSENVIRONMENTDIALOG_H
#define CNCOSENVIRONMENTDIALOG_H

#include "wxCrafterBaseDialogs.h"

class CncContextListCtrl;
class CncOSEnvironmentDialog : public CncOSEnvironmentDialogBase {
	
	public:
		CncOSEnvironmentDialog(wxWindow* parent);
		virtual ~CncOSEnvironmentDialog();
		
		CncContextListCtrl* contextControl;
		
		void update();
		
		const unsigned int PAGE_ENVIRONMENT = 0;
		const unsigned int PAGE_MODULES 	= 1;

		const unsigned int ENV_COL_PARAM 	= 0;
		const unsigned int ENV_COL_VALUE 	= 1;
		
		const unsigned int MOD_COL_ADDR  	= 0;
		const unsigned int MOD_COL_NAME 	= 1;
		const unsigned int MOD_COL_VERSION 	= 2;
		const unsigned int MOD_COL_PATH 	= 3;
		
		const unsigned int VER_COL_PARAM 	= 0;
		const unsigned int VER_COL_VALUE 	= 1;
		
	protected:
    virtual void onRefresh(wxCommandEvent& event);
		virtual void onSelectListBook(wxListbookEvent& event);
		virtual void sortModules(wxCommandEvent& event);
		virtual void selectModulesItem(wxListEvent& event);
		virtual void selectOSEnvironmentItem(wxListEvent& event);
		virtual void selectAppEnvironmentItem(wxListEvent& event);
		
		virtual void onClose(wxCommandEvent& event);
		virtual void onSize(wxSizeEvent& event);
			
		void evaluate();
		void evaluateOSEnvrionment();
		void evaluateAppEnvrionment();
		void evaluateLoadedModules();
		void evaluateVersionInfo();
		
		void sizeOSEnvironmentColumns(bool onlyLastColumn=false);
		void sizeAppEnvironmentColumns(bool onlyLastColumn=false);
		void sizeModulesColumns(bool onlyLastColumn=false);
};
#endif // CNCOSENVIRONMENTDIALOG_H
