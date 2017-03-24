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
			switch ( (const char)ret ) {
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
									ret << ntohl(decodeHexValueAsInteger(value));
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
			
			// Deconde command
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
			switch ( (const char)t ) {
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
			
			return returnValue;
		}
};

#endif