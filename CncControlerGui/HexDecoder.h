#ifndef HEX_DECODER
#define HEX_DECODER

#include <iostream>
#include <wx/tokenzr.h>
#include "SerialPort.h"
#include "CncArduino.h"

//////////////////////////////////////////////////////////////////
class HexDecoder {
	
	public:
		/////////////////////////////////////////////////////////
		const char* decodeContollerResult(int ret) {
			switch ( (const unsigned char)ret ) {
				case RET_NULL:		return "Fetch Result";
				case RET_SENT:		return "Sent data";
				case RET_SOH:		return "Fetch Data";

				case RET_OK:
				case RET_MORE:
				case RET_ERROR:
				case RET_LIMIT:
				case RET_INTERRUPT:
				case RET_HALT:
				case RET_QUIT:		return ArduinoPIDs::getPIDLabel((const unsigned char)ret);
			}
			
			static wxString s(wxString::Format("Unkonwn fetch result value(%d)", ret));
			return s.c_str();
		}
		
		/////////////////////////////////////////////////////////
		static const wxString& reorderHexInt32String(wxString& hexToken) {
			if ( hexToken.length() > 8 ) {
				std::cerr << "HexDecoder::reorderHexInt32String: Length error: " << hexToken.length() << " - " << hexToken << std::endl;
				return _T("00000000");
			}
			
			wxString unordered(hexToken);
			unordered.append(wxString('0', 8 - hexToken.length()));
			
			char hex[9];
			hex[0] = (char)unordered[6];
			hex[1] = (char)unordered[7];
			hex[2] = (char)unordered[4];
			hex[3] = (char)unordered[5];
			hex[4] = (char)unordered[2];
			hex[5] = (char)unordered[3];
			hex[6] = (char)unordered[0];
			hex[7] = (char)unordered[1];
			hex[8] = '\0';
			
			hexToken.assign(hex);
			return hexToken;
		}
		
		/////////////////////////////////////////////////////////
		static int32_t decodeHexValueAsInt32(const wxString& hexToken) {
			//  examples are: 00000000,  2C010000,  FAF4FFFF
			if ( hexToken.length() != 8 ) {
				std::cerr << "HexDecoder::decodeHexValueAsInt32: Length error: " << hexToken.length() << " - " << hexToken << std::endl;
				return 0;
			}
			
			for ( unsigned int i = 0; i < hexToken.length(); i++ ) {
				if ( isxdigit((char)(hexToken[i])) == 0 ) {
					std::cerr << "HexDecoder::decodeHexValueAsInt32: Digit error at pos: " << i << " - " << hexToken << std::endl;
					return 0;
				}
			}
			
			int32_t ret = 0;
			sscanf(hexToken, "%8X", &ret);
			return ret;
		}
		
		/////////////////////////////////////////////////////////
		static int16_t decodeHexValueAsInt16(const wxString& hexToken) {
			//  examples are: 0000,  2C01,  FAF4
			if ( hexToken.length() != 4 ) {
				std::cerr << "HexDecoder::decodeHexValueAsInt16: Length error: " << hexToken.length() << " - " << hexToken << std::endl;
				return 0;
			}
			
			for ( unsigned int i = 0; i < hexToken.length(); i++ ) {
				if ( isxdigit((char)(hexToken[i])) == 0 ) {
					std::cerr << "HexDecoder::decodeHexValueAsInt16: Digit error at pos: " << i << " - " << hexToken << std::endl;
					return 0;
				}
			}
			
			int32_t ret = 0;
			sscanf(hexToken, "%4X", &ret);
			return (int16_t)ret;
		}
		
