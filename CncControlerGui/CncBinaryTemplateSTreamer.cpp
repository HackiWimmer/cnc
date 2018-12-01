#include <iostream>
#include <sstream> 
#include <wx/file.h>
#include <wx/datetime.h>
#include <wx/mstream.h>
#include <wx/sstream.h>
#include <wx/zstream.h>
#include "CncSha1Wrapper.h"
#include "CncFileNameService.h"
#include "CncBinaryTemplateStreamer.h"

//////////////////////////////////////////////////////////////////
CncBinaryTemplateStreamer::CncBinaryTemplateStreamer()
: readyToStream(false)
, compress(true)
, parameter()
, tmpFileNameDataBody("")
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
CncBinaryTemplateStreamer::~CncBinaryTemplateStreamer() {
//////////////////////////////////////////////////////////////////
	if ( readyToStream == true ) {
		std::cerr << "CncBinaryTemplateStreamer::~CncBinaryTemplateStreamer(): Is still ready to stream" << std::endl;
	}

	destroyLastDataBodyStream();
}
//////////////////////////////////////////////////////////////////
void CncBinaryTemplateStreamer::createNextDataBodyStream() {
//////////////////////////////////////////////////////////////////
	destroyLastDataBodyStream();
	
	tmpFileNameDataBody.assign(CncFileNameService::getTempFileName(TplBinary));
	dataBodyStream.open(tmpFileNameDataBody.c_str(), std::ifstream::out | std::ifstream::binary);
}
//////////////////////////////////////////////////////////////////
void CncBinaryTemplateStreamer::destroyLastDataBodyStream() {
//////////////////////////////////////////////////////////////////
	dataBodyStream.close();

	if ( tmpFileNameDataBody.compare("") != 0 ) {
		if ( remove(tmpFileNameDataBody.c_str()) != 0 ) {
			std::cerr << "CncBinaryTemplateStreamer::destroyLastDataBodyStream(): Can't delete '"
					  << tmpFileNameDataBody
					  << "'" <<std::endl;
		}
	}
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateStreamer::fileExists(const char* fullFileName) {
//////////////////////////////////////////////////////////////////
	return wxFile::Exists(fullFileName);
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateStreamer::initNextSourceTemplateFileName(const ParameterSet& para) {
//////////////////////////////////////////////////////////////////
	// copy the parameters
	parameter = para;
	
	// reset output streams
	if ( fileStream.is_open() )
		fileStream.close();
	
	fileStream.open(para.fullFileName.c_str(), std::ifstream::out | std::ifstream::binary);
	if ( !fileStream.is_open() ) {
		std::cerr << "CncBinaryTemplateStreamer::initNextSourceTemplateFileName(): Can't create file: '"
				  << para.fullFileName.c_str()
				  << "'" << std::endl;
		return false;
	}

	createNextDataBodyStream();

	readyToStream = true;
	return readyToStream;
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateStreamer::finalize() {
//////////////////////////////////////////////////////////////////
	if ( readyToStream == false ) {
		std::cerr << "CncBinaryTemplateStreamer::finalize(): Isn't ready to stream" << std::endl;
		return false;
	}

	DataContainer dc;
	prepareDataContainer(dc);

	if ( appendFileHeader(dc) == false )	return false;
	if ( appendFileSource(dc) == false )	return false;
	if ( appendBodyHeader(dc) == false )	return false;

	fileStream.flush();
	fileStream.close();

	dataBodyStream.flush();

	// concatenate both files
	std::ofstream a(parameter.fullFileName.c_str(), std::ifstream::app | std::ifstream::binary);
	std::ifstream b(tmpFileNameDataBody.c_str(), 	std::ifstream::in  | std::ifstream::binary);

	if ( a.good() && b.good() ) {
		a << b.rdbuf();
		
	} else {
		std::cerr << "CncBinaryTemplateStreamer::finalize(): Error while concatenation"
				  << std::endl;
	}

	a.close();
	b.close();

	readyToStream = false;
	return true;
}
//////////////////////////////////////////////////////////////////
void CncBinaryTemplateStreamer::write_uint32_t(OutputStream& o, const uint32_t value) {
//////////////////////////////////////////////////////////////////
	char buf[sizeof(uint32_t)];
	memcpy(buf, &value, sizeof(uint32_t));
	o.write(buf, sizeof(uint32_t));
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateStreamer::prepareDataContainer(DataContainer& dc) {
//////////////////////////////////////////////////////////////////
	dc.signature = fileSignatureRef;
	if ( evaluateSourceContent(dc) == false ) {
		std::cerr << "CncBinaryTemplateStreamer::prepareDataContainer(): evaluate source content failed" << std::endl;
		return false;
	}

	if ( evaluateDataHeader(dc) == false ) {
		std::cerr << "CncBinaryTemplateStreamer::prepareDataContainer(): evaluate data header failed" << std::endl;
		return false;
	}

	uint32_t offset = 0;
	offset += lenFileSignature;
	offset += sizeof(uint32_t);	// version
	offset += sizeof(uint32_t);	// compression flag
	offset += sizeof(uint32_t);	// offset to source
	offset += sizeof(uint32_t);	// offset to data header
	offset += sizeof(uint32_t); // offset to data body

	dc.sourceContentOffset 	= offset;
	dc.dataHeaderOffset		= dc.sourceContentOffset + dc.sourceContentSize;
	dc.dataBodyOffset		= dc.dataHeaderOffset    + dc.dataHeaderSize;

	return true;
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateStreamer::appendFileHeader(DataContainer& dc) {
//////////////////////////////////////////////////////////////////
	fileStream.write(dc.signature.c_str(), lenFileSignature);
	write_uint32_t(fileStream, version);
	write_uint32_t(fileStream, (uint32_t)compress);
	write_uint32_t(fileStream, dc.sourceContentOffset);
	write_uint32_t(fileStream, dc.dataHeaderOffset);
	write_uint32_t(fileStream, dc.dataBodyOffset);

	if ( false ) {
		std::cout << "CncBinaryTemplateStreamer::appendFileHeader: source content offset: " << dc.sourceContentOffset << std::endl;
		std::cout << "CncBinaryTemplateStreamer::appendFileHeader: data header offset   : " << dc.dataHeaderOffset << std::endl;
		std::cout << "CncBinaryTemplateStreamer::appendFileHeader: data body offset     : " << dc.dataBodyOffset << std::endl;
	}

	return true;
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateStreamer::appendFileSource(DataContainer& dc) {
//////////////////////////////////////////////////////////////////
	if ( dc.sourceContent == NULL ) {
		std::cerr << "CncBinaryTemplateStreamer::appendFileSource(): Empty content" << std::endl;
		return false;
	}
	
	fileStream.write(dc.sourceContent, dc.sourceContentSize);
	fileStream.flush();
	
	delete [] dc.sourceContent;
	dc.sourceContent     = NULL;
	dc.sourceContentSize = 0;
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateStreamer::appendBodyHeader(DataContainer& dc) {
//////////////////////////////////////////////////////////////////
	if ( dc.dataHeader == NULL ) {
		std::cerr << "CncBinaryTemplateStreamer::appendBodyHeader(): Empty content" << std::endl;
		return false;
	}
	
	fileStream.write(dc.dataHeader, dc.dataHeaderSize);
	fileStream.flush();
	
	delete [] dc.dataHeader;
	dc.dataHeader = NULL;
	dc.dataHeader = 0;
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateStreamer::appendDataBlock(unsigned char* buffer, uint32_t nbByte) {
//////////////////////////////////////////////////////////////////
	if ( nbByte == 0 ) {
		// nothing to append
		return true;
	}

	if ( readyToStream == false ) {
		// appending is off
		return true;
	}

	if ( buffer == NULL ) {
		std::cerr << "CncBinaryTemplateStreamer::appendDataBlock(): Invalid buffer" << std::endl;
		return false;
	}
	
	unsigned char lenBuf[lenBuffSize];
	memcpy(lenBuf, &nbByte, lenBuffSize);

	dataBodyStream.write((const char*)lenBuf, lenBuffSize);
	dataBodyStream.write((const char*)buffer, nbByte);
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateStreamer::evaluateSourceContent(DataContainer& dc) {
//////////////////////////////////////////////////////////////////
	std::ifstream sourceContent(parameter.SRC.fileName, std::ifstream::in);
	
	wxString content;
	if ( sourceContent.good() ) {
		std::stringstream ss;
		ss << sourceContent.rdbuf();
		content.assign(ss.str());
	
	} else {
		content.assign("SourceFile: '");
		content.append(parameter.SRC.fileName);
		content.append("' could not be found");
	}
	
	// prepare buffer
	if ( dc.sourceContent != NULL )
		delete [] dc.sourceContent;
	
	if ( compress == true ) {
		wxMemoryOutputStream out;
		wxZlibOutputStream zlib(out);
		zlib.Write(content.c_str(), content.length());
		zlib.Close();
		
		uint32_t size = out.GetLength();
		dc.sourceContent = new char[size + 1];
		out.CopyTo(dc.sourceContent, size);
		dc.sourceContent[size] = '\0';
		dc.sourceContentSize   = size;
		
	} else {
		uint32_t size = content.length();
		dc.sourceContent = new char[size + 1];
		strcpy(dc.sourceContent, content.c_str());
		dc.sourceContent[size] = '\0';
		dc.sourceContentSize   = size;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateStreamer::evaluateDataHeader(DataContainer& dc) {
//////////////////////////////////////////////////////////////////
	std::stringstream ss;

	const char * TAB1 = " ";
	const char * TAB2 = "  ";

	#define LOG_PARAM(name, value) \
		o << TAB2 << "<parameter name=\"" << name << "\" value=\"" << value << "\"/>\n";

	auto serializeSource = [&](std::ostream& o) {
		wxString sha1;
		
		o << TAB1 << "<" << XMLSourceNodeName << ">\n";
			LOG_PARAM(XMLSourceNode_AttribFile, 		parameter.SRC.fileName);
			LOG_PARAM(XMLSourceNode_AttribType, 		parameter.SRC.fileType);
			LOG_PARAM(XMLSourceNode_AttribCheckSum, 	CncFileSha1::checksum(parameter.SRC.fileName, sha1));
		o << TAB1 << "</" << XMLSourceNodeName << ">\n";
	};
	
	auto serializeSetup = [&](std::ostream& o) {
		o << TAB1 << "<" << XMLSetupNodeName << ">\n";
			LOG_PARAM(XMLSetupNode_AttribResX, parameter.SET.hardwareResX);
			LOG_PARAM(XMLSetupNode_AttribResY, parameter.SET.hardwareResY);
			LOG_PARAM(XMLSetupNode_AttribResZ, parameter.SET.hardwareResZ);
		o << TAB1 << "</" << XMLSetupNodeName << ">\n";
	};
	
	auto serializeProcess = [&](std::ostream& o) {
		wxDateTime unow(wxDateTime::UNow());
		wxString ts(wxString::Format("%s.%u", unow.FormatISOCombined(' '), unow.GetMillisecond()));
		
		o << TAB1 << "<" << XMLprocessNodeName << ">\n";
			LOG_PARAM(XMLprocess_AtribUser, 	parameter.PRC.user);
			LOG_PARAM(XMLprocess_AtribCreated, 	ts);
		o << TAB1 << "</" << XMLprocessNodeName << ">\n";
	};

	ss << "<root>\n";
		serializeSource(ss);
		serializeSetup(ss);
		serializeProcess(ss);
	ss << "</root>\n";
	
	// prepare buffer
	if ( dc.dataHeader != NULL )
		delete [] dc.dataHeader;
	
	if ( compress == true ) {
		wxMemoryOutputStream out;
		wxZlibOutputStream zlib(out);
		zlib.Write(ss.str().c_str(), strlen(ss.str().c_str()));
		zlib.Close();
		
		uint32_t size = out.GetLength();
		dc.dataHeader = new char[size + 1];
		out.CopyTo(dc.dataHeader, size);
		dc.dataHeader[size] = '\0';
		dc.dataHeaderSize = size;

	} else {
		uint32_t size = strlen(ss.str().c_str());
		dc.dataHeader = new char[size + 1];
		strcpy(dc.dataHeader, ss.str().c_str());
		dc.dataHeader[size] = '\0';
		dc.dataHeaderSize = size;
		
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateStreamer::uncompress(char* buf, int32_t size, wxString& content) {
//////////////////////////////////////////////////////////////////
	if ( buf == NULL ) {
		std::cerr << "CncBinaryTemplateStreamer::uncompress(); Invalid buffer!" << std::endl;
		return false;
	}
	
	wxMemoryInputStream in(buf, size);
	wxZlibInputStream zlib(in);
	
	char dataBlock[1024];
	size_t n;

	while ( ( n = zlib.Read(dataBlock, sizeof(dataBlock)).LastRead() ) > 0 )
		content.Append(dataBlock, n);

	return true;
}