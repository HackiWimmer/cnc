#ifndef CNCTOUCHBLOCKMANAGEMENT_H
#define CNCTOUCHBLOCKMANAGEMENT_H
#include "wxCrafterReferencePosition.h"

class CncTouchblockManagement : public CncTouchblockManagementBase {
	
	public:
		enum TouchMode { TM_UNKNOWN, TM_A, TM_B, TM_C, TM_D };

		CncTouchblockManagement(wxWindow* parent);
		virtual ~CncTouchblockManagement();
		
	protected:
		virtual void onTakeOver(wxCommandEvent& event);
		virtual void onCancel(wxCommandEvent& event);
		virtual void onTouchBlock(wxCommandEvent& event);
		virtual void onTouchTest(wxCommandEvent& event);
		virtual void onTouchXYZ(wxCommandEvent& event);
		virtual void onTouchZ(wxCommandEvent& event);
		
	private:
		
		const TouchMode		getTouchMode()							const;
		void				setTouchMode(const TouchMode m);
		double				getValueAsDouble(wxTextCtrl* ctrl, double defaultValue = 0.0);
};

#endif // CNCTOUCHBLOCKMANAGEMENT_H