		/////////////////////////////////////////////////////////
		static int8_t decodeHexValueAsInt8(const wxString& hexToken) {
			//  examples are: 00,  2C,  FA
			if ( hexToken.length() != 2 ) {
				std::cerr << "HexDecoder::decodeHexValueAsInt8: Length error: " << hexToken.length() << " - " << hexToken << std::endl;
				return 0;
			}
			
			for ( unsigned int i = 0; i < hexToken.length(); i++ ) {
				if ( isxdigit((char)(hexToken[i])) == 0 ) {
					std::cerr << "HexDecoder::decodeHexValueAsInt8: Digit error at pos: " << i << " - " << hexToken << std::endl;
					return 0;
				}
			}
			
			int32_t ret = 0;
			sscanf(hexToken, "%2X", &ret);
			return (int8_t)ret;
		}
		
		
		/////////////////////////////////////////////////////////
		static int decodeHexValueAsInteger(const wxString& hexToken) {
			int ret = 0;
			sscanf(hexToken, "%X", &ret);
			return ret;
		}
		
		/////////////////////////////////////////////////////////
		static char decodeHexValueAsCharacter(const wxString& hexToken) {
			return (char)decodeHexValueAsInteger(hexToken);
		}
		
		/////////////////////////////////////////////////////////
		static const char* decodeHexValueAsCharacterString(const wxString& hexToken, wxString& ret) {
			int val = decodeHexValueAsInteger(hexToken); 
			
			if ( val >= 0 && val < 32 )	ret.assign(wxString::Format("'\%d'", val));
			else						ret.assign(wxString::Format("'%c'",  decodeHexValueAsInteger(hexToken)));
			
			return ret;
		}

		/////////////////////////////////////////////////////////
		static const char* decodeHexValueAsArduinoPID(const wxString& hexToken) {
			int val = decodeHexValueAsInteger(hexToken);
			return ArduinoPIDs::getPIDLabelWithDefault(val, std::string(wxString::Format("%d", val).c_str()));
		}
		
		/////////////////////////////////////////////////////////
		static const char* decodeHexStringAsArduinoPIDs(const wxString& hexToken, wxString& ret, const char delimiter = ' ') {
			wxStringTokenizer tokenizer(hexToken, wxString::Format("%c", delimiter));
			unsigned int count = 0;
			while ( tokenizer.HasMoreTokens() ) {
				wxString token = tokenizer.GetNextToken();
				
				if ( token.IsEmpty() == false ) {
					if ( count++ > 0 )
						ret << ", ";
						
					int val = decodeHexValueAsInteger(hexToken);
					ret << ArduinoPIDs::getPIDLabelWithDefault(val, std::string(wxString::Format("%d", val).c_str()));
				}
			}
			ret << "\n";
			return ret;
		}

		/////////////////////////////////////////////////////////
		static const char* decodeHexStringAsIntegers(const wxString& hexToken, wxString& ret, const char delimiter = ' ') {
			wxStringTokenizer tokenizer(hexToken, wxString::Format("%c", delimiter));
			unsigned int count = 0;
			while ( tokenizer.HasMoreTokens() ) {
				wxString token = tokenizer.GetNextToken();
				
				if ( token.IsEmpty() == false ) {
					if ( count++ > 0 )
						ret << ", ";
					ret << decodeHexValueAsInteger(token);
				}
			}
			ret << "\n";
			return ret;
		}
		
		/////////////////////////////////////////////////////////
		static const char* decodeHexStringAsCharacters(const wxString& hexToken, wxString& ret, const char delimiter = ' ') {
			wxStringTokenizer tokenizer(hexToken, wxString::Format("%c", delimiter));
			while ( tokenizer.HasMoreTokens() ) {
				wxString token = tokenizer.GetNextToken();
				
				if ( token.IsEmpty() == false ) {
					ret << decodeHexValueAsCharacter(token);
				}
			}
			ret << "\n";
			return ret;
		}
		
