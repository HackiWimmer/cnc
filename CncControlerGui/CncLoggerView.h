#ifndef CNCLOGGERVIEW_H
#define CNCLOGGERVIEW_H

#include "CncCommon.h"
#include "wxCrafterLoggerPanel.h"

class CncTraceCtrl;
class CncLoggerListCtrl;
class CncLoggerView : public CncLoggerViewBase {
	
	public:
		CncLoggerView(wxWindow* parent);
		virtual ~CncLoggerView();
		
		bool doShowLoggerOnCommand() const	{ return m_btLoggerOnDemand->GetValue(); }
		void setShowOnDemandState(bool s)	{ m_btLoggerOnDemand->SetValue(s); }
		
		void enable(bool state);
		
		void initialize();
		
		void popImmediatelyMode				(LoggerSelection::VAL id);
		void popProcessMode					(LoggerSelection::VAL id);
		void pushUpdateMode					(LoggerSelection::VAL id);
		
		void select							(LoggerSelection::VAL id);
		void clear							(LoggerSelection::VAL id);
		void logCurrentPosition				(LoggerSelection::VAL id);
		void setErrorFlag					(LoggerSelection::VAL id, bool flag);
		void changeTextAttr					(LoggerSelection::VAL id, const wxTextAttr& ta);
		void changeResult					(LoggerSelection::VAL id, const wxString& text, long int row);
		void changeResultForLoggedPosition	(LoggerSelection::VAL id, const wxString& text);
		void add							(LoggerSelection::VAL id, const char c);
		void add							(LoggerSelection::VAL id, const wxString& text);
		
		void clearTrace();
		void changeTextAttr					(const wxTextAttr& ta);
		void trace							(const char c);
		void trace							(const wxString& text);
		
		void openTraceHistory();
		
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
		
		CncTextCtrl*	timeCtrl;
		CncTraceCtrl*	traceCtrl;
};
#endif // CNCLOGGERVIEW_H
