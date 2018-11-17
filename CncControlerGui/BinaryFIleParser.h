#ifndef CNC_BIN_TPL_PARSER_H
#define CNC_BIN_TPL_PARSER_H

#include <map>
#include <wx/xml/xml.h>
#include "FileParser.h"
#include "BinaryPathHandlerBase.h"
#include "CncBinaryTemplateStreamer.h"

class BinaryFileParser : protected CncBinaryTemplateStreamer 
                       , public FileParser
{
	
	typedef std::ifstream InputStream;
	
	public:
	
		enum ViewType { HexRaw, HexCStyle, ReadableSteps, ReadableMetric };
		
		BinaryFileParser(const char* fullFileName, BinaryPathHandlerBase* ph = NULL);
		virtual ~BinaryFileParser();
		
		bool preview() { return preprocess(); }
		
		const wxString& getSourceContent(wxString& content);
		const wxString& getDataHeaderContent(wxString& content);
		const wxString& getDataBodyView(wxString& content);
		
		const wxString& getXMLParameter(const wxString& section, const wxString& name, wxString& value);
		const wxString& getSourceParameter(const wxString& name, wxString& value);
		const wxString& getSetupParameter(const wxString& name, wxString& value);
		const wxString& getProcessParameter(const wxString& name, wxString& value);
		
		static bool extractSourceContentAsString(const wxString& binFileName, wxString& content);
		static bool extractDataHeaderAsString(const wxString& binFileName, wxString& content);
		static bool extractViewAsString(ViewType vt,  const wxString& binFileName, wxString& content);
		static bool extractSourceContentAsFile(const wxString& binFileName, wxString& sourceFileName);
		
	protected:
		
		BinaryPathHandlerBase* pathHandler;
		
		virtual bool preprocess();
		virtual bool spool();
		virtual bool postprocess();

		virtual void initNextClientId(long id);
		virtual void logMeasurementStart();
		virtual void logMeasurementEnd();
	
	private:

		InputStream inputStream;
		
		typedef std::map<wxString, wxString> ParameterMap;
		ParameterMap parameterMap;
		
		wxXmlDocument* xmlParameter;
		std::string outputFileName;
		std::string sourceContent;
		std::string dataHeaderContent;
		
		uint32_t totalFileLength;
		uint32_t sourceContentOffset;
		uint32_t dataHeaderOffset;
		uint32_t dataBodyOffset;
		
		void expandXmlParameter();
		
		bool checkFileSignature(unsigned char* fileSignature);
		unsigned int readBytes(InputStream& is, unsigned char* buffer, unsigned int nbBytes);
		unsigned int readDataBlock(InputStream& is);
		
		bool readDataBody(InputStream& is);
		bool read_unit32_t(InputStream& is, uint32_t& ret);
};

#endif