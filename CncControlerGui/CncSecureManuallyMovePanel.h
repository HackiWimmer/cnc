#ifndef CNCSECUREMANUALLYMOVEPANEL_H
#define CNCSECUREMANUALLYMOVEPANEL_H

#include <vector>
#include "wxCrafterSecurePanel.h"

class CncSecureManuallyMovePanel : public CncSecureManuallyMovePanelBase {
	public:
		CncSecureManuallyMovePanel(wxWindow* parent);
		virtual ~CncSecureManuallyMovePanel();
		
	protected:
    virtual void onClearF(wxMouseEvent& event);
		virtual void onSetDimMode(wxCommandEvent& event);
		virtual void onClearX(wxMouseEvent& event);
		virtual void onClearY(wxMouseEvent& event);
		virtual void onClearZ(wxMouseEvent& event);
		virtual void onLeftDownResultValue(wxMouseEvent& event);
		virtual void onMove(wxCommandEvent& event);
		virtual void onAxis(wxCommandEvent& event);
		
	private:
		typedef std::vector<wxToggleButton*>	AxisButtons;
		typedef std::vector<wxToggleButton*>	DimButtons;
		
		const char*			AXIS_RESULT_FORMAT	= "%+.3lf";
		const char*			SPEED_RESULT_FORMAT	= "%.1lf";
		const char 			DOT					= '.';
		const char			BKS					= '\b';
		const char			INVALID_AXIS		='\0';
		const char			INVALID_NUMBER		='\0';
		
		AxisButtons			axisButtons;
		DimButtons			dimButtons;
		char				currentAxis;
		char				lastNumber;
		double				currentValueX;
		double				currentValueY;
		double				currentValueZ;
		double				currentValueF;
		
		void				updateResult();
		double				getCurrentAxisValue();
		bool				setCurrentAxisValue(double v);
		bool				setCurrentAxisMode(const char axis);
		bool				setCurrentAxisMode(const wxString& axis);
		const wxString&		prepareStringValue(wxString& val);
		
};
#endif // CNCSECUREMANUALLYMOVEPANEL_H
