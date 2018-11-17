#include <iostream>
#include "BinaryFIleParser.h"

//////////////////////////////////////////////////////////////////
CncBinaryTemplateParser::CncBinaryTemplateParser(const char* fullFileName)
: CncBinaryTemplateStreamer()
// ,FileParser
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
CncBinaryTemplateParser::~CncBinaryTemplateParser() {
//////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateParser::checkFileSignature(unsigned char* fileSignature) {
//////////////////////////////////////////////////////////////////
	std::string fs((char*)fileSignature);
	return fs.compare(fileSignaturePart);
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateParser::read_unit32_t(InputStream& is, uint32_t& ret) {
//////////////////////////////////////////////////////////////////
	const short size = sizeof(uint32_t);
	unsigned char buffer[size];

	if ( readBytes(is, buffer, size) != size ) {
    	std::cerr << "CncBinaryTemplateParser::read_unit32_t(): Error while reading an uin32_t value" << std::endl;
    	// more details are already published
		return false;
	}

	memcpy(&ret, buffer, size);
	#warning htonl ntohl
	return true;
}
//////////////////////////////////////////////////////////////////
unsigned int CncBinaryTemplateParser::readBytes(InputStream& is, unsigned char* buffer, unsigned int nbBytes) {
//////////////////////////////////////////////////////////////////
	if ( buffer == NULL ) {
		std::cerr << "CncBinaryTemplateParser::readBytes(): Invalid buffer" << std::endl;
		return false;
	}

	is.read((char*)buffer, nbBytes);

    if ( !is.good() ) {
    	std::cerr << "CncBinaryTemplateParser::readBytes(): Error while reading : Required bytes = "
    	          << nbBytes << ". But only " << is.gcount() << " bytes could be read"
    	          << std::endl;

    	return -1;
	}

	return nbBytes;
}

//////////////////////////////////////////////////////////////////
unsigned int CncBinaryTemplateParser::readDataBlock(InputStream& is) {
//////////////////////////////////////////////////////////////////
	uint32_t len;
	if ( read_unit32_t(is, len) == false ) {
		std::cerr << "CncBinaryTemplateParser::readDataBlock(): Error while reading length information" << std::endl;
		// more details are already published
		return -1;
	}

	unsigned char* buffer = new unsigned char[len];
	len = readBytes(is, buffer, len);

	if ( len > 0 ) {

		// do something with data
		std::cout << len << ": ";

		for (unsigned int i=0; i< len; i++)
			std::cout << buffer[i];

		std::cout << std::endl;
	}

	// destroy data buffer
	delete [] buffer;

	return len;
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateParser::readDataBody(InputStream& is) {
//////////////////////////////////////////////////////////////////
	// determine length values
	long bodyStartPos = is.tellg();

	is.seekg (0, is.end);
	long bodyLength = is.tellg() - bodyStartPos;
	long remaining  = bodyLength;

    // set pos. back to body start
    is.seekg(bodyStartPos, is.beg);

    bool ret = true;

	// parse body - read to file end, block by block
	while ( is.good() ) {

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

			long pos1 = is.tellg();
			is.seekg (0, is.end);
			long pos2 = is.tellg();

			if ( pos1 != pos2 ) {
				std::cerr << "CncBinaryTemplateParser::readDataBody(): Error while reaching end of data body. "
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
			std::cerr << "CncBinaryTemplateParser::readDataBody(): Error while reading data body. "
					  << "Remaining buffer to small. Available size = "
					  << remaining
					  << std::endl;

			break;
		}
	}

	return ret;
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateParser::preprocess() {
//////////////////////////////////////////////////////////////////
	if ( inputStream.is_open() )
		inputStream.close();

	inputStream.open(outputFileName.c_str(), std::ifstream::binary);

	if ( !inputStream.good() ) {
		std::cerr << "Can't open file: '" << outputFileName << "'" << std::endl;
		return false;
	}

	inputStream.seekg(0, inputStream.end);
	totalFileLength = inputStream.tellg();
	inputStream.seekg(0, inputStream.beg);

	unsigned char fileSignature[lenFileSignature];
	if ( readBytes(inputStream, fileSignature, lenFileSignature) < lenFileSignature ) {
		std::cerr << "Error while reading file signature: File '" << outputFileName << "'" << std::endl;
		return false;
	}

	if ( checkFileSignature(fileSignature) == false ) {
		std::cerr << "Invalid file signature: File '" << outputFileName << "'" << std::endl;
		return false;
	}

	// determine version
	uint32_t version = 0;
	if ( read_unit32_t(inputStream, version) == false) {
		std::cerr << "CncBinaryTemplateParser::preprocess(): Error while reading source content offset" << std::endl;
		return false;
	}

	if ( version != this->version ) {
		std::cerr << "CncBinaryTemplateParser::preprocess(): Version mismatch: "
				  << "Received version   : "
				  << version
				  << "Implemented version: "
				  << this->version
				  << std::endl;
		return false;
	}

	// determine offsets
	if ( read_unit32_t(inputStream, sourceContentOffset) == false) {
		std::cerr << "CncBinaryTemplateParser::preprocess(): Error while reading source content offset" << std::endl;
		return false;
	}

	if ( read_unit32_t(inputStream, dataHeaderOffset) == false) {
		std::cerr << "CncBinaryTemplateParser::preprocess(): Error while reading data header offset" << std::endl;
		return false;
	}

	if ( read_unit32_t(inputStream, dataBodyOffset) == false ) {
		std::cerr << "CncBinaryTemplateParser::preprocess(): Error while reading data body offset" << std::endl;
		return false;
	}

	if ( sourceContentOffset > totalFileLength ) {
		std::cerr << "CncBinaryTemplateParser::preprocess(): Error while checking data body offset"
				  << "Total file length = "
				  << totalFileLength << ", "
				  << "Source content offset = "
				  << sourceContentOffset
				  << std::endl;
		return false;
	}

	if ( dataHeaderOffset > totalFileLength ) {
		std::cerr << "CncBinaryTemplateParser::preprocess(): Error while checking data body offset"
				  << "Total file length = "
				  << totalFileLength << ", "
				  << "Data header offset = "
				  << dataHeaderOffset
				  << std::endl;
		return false;
	}

	if ( dataBodyOffset > totalFileLength ) {
		std::cerr << "CncBinaryTemplateParser::preprocess(): Error while checking data body offset"
				  << "Total file length = "
				  << totalFileLength << ", "
				  << "Data body offset = "
				  << dataBodyOffset
				  << std::endl;
		return false;
	}


	// calculate block sizes
	const uint32_t sourceContentSize = dataHeaderOffset - sourceContentOffset;
	const uint32_t dataHeaderSize    = dataBodyOffset   - dataHeaderOffset;
	char* buffer = NULL;

	if ( false ) {
		std::cout << sourceContentOffset << "->" << sourceContentSize << std::endl;
		std::cout << dataHeaderOffset << "->" << dataHeaderSize << std::endl;
		std::cout << dataBodyOffset << "-> EOF" << std::endl;
	}

	inputStream.seekg(sourceContentOffset, inputStream.beg);
	if ( inputStream.good() == false ) {
		std::cerr << "CncBinaryTemplateParser::preprocess(): Error while set source content offset" << std::endl;
		return false;
	}

	// read source content
	buffer = new char[sourceContentSize];
	inputStream.read(buffer, sourceContentSize);
	sourceContent.assign(buffer);
	delete []buffer;


	// read data header content
	inputStream.seekg(dataHeaderOffset, inputStream.beg);
	if ( inputStream.good() == false) {
		std::cerr << "CncBinaryTemplateParser::preprocess(): Error while set data header content offset" << std::endl;
		return false;
	}

	// read data header content
	buffer = new char[dataHeaderSize];
	inputStream.read(buffer, dataHeaderSize);
	dataHeaderContent.assign(buffer);
	delete []buffer;

	return true;
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateParser::spool() {
//////////////////////////////////////////////////////////////////
	// read data body content
	inputStream.seekg(dataBodyOffset, inputStream.beg);
	return readDataBody(inputStream);
}
//////////////////////////////////////////////////////////////////
bool CncBinaryTemplateParser::postprocess() {
//////////////////////////////////////////////////////////////////
	// close input stream
	if ( inputStream.is_open() )
		inputStream.close();

	return true;
}
//////////////////////////////////////////////////////////////////
void CncBinaryTemplateParser::initNextClientId(long id) {
//////////////////////////////////////////////////////////////////
	#warning to do
	//pathHandler->
}
//////////////////////////////////////////////////////////////////
void CncBinaryTemplateParser::logMeasurementStart() {
//////////////////////////////////////////////////////////////////
	#warning to do
	//pathHandler->
}
//////////////////////////////////////////////////////////////////
void CncBinaryTemplateParser::logMeasurementEnd() {
//////////////////////////////////////////////////////////////////
	#warning to do
	//pathHandler->
}




/*
 	CncBinaryTemplateStreamer cbs;
	CncBinaryTemplateStreamer::ParameterSet ps;
	ps.fullFileName = "/home/hoelste/xyz1.bin";

	cbs.initNextSourceTemplateFileName(ps);

	{
		const unsigned char* data = (const unsigned char*)"Sonja Mack";
		cbs.appendDataBlock(data, strlen((char*)data));
	}
	{
		const unsigned char* data = (const unsigned char*)"Stefan Hoelzer";
		cbs.appendDataBlock(data, strlen((char*)data));
	}

	for ( int i=0; i<100; i++ ) {
		const unsigned char* data = (const unsigned char*)"Stefan Hoelzer";
		cbs.appendDataBlock(data, strlen((char*)data));
	}


	cbs.finalize();

	ps.fullFileName = "/home/hoelste/xyz2.bin";
	cbs.initNextSourceTemplateFileName(ps);

	{
		const unsigned char* data = (const unsigned char*)"Max Mack";
		cbs.appendDataBlock(data, strlen((char*)data));
	}

	cbs.finalize();

	CncBinaryTemplateParser parser("/home/hoelste/xyz1.bin");
	parser.test();
	parser.trace();


 */