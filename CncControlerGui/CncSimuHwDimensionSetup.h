#ifndef CNCSIMUHWDIMENSIONSETUP_H
#define CNCSIMUHWDIMENSIONSETUP_H
#include "wxCrafterBaseDialogs.h"

class CncSimuHwDimensionSetup : public CncSimuHwDimensionSetupBase
{
	public:
		
		enum Mode { M_DIMENSION, M_ORIGIN };
		
		CncSimuHwDimensionSetup(wxWindow* parent, Mode m);
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
			double		maxDimX;
			double		maxDimY;
			double		maxDimZ;
			
			double		hwoX;
			double		hwoY;
			double		hwoZ;
		};
		
		PreviousSetup	previousSetup;
		Mode			mode;
		
		double	getValue(wxTextCtrl* ctrl, double dfltValue) const;
		double	evaluateZLocation();
		
		bool	isSomethingChanged() const;
		
		void	cancel();
		void	apply();
		void	applyFinally();
};

#endif // CNCSIMUHWDIMENSIONSETUP_H
