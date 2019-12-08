#ifndef CNCOSENVIRONMENTDIALOG_H
#define CNCOSENVIRONMENTDIALOG_H

#include "wxCrafterBaseDialogs.h"

class CncContextListCtrl;
class CncOSEnvironmentDialog : public CncOSEnvironmentDialogBase {
	
	public:
		CncOSEnvironmentDialog(wxWindow* parent);
		virtual ~CncOSEnvironmentDialog();
		
		CncContextListCtrl* contextControl;
		
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
	
		virtual void sortModules(wxCommandEvent& event);
		virtual void selectModulesItem(wxListEvent& event);
		virtual void selectOSEnvironmentItem(wxListEvent& event);
		virtual void selectAppEnvironmentItem(wxListEvent& event);
		
		virtual void onClose(wxCommandEvent& event);
		virtual void onSize(wxSizeEvent& event);
			
		void evaluateOSEnvrionemnt();
		void evaluateAppEnvrionemnt();
		void evaluateLoadedModules();
		void evaluateVersionInfo();
		
		void sizeOSEnvironmentColumns(bool onlyLastColumn=false);
		void sizeAppEnvironmentColumns(bool onlyLastColumn=false);
		void sizeModulesColumns(bool onlyLastColumn=false);
};
#endif // CNCOSENVIRONMENTDIALOG_H
