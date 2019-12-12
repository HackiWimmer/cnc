#include <iostream>
#include <wx/file.h>
#include <wx/sstream.h>
#include <wx/log.h>
#include "CncConfig.h"
#include "CncFileNameService.h"
#include "BinaryFileParser.h"

//BinaryFileParser/////////////////////////////////////////
BinaryFileParser::BinaryFileParser(const char* fullFileName, BinaryPathHandlerBase* ph)
: CncBinaryTemplateStreamer()
, FileParser(fullFileName)
, pathHandler(ph)
, reportHardwareDifference(true)
, parameterMap()
, xmlParameter(NULL)
, outputFileName(fullFileName)
, sourceContent("")
, dataHeaderContent("")
, totalFileLength(0)
, sourceContentOffset(0)
, dataHeaderOffset(0)
, dataBodyOffset(0)
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
BinaryFileParser::~BinaryFileParser() {
//////////////////////////////////////////////////////////////////
	parameterMap.clear();
	
	if ( xmlParameter != NULL )
		delete xmlParameter;
		
	if ( pathHandler != NULL )
		delete pathHandler;
}
//////////////////////////////////////////////////////////////////
bool BinaryFileParser::checkFileSignature(unsigned char* fileSignature) {
//////////////////////////////////////////////////////////////////
	if ( fileSignature == NULL ) {
		std::cerr << "BinaryFileParser::checkFileSignature(); Empty file signature" << std::endl;
		return false;
	}
	
	wxString fs((char*)fileSignature);
	if ( fs != fileSignatureRef ) {
		std::cerr << "Required: " << fileSignatureRef 	<< std::endl;
		std::cerr << "Received: " << fileSignature 		<< std::endl;
		return false;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool BinaryFileParser::checkHardwareSetup() {
//////////////////////////////////////////////////////////////////
	wxString x, y, z;
	getSetupParameter("HARDWARE_RESOLUTION_AXIS_X", x);
	getSetupParameter("HARDWARE_RESOLUTION_AXIS_X", y);
	getSetupParameter("HARDWARE_RESOLUTION_AXIS_X", z);
	
	double dx, dy, dz;
	x.ToDouble(&dx);
	y.ToDouble(&dy);
	z.ToDouble(&dz);
	
	if (	   cnc::dblCompare(THE_CONFIG->getDisplayFactX(), dx) == false
			|| cnc::dblCompare(THE_CONFIG->getDisplayFactY(), dy) == false
			|| cnc::dblCompare(THE_CONFIG->getDisplayFactZ(), dz) == false
	) {
		std::cerr << "BinaryFileParser::checkHardwareSetup(): Hardware setup difference:" 	<< std::endl
				  << " File        : " << dx << ", " << dy << ", " << dz					<< std::endl
				  << " Application : " << THE_CONFIG->getDisplayFactX() 					<< ", " 
									   << THE_CONFIG->getDisplayFactY() 					<< ", " 
									   << THE_CONFIG->getDisplayFactZ() 					<< std::endl;
		return false;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool BinaryFileParser::read_unit32_t(InputStream& is, uint32_t& ret) {
//////////////////////////////////////////////////////////////////
	const short size = sizeof(uint32_t);
	unsigned char buffer[size];

	if ( readBytes(is, buffer, size) != size ) {
    	std::cerr << "BinaryFileParser::read_unit32_t(): Error while reading an uin32_t value" << std::endl;
    	// more details are already published
		return false;
	}

	memcpy(&ret, buffer, size);
	return true;
}
//////////////////////////////////////////////////////////////////
unsigned int BinaryFileParser::readBytes(InputStream& is, unsigned char* buffer, unsigned int nbBytes) {
//////////////////////////////////////////////////////////////////
	if ( buffer == NULL ) {
		std::cerr << "BinaryFileParser::readBytes(): Invalid buffer" << std::endl;
		return false;
	}

	is.read((char*)buffer, nbBytes);

    if ( !is.good() ) {
    	std::cerr << "BinaryFileParser::readBytes(): Error while reading : Required bytes = "
    	          << nbBytes << ". But only " << is.gcount() << " bytes could be read"
    	          << std::endl;

    	return -1;
	}

	return nbBytes;
}

//////////////////////////////////////////////////////////////////
unsigned int BinaryFileParser::readDataBlock(InputStream& is) {
//////////////////////////////////////////////////////////////////
	uint32_t len;
	if ( read_unit32_t(is, len) == false ) {
		std::cerr << "BinaryFileParser::readDataBlock(): Error while reading length information" << std::endl;
		// more details are already published
		return -1;
	}
	
	unsigned char* buffer = new unsigned char[len];
	len = readBytes(is, buffer, len);
	
	if ( len > 0 ) {
		if ( pathHandler != NULL )
			pathHandler->processCommand(buffer, len);
	}
	
	// destroy data buffer
	delete [] buffer;
	return len;
}
//////////////////////////////////////////////////////////////////
bool BinaryFileParser::readDataBody(InputStream& is) {
//////////////////////////////////////////////////////////////////
	// determine offset and length values
	int32_t bodyStartPos = is.tellg();
	
	is.seekg (0, is.end);
	int32_t bodyLength = is.tellg(); bodyLength -= bodyStartPos;
	int32_t remaining  = bodyLength;
	
	// set pos. back to body start
	is.seekg(bodyStartPos, is.beg);
	
	setCurrentLineNumber(0);
	// parse body - read to file end, block by block
	bool ret = true;
	while ( is.good() ) {
		
		if ( evaluateDebugState() == false )
			return false;
			
		incCurrentLineNumber();
		
		unsigned int datLen = readDataBlock(is);
		if ( datLen > 0 ) {
			// always is fine
			remaining -= ( lenBuffSize + datLen);
		
		} else {
			
			if ( datLen < 0 ) {
				std::cerr << "Error while reading data body" << std::endl;
				// more details already published
				break;
			}
		}
		
		// check eof
		if ( remaining == 0 ) {
			
			int32_t pos1 = is.tellg();
			is.seekg (0, is.end);
			int32_t pos2 = is.tellg();
			
			if ( pos1 != pos2 ) {
				std::cerr << "BinaryFileParser::readDataBody(): Error while reaching end of data body. "
						  << "Remaining = "
						  << remaining << ", "
						  << "Current file pos = "
						  << pos1 << ", "
						  << "End of file = "
						  << pos2 << std::endl;
			}

			break;
		}

		// check remaining length buffer + min. 1 byte data buffer
		if ( remaining < lenBuffSize + 1 ) {
			std::cerr << "BinaryFileParser::readDataBody(): Error while reading data body. "
					  << "Remaining buffer to small. Available size = "
					  << remaining
					  << std::endl;

			break;
		}
	}

	return ret;
}
//////////////////////////////////////////////////////////////////
bool BinaryFileParser::preprocess() {
//////////////////////////////////////////////////////////////////
	if ( inputStream.is_open() )
		inputStream.close();

	inputStream.open(outputFileName.c_str().AsChar(), std::ifstream::binary);

	if ( !inputStream.good() ) {
		std::cerr << "Can't open file: '" << outputFileName << "'" << std::endl;
		return false;
	}

	inputStream.seekg(0, inputStream.end);
	totalFileLength = inputStream.tellg();
	inputStream.seekg(0, inputStream.beg);

	unsigned char fileSignature[lenFileSignature + 1];
	if ( readBytes(inputStream, fileSignature, lenFileSignature) < lenFileSignature ) {
		std::cerr << "Error while reading file signature: File '" << outputFileName << "'" << std::endl;
		return false;
	}
	fileSignature[lenFileSignature] = '\0';
	
	if ( checkFileSignature(fileSignature) == false ) {
		std::cerr << "Invalid file signature: File '" << outputFileName << "'" << std::endl;
		return false;
	}
	
	// ..........................................................................................
	// determine version
	uint32_t version = 0;
	if ( read_unit32_t(inputStream, version) == false) {
		std::cerr << "BinaryFileParser::preprocess(): Error while reading version" << std::endl;
		return false;
	}
	
	if ( version != this->version ) {
		std::cerr << "BinaryFileParser::preprocess(): Version mismatch: " 	<< std::endl
				  << "Received version   : "
				  << version 												<< std::endl
				  << "Implemented version: "
				  << this->version											<< std::endl;
		return false;
	}
	
	// ..........................................................................................
	// determine cpmpression
	uint32_t compression = 0;
	if ( read_unit32_t(inputStream, compression) == false) {
		std::cerr << "BinaryFileParser::preprocess(): Error while reading compression flag" << std::endl;
		return false;
	}
	
	// ..........................................................................................
	// determine offsets
	if ( read_unit32_t(inputStream, sourceContentOffset) == false) {
		std::cerr << "BinaryFileParser::preprocess(): Error while reading source content offset" << std::endl;
		return false;
	}

	if ( read_unit32_t(inputStream, dataHeaderOffset) == false) {
		std::cerr << "BinaryFileParser::preprocess(): Error while reading data header offset" << std::endl;
		return false;
	}

	if ( read_unit32_t(inputStream, dataBodyOffset) == false ) {
		std::cerr << "BinaryFileParser::preprocess(): Error while reading data body offset" << std::endl;
		return false;
	}

	if ( sourceContentOffset > totalFileLength ) {
		std::cerr << "BinaryFileParser::preprocess(): Error while checking data body offset"
				  << "Total file length = "
				  << totalFileLength << ", "
				  << "Source content offset = "
				  << sourceContentOffset
				  << std::endl;
		return false;
	}

	if ( dataHeaderOffset > totalFileLength ) {
		std::cerr << "BinaryFileParser::preprocess(): Error while checking data body offset"
				  << "Total file length = "
				  << totalFileLength << ", "
				  << "Data header offset = "
				  << dataHeaderOffset
				  << std::endl;
		return false;
	}

	if ( dataBodyOffset > totalFileLength ) {
		std::cerr << "BinaryFileParser::preprocess(): Error while checking data body offset"
				  << "Total file length = "
				  << totalFileLength << ", "
				  << "Data body offset = "
				  << dataBodyOffset
				  << std::endl;
		return false;
	}

	// ..........................................................................................
	// calculate block sizes
	const uint32_t sourceContentSize = dataHeaderOffset - sourceContentOffset;
	const uint32_t dataHeaderSize    = dataBodyOffset   - dataHeaderOffset;
	char* buffer = NULL;

	if ( false ) {
		std::cout << sourceContentOffset 	<< "->" 	<< sourceContentSize << std::endl;
		std::cout << dataHeaderOffset 		<< "->" 	<< dataHeaderSize << std::endl;
		std::cout << dataBodyOffset 		<< "-> EOF" << std::endl;
	}

	// ..........................................................................................
	// read source content
	{
		inputStream.seekg(sourceContentOffset, inputStream.beg);
		if ( inputStream.good() == false ) {
			std::cerr << "BinaryFileParser::preprocess(): Error while set source content offset" << std::endl;
			return false;
		}
		
		buffer = new char[sourceContentSize + 1];
		inputStream.read(buffer, sourceContentSize);
		buffer[sourceContentSize] = '\0';
		
		bool ret = true;
		if ( compression != 0 ) 	ret = uncompress(buffer, sourceContentSize + 1, sourceContent);
		else 						sourceContent.assign(buffer);
		
		delete [] buffer;
		
		if ( ret == false ) {
			std::cerr << "BinaryFileParser::preprocess(): Error while uncompressing source content" << std::endl;
			return false;
		}
	}
	
	// ..........................................................................................
	// read data header content
	{
		inputStream.seekg(dataHeaderOffset, inputStream.beg);
		if ( inputStream.good() == false) {
			std::cerr << "BinaryFileParser::preprocess(): Error while set data header content offset" << std::endl;
			return false;
		}
		
		// read data header content
		buffer = new char[dataHeaderSize + 1];
		inputStream.read(buffer, dataHeaderSize);
		buffer[dataHeaderSize] = '\0';
		
		bool ret = true;
		if ( compression != 0 ) 	ret = uncompress(buffer, dataHeaderSize + 1, dataHeaderContent);
		else 						dataHeaderContent.assign(buffer);
		
		delete [] buffer;
		if ( ret == false ) {
			std::cerr << "BinaryFileParser::preprocess(): Error while uncompressing data header" << std::endl;
			return false;
		}
	}
	
	// ..........................................................................................
	// Evaluate XML part
	{
		if ( xmlParameter != NULL )
			delete xmlParameter;
			
		xmlParameter = new wxXmlDocument();
		wxStringInputStream xmlStream(dataHeaderContent.c_str());
		//wxLogNull dummyToSuppressXmlDocErrorMessages;
		if ( xmlParameter->Load(xmlStream) == false ) {
			std::cerr << "BinaryFileParser::preprocess(): Error while creating xml data" << std::endl;
			return false;
		}
	}
	
	expandXmlParameter();
	return checkHardwareSetup();
}
//////////////////////////////////////////////////////////////////
bool BinaryFileParser::spool() {
//////////////////////////////////////////////////////////////////
	// read data body content
	inputStream.seekg(dataBodyOffset, inputStream.beg);
	return readDataBody(inputStream);
}
//////////////////////////////////////////////////////////////////
bool BinaryFileParser::postprocess() {
//////////////////////////////////////////////////////////////////
	// close input stream
	if ( inputStream.is_open() )
		inputStream.close();

	return true;
}
//////////////////////////////////////////////////////////////////
void BinaryFileParser::initNextClientId(long id) {
//////////////////////////////////////////////////////////////////
	if ( pathHandler != NULL )
		pathHandler->initNextClientId(id);
}
//////////////////////////////////////////////////////////////////
void BinaryFileParser::logMeasurementStart() {
//////////////////////////////////////////////////////////////////
	if ( pathHandler != NULL )
		pathHandler->logMeasurementStart();
}
//////////////////////////////////////////////////////////////////
void BinaryFileParser::logMeasurementEnd() {
//////////////////////////////////////////////////////////////////
	if ( pathHandler != NULL )
		pathHandler->logMeasurementEnd();
}
//////////////////////////////////////////////////////////////////
const wxString& BinaryFileParser::getDataBodyView(wxString& content) {
//////////////////////////////////////////////////////////////////
	content.clear();
	
	if ( dataBodyOffset > 0 )
		if ( pathHandler != NULL )
			pathHandler->getViewContent(content);
	
	return content;
}
//////////////////////////////////////////////////////////////////
const wxString& BinaryFileParser::getDataHeaderContent(wxString& content) {
//////////////////////////////////////////////////////////////////
	content.clear();
	
	if ( dataBodyOffset > 0 )
		content.assign(dataHeaderContent.c_str());
	
	return content;
}
//////////////////////////////////////////////////////////////////
const wxString& BinaryFileParser::getSourceContent(wxString& content) {
//////////////////////////////////////////////////////////////////
	content.clear();
	
	if ( dataBodyOffset > 0 )
		content.assign(sourceContent.c_str());
	
	return content;
}
//////////////////////////////////////////////////////////////////
void BinaryFileParser::expandXmlParameter() {
//////////////////////////////////////////////////////////////////
	if ( xmlParameter == NULL ) 
		return;
		
	wxXmlNode* root = xmlParameter->GetRoot();
	if ( root != NULL ) {
		wxXmlNode* sec = root->GetChildren();
		while ( sec != NULL ) {
			// over all sections: source, setup ...
			wxString secName(sec->GetName());
			wxXmlNode* para = sec->GetChildren();
			
			while ( para != NULL ) {
				// over all section parameter nodes
				if ( para->HasAttribute("name") == false )
					continue;
					
				wxString key(para->GetAttribute("name"));
				if ( key.IsEmpty() == true )
					continue;
					
				key.Prepend(".");
				key.Prepend(secName);
				
				parameterMap[key] = para->GetAttribute("value");
				
				para = para->GetNext();
			}
			
			sec = sec->GetNext();
		}
	}
}
//////////////////////////////////////////////////////////////////
const wxString& BinaryFileParser::getXMLParameter(const wxString& section, const wxString& name, wxString& value) {
//////////////////////////////////////////////////////////////////
	value.clear();
	wxString key(wxString::Format("%s.%s", section, name));
	
	if ( parameterMap.find(key) != parameterMap.end() )
		value.assign(parameterMap[key]);
		
	return value;
}
//////////////////////////////////////////////////////////////////
const wxString& BinaryFileParser::getSourceParameter(const wxString& name, wxString& value) {
//////////////////////////////////////////////////////////////////
	return getXMLParameter(XMLSourceNodeName, name, value);
}
//////////////////////////////////////////////////////////////////
const wxString& BinaryFileParser::getSetupParameter(const wxString& name, wxString& value) {
//////////////////////////////////////////////////////////////////
	return getXMLParameter(XMLSetupNodeName, name, value);
}
//////////////////////////////////////////////////////////////////
const wxString& BinaryFileParser::getProcessParameter(const wxString& name, wxString& value) {
//////////////////////////////////////////////////////////////////
	return getXMLParameter(XMLprocessNodeName, name, value);
}
//////////////////////////////////////////////////////////////////
bool BinaryFileParser::extractDataHeaderAsString(const wxString& binFileName, wxString& content) {
//////////////////////////////////////////////////////////////////
	content.clear();
	
	if ( wxFile::Exists(binFileName) == false )
		return false;
	
	BinaryFileParser parser(binFileName);
	if ( parser.preprocess() == false )
		return false;
	
	parser.getDataHeaderContent(content);
	if ( content.IsEmpty() ) {
		std::cerr << "BinaryFileParser::extractParameterAsString(): Empty content!" << std::endl;
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////
bool BinaryFileParser::extractSourceContentAsString(const wxString& binFileName, wxString& content) {
//////////////////////////////////////////////////////////////////
	content.clear();
	
	if ( wxFile::Exists(binFileName) == false )
		return false;
	
	BinaryFileParser parser(binFileName);
	parser.disableHardwareDifferenceReport();
	if ( parser.preprocess() == false )
		return false;
		
	parser.getSourceContent(content);
	if ( content.IsEmpty() ) {
		std::cerr << "BinaryFileParser::extractSourceContentAsString(): Empty content!" << std::endl;
		return false;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool BinaryFileParser::extractSourceContentAsFile(const wxString& binFileName, wxString& sourceFileName) {
//////////////////////////////////////////////////////////////////
	sourceFileName.clear();
	
	if ( wxFile::Exists(binFileName) == false )
		return false;
	
	BinaryFileParser parser(binFileName);
	parser.disableHardwareDifferenceReport();
	
	if ( parser.preprocess() == false )
		return false;
		
	wxString content;
	parser.getSourceContent(content);
	if ( content.IsEmpty() ) {
		std::cerr << "BinaryFileParser::extractSourceContentAsFile(): Empty content!" << std::endl;
		return false;
	}
	
	wxString parameter;
	sourceFileName.assign(CncFileNameService::getTempFileName(TplBinary));
	sourceFileName.append(".");
	sourceFileName.append(parser.getXMLParameter(XMLSourceNodeName, XMLSourceNode_AttribType, parameter));
	
	std::ofstream out(sourceFileName.c_str().AsChar());
	if ( !out.good() ) {
		std::cerr << "BinaryFileParser::extractSourceContentAsFile(): Can't create temp file" << std::endl;
		return false;
	}
	
	out << content;
	out.close();
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool BinaryFileParser::extractViewInfo(ViewType vt, const wxString& binFileName, ViewInfo& vi) {
//////////////////////////////////////////////////////////////////
	vi.viewContent.clear();
	vi.dataHeader.clear();
	vi.sourceContent.clear();
	
	if ( wxFile::Exists(binFileName) == false )
		return false;
		
	BinaryPathHandlerBase* ph = NULL;
	switch ( vt ) {
		case ReadableSteps:		ph = new BinaryPathHandlerHumanReadableView(BinaryPathHandlerHumanReadableView::FormatType::Steps);
								break;
								
		case ReadableMetric:	ph = new BinaryPathHandlerHumanReadableView(BinaryPathHandlerHumanReadableView::FormatType::Metric);
								break;

		case HexCStyle:			ph = new BinaryPathHandlerHexView(BinaryPathHandlerHexView::FormatType::CStyle);
								break;
								
		case HexRaw:			
		default: 				ph = new BinaryPathHandlerHexView(BinaryPathHandlerHexView::FormatType::Raw);
	}
	
	wxASSERT( ph != NULL );
	
	BinaryFileParser parser(binFileName, ph);
	parser.disableHardwareDifferenceReport();

	if ( parser.processRelease() == false )
		return false;
		
	vi.lineNumberTranslater = parser.getLineNumberTranslater();
	 
	parser.getSourceParameter(XMLSourceNode_AttribFile, 	vi.file);
	parser.getSourceParameter(XMLSourceNode_AttribType, 	vi.type);
	parser.getSourceParameter(XMLSourceNode_AttribCheckSum, vi.checksum);
	
	parser.getSourceContent(vi.sourceContent);
	parser.getDataHeaderContent(vi.dataHeader);
	parser.getDataBodyView(vi.viewContent);
	
	return true;
}
