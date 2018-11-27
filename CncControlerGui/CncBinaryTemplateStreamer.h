#ifndef CNC_BIN_TPL_STREAMER_H
#define CNC_BIN_TPL_STREAMER_H

#include <cstring>
#include <fstream>
#include <stdint.h>

#define XMLSourceNodeName			"source"
#define XMLSourceNode_AttribFile		"FILE"
#define XMLSourceNode_AttribType		"TYPE"
#define XMLSourceNode_AttribCheckSum	"CHECKSUM"

#define XMLSetupNodeName			"setup"
#define XMLSetupNode_AttribResX			"HARDWARE_RESOLUTION_AXIS_X"
#define XMLSetupNode_AttribResY			"HARDWARE_RESOLUTION_AXIS_Y"
#define XMLSetupNode_AttribResZ			"HARDWARE_RESOLUTION_AXIS_Z"

#define XMLprocessNodeName			"process"
#define XMLprocess_AtribCreated			"CREATED"
#define XMLprocess_AtribUser			"USER"

class CncBinaryTemplateStreamer {
	
	typedef std::ofstream OutputStream;
	
	protected:

		//                                		   1       2       3       4       5       6       7       8 
		//                                		   0123456701234567012345670123456701234567012345670123456701234567
		const char* fileSignatureRef  			= "CncBinaryTemplateFile...........................................";
		const unsigned short lenFileSignature 	= 64;
		const unsigned short lenBuffSize 		= sizeof(uint32_t);
		const uint32_t version					= 1001;
		
		bool fileExists(const char*);

	public:
		
		struct ParameterSet {
			
			std::string fullFileName;
			
			struct Source {
				wxString fileName;
				wxString fileType;
			} SRC;

			struct Setup {
				float hardwareResX	= 0.0;
				float hardwareResY	= 0.0;
				float hardwareResZ	= 0.0;
				 
			} SET;
			
			struct Process {
				std::string user;
				 
			} PRC;
		};
		
		CncBinaryTemplateStreamer();
		virtual ~CncBinaryTemplateStreamer();
		
		bool initNextSourceTemplateFileName(const ParameterSet& para);
		bool appendDataBlock(unsigned char* buffer, uint32_t nbByte);
		bool finalize();
		bool isReadyToStream() { return readyToStream; }
		
		const char* getOutboundFileName() { return parameter.fullFileName.c_str(); }
		
		static bool uncompress(char* buf, int32_t size, wxString& content);
	
	private:
	
		bool readyToStream;
		bool compress;
		ParameterSet parameter;
		std::string tmpFileNameDataBody;
		
		OutputStream fileStream;
		OutputStream dataBodyStream;
		
		struct DataContainer {
			
			std::string signature;
			
			char* sourceContent				= NULL;
			char* dataHeader				= NULL;
			uint32_t sourceContentSize		= 0;
			uint32_t dataHeaderSize			= 0;

			uint32_t sourceContentOffset	= 0;
			uint32_t dataHeaderOffset		= 0;
			uint32_t dataBodyOffset			= 0;
		};
		
		inline void createNextDataBodyStream();
		inline void destroyLastDataBodyStream();

		inline void write_uint32_t(OutputStream& o, const uint32_t value);
		
		inline bool prepareDataContainer(DataContainer& dc);
		inline bool appendFileSource(DataContainer& dc);
		inline bool appendBodyHeader(DataContainer& dc);
		inline bool appendFileHeader(DataContainer& dc);
		
		inline bool evaluateSourceContent(DataContainer& dc);
		inline bool evaluateDataHeader(DataContainer& dc);
		
		
		
};

#endif