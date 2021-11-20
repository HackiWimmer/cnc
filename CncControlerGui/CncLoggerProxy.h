#ifndef CNC_LOGGER_PROXY_H
#define CNC_LOGGER_PROXY_H

#include "CncTextCtrl.h"

// --------------------------------------------------------------
class CncStartupLoggerProxy : public CncTextCtrl  {
	
	public:
		CncStartupLoggerProxy(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		               long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr);
		virtual ~CncStartupLoggerProxy();
		
		virtual bool SetDefaultStyle(const wxTextAttr& style);
		virtual void AppendChar(char c);
		virtual void AppendText(const wxString &text);
		
		wxDECLARE_NO_COPY_CLASS(CncStartupLoggerProxy);
};

// --------------------------------------------------------------
class CncStandardLoggerProxy : public CncTextCtrl  {
	
	public:
		CncStandardLoggerProxy(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		               long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr);
		virtual ~CncStandardLoggerProxy();
		
		virtual bool SetDefaultStyle(const wxTextAttr& style);
		virtual void AppendChar(char c);
		virtual void AppendText(const wxString &text);
		
		wxDECLARE_NO_COPY_CLASS(CncStandardLoggerProxy);
};

// --------------------------------------------------------------
class CncTryRunLoggerProxy : public CncTextCtrl  {
	
	public:
		CncTryRunLoggerProxy(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		               long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr);
		virtual ~CncTryRunLoggerProxy();
		
		virtual bool SetDefaultStyle(const wxTextAttr& style);
		virtual void AppendChar(char c);
		virtual void AppendText(const wxString &text);
		
		wxDECLARE_NO_COPY_CLASS(CncTryRunLoggerProxy);
};

// --------------------------------------------------------------
class CncMsgHistoryLoggerProxy : public CncTextCtrl  {
	
	public:
		CncMsgHistoryLoggerProxy(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		               long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr);
		virtual ~CncMsgHistoryLoggerProxy();
		
		virtual bool SetDefaultStyle(const wxTextAttr& style);
		virtual void AppendChar(char c);
		virtual void AppendText(const wxString &text);
		
		wxDECLARE_NO_COPY_CLASS(CncMsgHistoryLoggerProxy);
};

// --------------------------------------------------------------
class CncTraceProxy : public CncTextCtrl  {
	
	public:
		CncTraceProxy(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		               long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr);
		virtual ~CncTraceProxy();
		
		virtual void Clear();
		virtual bool SetDefaultStyle(const wxTextAttr& style);
		virtual void AppendChar(char c);
		virtual void AppendText(const wxString &text);
		
		wxDECLARE_NO_COPY_CLASS(CncTraceProxy);
};

#endif