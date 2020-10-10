#ifndef CNCARDUINOENVIRONMENT_H
#define CNCARDUINOENVIRONMENT_H
#include "wxCrafterArduinoEnv.h"

class CncArduinoPinsListCtrl;
class CncMessageListCtrl;
class CncArduinoEnvironment : public CncArduinoEnvironmentBase {
	
	public:
		CncArduinoEnvironment(wxWindow* parent);
		virtual ~CncArduinoEnvironment();
		
		void appendMessage	(char type, const wxString& msg, const wxString& context=wxEmptyString);
		void appendSep      () { appendMessage('S', wxString('*', 100), wxString('*', 50)); }

		void appendInfo		(const wxString& msg, const wxString& ctx=wxEmptyString)	{ appendMessage('I', msg, ctx); }
		void appendWarning	(const wxString& msg, const wxString& ctx=wxEmptyString)	{ appendMessage('W', msg, ctx); }
		void appendDebug	(const wxString& msg, const wxString& ctx=wxEmptyString)	{ appendMessage('D', msg, ctx); }
		void appendError	(const wxString& msg, const wxString& ctx=wxEmptyString)	{ appendMessage('E', msg, ctx); }
		
		void notifyStarted();
		void notifyConnecting();
		void notifyConnected();
		void notifyDisconnected();
		void notifyPaused();
		void notifyResumed();
		
		void notifyPinUpdate();
		void notifyDataUpdate();
		
		void activateOnClose(bool state) { canClose = state; }
		
	protected:
		virtual void onValuesUpdateInterval(wxScrollEvent& event);
		virtual void onLoggerUpdateInterval(wxScrollEvent& event);
		virtual void onSortPins(wxCommandEvent& event);
		virtual void onConfigChanged(wxPropertyGridEvent& event);
		virtual void onConfigChanging(wxPropertyGridEvent& event);
		virtual void onSelectConfiguration(wxCommandEvent& event);
		virtual void onShow(wxShowEvent& event);
		virtual void onEmergencyButton(wxCommandEvent& event);
		virtual void onClearTrace(wxCommandEvent& event);
		virtual void onForceUpdate(wxCommandEvent& event);
		virtual void onSelectArduinoPeriphery(wxCommandEvent& event);
		virtual void onClose(wxCloseEvent& event);
		virtual void onStartupTimer(wxTimerEvent& event);
		virtual void onContinuousTimer(wxTimerEvent& event);
		
	private:
		
		enum BookSelection { BS_Periphery=0, BS_Configuration=1 };
		
		struct LimitSwitchInfo {
			wxButton* 	button	= NULL;
			char		type	= '\0';
			int 		name	=   -1;
			bool		value	= LimitSwitch::LIMIT_SWITCH_OFF;
			
			LimitSwitchInfo()
			: LimitSwitchInfo (NULL, '\0' , -1, LimitSwitch::LIMIT_SWITCH_OFF)
			{}
			
			LimitSwitchInfo(wxButton* b, char t, int n, bool v)
			: button(b), type(t), name(n), value(v)
			{}
			
			int setValueOn()  { value = LimitSwitch::LIMIT_SWITCH_ON;  return value; }
			int setValueOff() { value = LimitSwitch::LIMIT_SWITCH_OFF; return value; }
			
			int toogleValue() {
				value = value == LimitSwitch::LIMIT_SWITCH_OFF  ? LimitSwitch::LIMIT_SWITCH_ON 
																: LimitSwitch::LIMIT_SWITCH_OFF;
				return value;
			}
			
			bool isLimit()		const { return value == LimitSwitch::LIMIT_SWITCH_ON; }
			bool getBoolValue()	const { return value == LimitSwitch::LIMIT_SWITCH_ON ? true : false; }
			bool getPinValue()	const { return value; }
		};
		
		struct SupportSwitchInfo {
			wxButton* 	button	= NULL;
			char		type	= '\0';
			bool		value	= OFF;
			
			SupportSwitchInfo()
			: SupportSwitchInfo (NULL, '\0', OFF)
			{}
			
			SupportSwitchInfo(wxButton* b, char t, bool v)
			: button(b), type(t), value(v)
			{}
			
			int setValueOn()  { value = 0; return value; }
			int setValueOff() { value = 1; return value; }
			
			int toogleValue() {
				value = value == 0 ? 1 : 0;
				return value;
			}
			
			bool getBoolValue()	const { return value == ON ? true : false; }
			bool getPinValue()	const { return value; }
		};
		

		bool 					reConnectRequired;
		bool					showDebugMessages;
		bool					canClose;

		CncArduinoPinsListCtrl* pinList;
		CncMessageListCtrl* 	logger;
		
		wxBitmap*				lsOn;
		wxBitmap*				lsOff;
		wxBitmap*				irOn;
		wxBitmap*				irOff;
		
		LimitSwitchInfo 		lsiMinX;
		LimitSwitchInfo			lsiMaxX;
		
		LimitSwitchInfo 		lsiMinY;
		LimitSwitchInfo			lsiMaxY;

		LimitSwitchInfo 		lsiMinZ;
		LimitSwitchInfo			lsiMaxZ;
		
		SupportSwitchInfo		ssiBit8;
		SupportSwitchInfo		ssiBit7;
		SupportSwitchInfo		ssiBit6;
		SupportSwitchInfo		ssiBit5;
		SupportSwitchInfo		ssiBit4;
		SupportSwitchInfo		ssiBit3;
		SupportSwitchInfo		ssiBit2;
		SupportSwitchInfo		ssiBit1;
		
		unsigned char			limitStates;
		unsigned char			supportStates;
		
		CncArduinoEnvironment::LimitSwitchInfo* findLimitSwichInfo(wxButton* bt);
		void decorateEmergencySwitch();
		void decorateSwitch(wxButton* bt, bool state);
		
		bool publishStatesUpdate();
		bool publishLimitSwitchUpdate(int name, bool state);
		void publishForceUpdate();
		
		void setLoggerUpdateInterval();
		void setValuesUpdateInterval();
		
		void update(const AE::TransferData& data);
		
		void updateLimitStates();
		void updateSupportStates();
		void updateStateCtrl(wxTextCtrl* ctrl, const unsigned char states);
		
		void updatePinValue(const char type, unsigned int num, const char mode, int value);
		
		void enableControls(bool state, bool all = false);
		void initValues();
		
		bool getBit(const unsigned char states, const unsigned char idx) const;
		void setBit(unsigned char& states, 		const unsigned char idx, const bool value);
		
		void onLsLeftDown (wxMouseEvent& event);
		void onLsLeftUp   (wxMouseEvent& event);
		
		void onSptBtnLeftUp1   (wxMouseEvent& event);
		void onSptBtnLeftDown1 (wxMouseEvent& event);
		void onSptBtnLeftUp2   (wxMouseEvent& event);
		void onSptBtnLeftDown2 (wxMouseEvent& event);
		void onSptBtnLeftUp3   (wxMouseEvent& event);
		void onSptBtnLeftDown3 (wxMouseEvent& event);
		void onSupportSwt1     (wxCommandEvent& event);
		void onSupportSwt2     (wxCommandEvent& event);
};

#endif // CNCARDUINOENVIRONMENT_H

