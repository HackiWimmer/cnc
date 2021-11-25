#ifndef CNC_LOGGER_PROXY_H
#define CNC_LOGGER_PROXY_H

#include "CncTextCtrl.h"

class CncLoggerListCtrl;

// --------------------------------------------------------------
class CncLoggerProxy : public CncTextCtrl {
	
	protected:
		static const unsigned int NO_SOUREC_ID =   -1;
		
		int lastSourceID;

	public:
		CncLoggerProxy(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		               long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr);
		virtual ~CncLoggerProxy();
		
		virtual CncLoggerListCtrl* getListCtrl() const = 0;
		
		virtual bool setTextColour(const wxColour& col);
		virtual void appendChar(char c, const wxColour& col, int sourceId);
		virtual void appendChar(char c, const wxTextAttr& style, int sourceId);

		virtual bool SetDefaultStyle(const wxTextAttr& style);
		virtual void AppendChar(char c);
		virtual void AppendText(const wxString &text);
		
		wxDECLARE_NO_COPY_CLASS(CncLoggerProxy);
};

// --------------------------------------------------------------
class CncStartupLoggerProxy : public CncLoggerProxy {
	
	public:
		CncStartupLoggerProxy(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		                      long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr)
		: CncLoggerProxy(parent, wxID_ANY, value, pos, size, style, validator, name)
		{}
		
		virtual ~CncStartupLoggerProxy()
		{}
		
		virtual CncLoggerListCtrl* getListCtrl() const;
		
		wxDECLARE_NO_COPY_CLASS(CncStartupLoggerProxy);
};

// --------------------------------------------------------------
class CncStandardLoggerProxy : public CncLoggerProxy {
	
	public:
		CncStandardLoggerProxy(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		                       long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr)
		: CncLoggerProxy(parent, wxID_ANY, value, pos, size, style, validator, name)
		{}
		
		virtual ~CncStandardLoggerProxy()
		{}
		
		virtual CncLoggerListCtrl* getListCtrl() const;
		
		wxDECLARE_NO_COPY_CLASS(CncStandardLoggerProxy);
};

// --------------------------------------------------------------
class CncDryRunLoggerProxy : public CncLoggerProxy {
	
	public:
		CncDryRunLoggerProxy(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		                     long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr)
		: CncLoggerProxy(parent, wxID_ANY, value, pos, size, style, validator, name)
		{}
		
		virtual ~CncDryRunLoggerProxy()
		{}
		
		virtual CncLoggerListCtrl* getListCtrl() const;
		
		wxDECLARE_NO_COPY_CLASS(CncDryRunLoggerProxy);
};

// --------------------------------------------------------------
class CncParserSynopsisProxy : public CncLoggerProxy  {
	
	public:
		CncParserSynopsisProxy(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		                       long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr)
		: CncLoggerProxy(parent, wxID_ANY, value, pos, size, style, validator, name)
		{}
		
		virtual ~CncParserSynopsisProxy()
		{}
		
		virtual CncLoggerListCtrl* getListCtrl() const;
		
		void addEntry(const char type, const wxString& entry);
		void addInfo(const wxString& entry)							{ addEntry('I', entry); }
		void addWarning(const wxString& entry)						{ addEntry('W', entry); }
		void addError(const wxString& entry)						{ addEntry('E', entry); }
		void addSeparator(const wxString& entry="")					{ addEntry('S', entry); }
		
		bool hasDebugEntries()	const;
		bool hasWarnEntries()	const;
		bool hasErrorEntries()	const;
		
		void popProcessMode();
		void pushUpdateMode();
		
		wxDECLARE_NO_COPY_CLASS(CncParserSynopsisProxy);
};

// --------------------------------------------------------------
class CncMsgHistoryLoggerProxy : public CncLoggerProxy  {
	
	public:
		CncMsgHistoryLoggerProxy(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		                         long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr)
		: CncLoggerProxy(parent, wxID_ANY, value, pos, size, style, validator, name)
		{}
		
		virtual ~CncMsgHistoryLoggerProxy()
		{}
		
		virtual CncLoggerListCtrl* getListCtrl() const;
		
		wxDECLARE_NO_COPY_CLASS(CncMsgHistoryLoggerProxy);
};

// --------------------------------------------------------------
class CncTraceProxy : public CncTextCtrl  {
	
	protected:
		static const unsigned int NO_SOUREC_ID =   -1;
		
		int lastSourceID;
		
	public:
		CncTraceProxy(wxWindow *parent, wxWindowID id=wxID_ANY, const wxString &value=wxEmptyString, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
		              long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxTextCtrlNameStr);
		virtual ~CncTraceProxy();
		
		virtual void Clear();
		
		virtual bool setTextColour(const wxColour& col);
		virtual void appendChar(char c, const wxColour& col, int sourceId);
		virtual void appendChar(char c, const wxTextAttr& style, int sourceId);

		virtual bool SetDefaultStyle(const wxTextAttr& style);
		virtual void AppendChar(char c);
		virtual void AppendText(const wxString &text);
		
		wxDECLARE_NO_COPY_CLASS(CncTraceProxy);
};

#endif