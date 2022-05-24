#ifndef CNCSIMUHWDIMENSIONSETUP_H
#define CNCSIMUHWDIMENSIONSETUP_H
#include "wxCrafterBaseDialogs.h"

class CncSimuHwDimensionSetup : public CncSimuHwDimensionSetupBase
{
	public:
		CncSimuHwDimensionSetup(wxWindow* parent);
		virtual ~CncSimuHwDimensionSetup();
		
		void processDefault();
		
	protected:
		virtual void onSelectZLocation(wxCommandEvent& event);
		virtual void onUpdateTextValues(wxCommandEvent& event);
	
		virtual void onRefCentered(wxCommandEvent& event);
		virtual void onRefSector1(wxCommandEvent& event);
		virtual void onRefSector2(wxCommandEvent& event);
		virtual void onRefSector3(wxCommandEvent& event);
		virtual void onRefSector4(wxCommandEvent& event);
		
		virtual void onTakeOverFromConfigMaxDimX(wxCommandEvent& event);
		virtual void onTakeOverFromConfigMaxDimY(wxCommandEvent& event);
		virtual void onTakeOverFromConfigMaxDimZ(wxCommandEvent& event);
		
		virtual void onCloseWindow(wxCloseEvent& event);
		virtual void onInitDialog(wxInitDialogEvent& event);
		virtual void onCancel(wxCommandEvent& event);
		virtual void onOk(wxCommandEvent& event);

	private:
		
		const char*	fltFormat	= "%.3lf";
		
		struct PreviousSetup
		{
			double					maxDimX;
			double					maxDimY;
			double					maxDimZ;
			
			double					hwoX;
			double					hwoY;
			double					hwoZ;
		};
		
		PreviousSetup previousSetup;
		
		double	getValue(wxTextCtrl* ctrl, double dfltValue);
		double	evaluateZLocation();
		
		void	cancel();
		void	apply();
};

#endif // CNCSIMUHWDIMENSIONSETUP_H
