#ifndef CNCLOGGERVIEW_H
#define CNCLOGGERVIEW_H

#include "CncCommon.h"
#include "wxCrafterLoggerPanel.h"

class CncTraceCtrl;
class CncLoggerListCtrl;
class CncTraceInfoBar;

class CncLoggerView : public CncLoggerViewBase {
	
	public:
		CncLoggerView(wxWindow* parent);
		virtual ~CncLoggerView();
		
		virtual bool Enable(bool enable = true);
		
		CncLoggerListCtrl* getLoggerListCtrl(LoggerSelection::VAL id) const;
		CncTraceCtrl* getTraceCtrl() const;
		
		bool doShowLoggerOnCommand() const	{ return m_btLoggerOnDemand->GetValue(); }
		void setShowOnDemandState(bool s)	{ m_btLoggerOnDemand->SetValue(s); }
		void setSecureMode(bool state);
		
		void enable(bool state);
		void enableListCtrlsOnly(bool state);
		
		void initialize();
		
		void incCurrentIndent					(LoggerSelection::VAL id);
		void decCurrentIndent					(LoggerSelection::VAL id);
		void setCurrentIndent					(LoggerSelection::VAL id, unsigned int i);
		unsigned int getCurrentIndent			(LoggerSelection::VAL id) const;
		
		void popImmediatelyMode					(LoggerSelection::VAL id);
		void popProcessMode						(LoggerSelection::VAL id);
		void pushUpdateMode						(LoggerSelection::VAL id);
		
		void select								(LoggerSelection::VAL id);
		void clear								(LoggerSelection::VAL id);
		void setErrorFlag						(LoggerSelection::VAL id, bool flag);
		void changeTextAttr						(LoggerSelection::VAL id, const wxTextAttr& ta);
		void changeTextColour					(LoggerSelection::VAL id, const wxColour& col);
		void add								(LoggerSelection::VAL id, const char c);
		void add								(LoggerSelection::VAL id, const wxString& text);
		void add								(LoggerSelection::VAL id, const wxString& text, const wxString& result);
		void add								(const char c);
		void add								(const wxString& text);
		
		void clearTrace();
		void changeTextAttr						(const wxTextAttr& ta);
		void trace								(const char c);
		void trace								(const wxString& text);
		
		void openTraceHistory();
		
		void saveAll(LoggerSelection::VAL id, bool doOpen);
		void saveAll(bool doOpen);
		
		static const char* getLoggerIDAsStr(LoggerSelection::VAL id);
		
	protected:
		
		virtual void onClearTraceHistory(wxCommandEvent& event);
		virtual void onSelectStandardLogger(wxCommandEvent& event);
		virtual void onSelectStartupLogger(wxCommandEvent& event);
		virtual void onViewTraceHistory(wxCommandEvent& event);
		virtual void onToggleShowOnDemand(wxCommandEvent& event);
		virtual void onClear(wxCommandEvent& event);
		virtual void onCopy(wxCommandEvent& event);
		virtual void onCopyAll(wxCommandEvent& event);
		virtual void onSave(wxCommandEvent& event);
		virtual void onSaveAll(wxCommandEvent& event);
		virtual void onView(wxCommandEvent& event);
		virtual void onViewAll(wxCommandEvent& event);
		
	private:
		
		const unsigned int MaxLoggerCount = 2;
		LoggerSelection::VAL currentLoggerIndex;
		
		typedef std::vector<CncLoggerListCtrl*> LoggerLists;
		LoggerLists loggerLists;
		
		CncTraceInfoBar*	traceInfoBar;
		CncTraceCtrl*		traceCtrl;
};
#endif // CNCLOGGERVIEW_H