		/////////////////////////////////////////////////////////
		static const char* decodeHexStringAsInt32s(const wxString& hexToken, wxString& ret, const char delimiter = ' ') {
			std::string s(hexToken.c_str());
			size_t n = std::count(s.begin(), s.end(), ' ') + 1;
			
			if ( n%4 == 0 ) {
				wxStringTokenizer tokenizer(hexToken, wxString::Format("%c", delimiter));
				unsigned int count = 0;
				wxString value;
				
				while ( tokenizer.HasMoreTokens() ) {
					wxString token = tokenizer.GetNextToken();
					
					if ( token.IsEmpty() == false ) {
						count++;
						
						switch ( count%4 ) {
							case 1:	
							case 2:
							case 3: value << token; 
									break;
									
							case 0: value << token;
									if ( ret.length() != 0 ) 
										ret << ", ";
									
									ret << decodeHexValueAsInt32(reorderHexInt32String(value));
									value.clear();
									break;
						}
					}
				}
			} else {
				decodeHexStringAsArduinoPIDs(hexToken, ret, delimiter);
			}
			
			ret << "\n";
			return ret;
		}
};

//////////////////////////////////////////////////////////////////
class SpyHexDecoder : public HexDecoder {
	
	protected:
		wxString hexString;
		
		unsigned char context;
		unsigned char cmd;
		unsigned char pid;
		unsigned int  index;
		unsigned int  portion;
		
		static const unsigned int IDX_DEF_PID				= 2;
		static const unsigned int IDX_DEF_SIZE				= 3;
		
		static const unsigned int IDX_MSG_TYPE				= 3;
		
		static const unsigned int IDX_HB_SIZE				= 3;
		
		static const unsigned int IDX_GETTER_PID			= 3;
		static const unsigned int IDX_GETTER_SIZE			= 4;
		
	public:
	
		struct Details {
			
			enum Type { DT_UNKNOWN = 0, DT_INBOUND = 1, DT_OUTBOUND = 2};
			Type type = DT_UNKNOWN;
			
			wxString 	context;
			wxString	cmd;
			wxString	pid;
			wxString	index;
			wxString	portion;
			wxString	more;
			
			void clear() {
				context.clear();
				cmd.clear();
				pid.clear();
				index.clear();
				portion.clear();
				more.clear();
			}
		};
		
		//////////////////////////////////////////////////////////
		SpyHexDecoder(const wxString& contextInfo, const wxString& hs) 
		: HexDecoder()
		, hexString(hs)
		, context(0)
		, cmd(0)
		, pid(0)
		, index(0)
		, portion(0)
		{
			unsigned int counter = 0;
			wxStringTokenizer tokenizer(contextInfo, " ");
			while ( tokenizer.HasMoreTokens() ) {
				wxString token = tokenizer.GetNextToken();
				
				switch ( counter ) {
					case 0:	cmd 	= decodeHexValueAsInteger(token); 
							break;
							
					case 1:	context = decodeHexValueAsInteger(token); 
							break;
							
					case 2: pid     = decodeHexValueAsInteger(token); 
							break;
							
					case 3: index   = decodeHexValueAsInteger(token);
							portion = decodeHexValueAsInteger(token); 
							break;
				}
				
				counter++;
			}
		}
		
		//////////////////////////////////////////////////////////
		virtual ~SpyHexDecoder() {
		}
		
		//////////////////////////////////////////////////////////
		void decode(SpyHexDecoder::Details& ret) {
			if ( context == 0 )
				return;
				
			switch ( ret.type ) {
				case Details::DT_OUTBOUND:	decodeOutbound(ret);
											break;
									
				case Details::DT_INBOUND:	decodeInbound(ret);
											break;
											
				case Details::DT_UNKNOWN:
				default:					ret.more.assign("decode(SpyHexDecoder::Details& ret): Unknown type, nothing will be done!");
				
			}
			
			ret.more.Replace("\n", "; ", true);
			ret.more.Replace("; ;", "",  true);
			return;
		}
		
	private:
		
		unsigned int getByteCount(const wxString& hexValues);
		bool readNextHexBytes(wxString& hexValues, unsigned int count, wxString& ret);
		bool skipNextHexBytes(wxString& hexValues, unsigned int count);
		
		void decodeOutbound(SpyHexDecoder::Details& ret);
		void decodeInbound(SpyHexDecoder::Details& ret);
		
		void decodeValuesDefault(SpyHexDecoder::Details& ret, wxString& restToken);
		void decodeMoveSeqOutbound(SpyHexDecoder::Details& ret, wxString& restToken);

};

#endif