#ifndef CNCMANUALLYMOVECOORDINATES_H
#define CNCMANUALLYMOVECOORDINATES_H

#include "wxCrafterMainFrame.h"

class CncManuallyMoveCoordinates : public CncManuallyMoveCoordinatesBase {
	
	public:
		CncManuallyMoveCoordinates(wxWindow* parent);
		virtual ~CncManuallyMoveCoordinates();
		
		void resetClearViewState();
		void reset();
		
		void initialize();
		void updateUnit();
		
		void setValueX(double value);
		void setValueY(double value);
		void setValueZ(double value);
		
		void enableControls(bool state);
		
		double getValueX();
		double getValueY();
		double getValueZ();
		
		bool isAbsoluteMove() 		{ return m_mmRadioCoordinates->GetSelection() == 0; }
		bool switchToolOn() 		{ return m_checkBoxToolEnabled->GetValue(); }
		bool correctLimitStates() 	{ return m_manuallyCorrectLimitPos->GetValue(); }
		
		bool shouldClearMontionMonitor();
		
	protected:
    virtual void onZeroXYZ(wxCommandEvent& event);
    virtual void onReverseXYZ(wxCommandEvent& event);
	
		virtual void onSetCommonValue(wxCommandEvent& event);
		virtual void onLBDownMax(wxMouseEvent& event);
		virtual void changeManuallySliderX(wxScrollEvent& event);
		virtual void changeManuallySliderY(wxScrollEvent& event);
		virtual void changeManuallySliderZ(wxScrollEvent& event);
		virtual void maxManuallyXSlider(wxCommandEvent& event);
		virtual void maxManuallyYSlider(wxCommandEvent& event);
		virtual void maxManuallyZSlider(wxCommandEvent& event);
		virtual void minManuallyXSlider(wxCommandEvent& event);
		virtual void minManuallyYSlider(wxCommandEvent& event);
		virtual void minManuallyZSlider(wxCommandEvent& event);
		virtual void moveManuallySliderX(wxScrollEvent& event);
		virtual void moveManuallySliderY(wxScrollEvent& event);
		virtual void moveManuallySliderZ(wxScrollEvent& event);
		virtual void signManuallyXSlider(wxCommandEvent& event);
		virtual void signManuallyYSlider(wxCommandEvent& event);
		virtual void signManuallyZSlider(wxCommandEvent& event);
		virtual void updateMetricX(wxCommandEvent& event);
		virtual void updateMetricY(wxCommandEvent& event);
		virtual void updateMetricZ(wxCommandEvent& event);
		virtual void zeroManuallyXSlider(wxCommandEvent& event);
		virtual void zeroManuallyYSlider(wxCommandEvent& event);
		virtual void zeroManuallyZSlider(wxCommandEvent& event);

	private:
		
		bool shouldClear;
		
		void update(wxTextCtrl* ctrl, double value);
		void update(wxSlider*   ctrl, double value);
		void update(wxSlider*   ctrl, const wxString& value);
		
		void updateValidators();
		
};
#endif // CNCMANUALLYMOVECOORDINATES_H
