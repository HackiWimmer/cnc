#ifndef BIN_PARSE_HANDLER_BASE_H
#define BIN_PARSE_HANDLER_BASE_H

#include <sstream>
#include "PathHandlerBase.h"

///////////////////////////////////////////////////////////////////////////////////////////
class BinaryPathHandlerBase : public PathHandlerBase {
	
	protected:
		
		virtual bool processLinearMove(bool alreadyRendered) { return true; }
		
	public:
		BinaryPathHandlerBase();
		virtual ~BinaryPathHandlerBase();
		
		virtual const char* getName() { return "BinaryPathHandlerBase"; }
		
		// common interface
		virtual bool processCommand(const unsigned char* buffer, int nbBytes) { return true; };

		// view interface
		virtual const wxString& getViewContent(wxString& content) { return content; }
		
};

///////////////////////////////////////////////////////////////////////////////////////////
class BinaryPathHandlerHexView : public BinaryPathHandlerBase {
	
	public:
		enum FormatType {Raw, CStyle};
	
	private:
		std::stringstream hexContent;
		FormatType formatType;
		
	public:
		
		explicit BinaryPathHandlerHexView(FormatType ft = FormatType::Raw);
		virtual ~BinaryPathHandlerHexView() {}
		
		virtual void prepareWork(); 
		virtual void finishWork();
		
		virtual bool processCommand(const unsigned char* buffer, int nbBytes);
		
		virtual const wxString& getViewContent(wxString& content) { content.assign(hexContent.str()); return content; }
};

///////////////////////////////////////////////////////////////////////////////////////////
class BinaryPathHandlerHumanReadableView : public BinaryPathHandlerBase {
	
	public:
		enum FormatType {Steps, Metric};

	private:
		std::stringstream readableContent;
		FormatType formatType;
		
		bool displaySetter(const unsigned char* buffer, int nbBytes);
		bool displaySteps(const unsigned char* buffer, int nbBytes);
		bool displayMetric(const unsigned char* buffer, int nbBytes);
		
	public:
		
		BinaryPathHandlerHumanReadableView(FormatType ft);
		virtual ~BinaryPathHandlerHumanReadableView() {}
		
		virtual void prepareWork(); 
		virtual void finishWork();
		
		virtual bool processCommand(const unsigned char* buffer, int nbBytes);

		virtual const wxString& getViewContent(wxString& content) { content.assign(readableContent.str()); return content; }
};

#endif