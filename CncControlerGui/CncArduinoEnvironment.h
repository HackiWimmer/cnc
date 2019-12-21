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
		
		void update(const AE::TransferData& data);
		
		void activateOnClose(bool state) { canClose = state; }
		
	protected:
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
			int			value	=   -1;
			
			LimitSwitchInfo()
			: LimitSwitchInfo (NULL, '\0' , -1, -1)
			{}
			
			LimitSwitchInfo(wxButton* b, char t, int n, int v)
			: button(b), type(t), name(n), value(v)
			{}
			
			int setValueOn()  { value = 0; return value; }
			int setValueOff() { value = 1; return value; }
			
			int toogleValue() {
				value = value == 0 ? 1 : 0;
				return value;
			}
			
			bool getBoolValue() {
				return value == 0 ? true : false;
			}
		};
		
		struct SupportSwitchInfo {
			wxButton* 	button	= NULL;
			char		type	= '\0';
			int			value	=   -1;
			
			SupportSwitchInfo()
			: SupportSwitchInfo (NULL, '\0', -1)
			{}
			
			SupportSwitchInfo(wxButton* b, char t, int v)
			: button(b), type(t), value(v)
			{}
			
			int setValueOn()  { value = 0; return value; }
			int setValueOff() { value = 1; return value; }
			
			int toogleValue() {
				value = value == 0 ? 1 : 0;
				return value;
			}
			
			bool getBoolValue() {
				return value == 0 ? true : false;
			}
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
