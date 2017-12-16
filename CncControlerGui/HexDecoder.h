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
				case RET_NULL:		return "Controller Fetch Result";
				case RET_OK: 		return "RET_OK";
				case RET_ERROR:		return "RET_ERROR";
				case RET_SOT:		return "RET_SOT";
				case RET_SOH:		return "RET_SOH";
				case RET_MSG:		return "RET_MSG";
				case RET_SENT:		return "Sent data";
			}
			
			static wxString s(wxString::Format("Unkonwn fetch result value(%d)", ret));
			return s.c_str();
		}
		
		/////////////////////////////////////////////////////////
		static const wxString& reorderHexInt32String(wxString& hexToken) {
			if ( hexToken.length() > 8 ) {
				cerr << "HexDecoder::reorderHexInt32String: Length error: " << hexToken.length() << " - " << hexToken << endl;
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
			if ( hexToken.length() > 8 ) {
				cerr << "HexDecoder::decodeHexValueAsInteger: Length error: " << hexToken.length() << " - " << hexToken << endl;
				return 0;
			}
			
			for ( unsigned int i = 0; i < hexToken.length(); i++ ) {
				if ( isxdigit((char)(hexToken[i])) == 0 ) {
					cerr << "HexDecoder::decodeHexValueAsInteger: Digit error at pos: " << i << " - " << hexToken << endl;
					return 0;
				}
			}
			
			int32_t ret = 0;
			sscanf(hexToken, "%8X", &ret);
			return ret;
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
		static const char* decodeHexValueAsArduinoPID(const wxString& hexToken) {
			return ArduinoPIDs::getPIDLabel(decodeHexValueAsInteger(hexToken));
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
					ret << ArduinoPIDs::getPIDLabel(decodeHexValueAsInteger(token));
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
		
		/////////////////////////////////////////////////////////
		static const char* decodeHexStringAsSentData(const wxString& hexToken, wxString& ret, const char delimiter = ' ') {
			std::string s(hexToken.c_str());
			size_t n = std::count(s.begin(), s.end(), ' ') + 1;
			
			// Decode command
			unsigned char cmd = '\0';
			if ( n >= 1 ) {
				cmd = decodeHexValueAsCharacter(hexToken);
				ret << "Cmd: ";
				ret << cmd;
			}
			
			if ( n > 1 )  {
				ret << ", ";
				
				// strip cmd token
				wxString restToken(hexToken.SubString(3, hexToken.length()-1));
				
				// Decode setter type
				if ( cmd == 'S' ) {
					ret << decodeHexValueAsArduinoPID(restToken);
					ret << ": ";
					restToken = restToken.SubString(3, restToken.length()-1);
				}
				
				// Recalc token count
				s = restToken.c_str();
				n = std::count(s.begin(), s.end(), ' ') + 1;
				
				if ( n%4 == 0 ) {
					wxStringTokenizer tokenizer(restToken, wxString::Format("%c", delimiter));
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
										if ( count > 4 )
											ret << ", ";
										ret << decodeHexValueAsInteger(value);
										value.clear();
										break;
							}
						}
					}
				} else {
					decodeHexStringAsIntegers(restToken, ret, delimiter);
				}
			}
			
			ret << "\n";
			return ret;
		}
};

//////////////////////////////////////////////////////////////////
class SpyHexDecoder : public HexDecoder {
	
	protected:
		wxString returnValue;
		wxString type;
		wxString hexString;
		
	public:
		//////////////////////////////////////////////////////////
		SpyHexDecoder(const wxString& t, const wxString& hs) 
		: HexDecoder()
		, returnValue()
		, type(t)
		, hexString(hs)
		{
		}
		
		//////////////////////////////////////////////////////////
		virtual ~SpyHexDecoder() {
			
		}
		
		//////////////////////////////////////////////////////////
		const char* decode() {
			returnValue.clear();
			int t = decodeHexValueAsInteger(type);
			returnValue << "Type: ";
			returnValue << decodeContollerResult(t);
			returnValue << "\n";
			
			wxString temp;
			switch ( (const unsigned char)t ) {
				case RET_NULL:	returnValue << "Read value: ";
								returnValue << decodeContollerResult(decodeHexValueAsInteger(hexString));
								break;
				case RET_OK:
				case RET_ERROR: returnValue << "Read value(s): ";
								returnValue << decodeHexStringAsIntegers(hexString, temp);
								break;
								
				case RET_SOT:	returnValue << "Read text: ";
								returnValue << decodeHexStringAsCharacters(hexString, temp);
								break;
								
				case RET_SOH:	returnValue << "Read value(s): ";
								returnValue << decodeHexStringAsInt32s(hexString, temp);
								break;
								
				case RET_MSG:	returnValue << "Read message: ";
								returnValue << decodeHexStringAsCharacters(hexString, temp);
								break;
								
				case RET_SENT:	returnValue << "Written value(s): ";
								returnValue << decodeHexStringAsSentData(hexString, temp);
								break;
								
				default:		returnValue << "Unknown fetch type: ";
								returnValue << t;
								returnValue << ", Value(s): ";
								returnValue << decodeHexStringAsIntegers(hexString, temp);
			}
			
			while ( returnValue.IsEmpty() == false && returnValue.Last() == '\n' )
				returnValue.assign(returnValue.SubString(0, returnValue.length() - 2));
			
			return returnValue;
		}
};

#endif