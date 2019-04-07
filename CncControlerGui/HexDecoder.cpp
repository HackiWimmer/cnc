#include "HexDecoder.h"

//////////////////////////////////////////////////////////
void SpyHexDecoder::decodeOutbound(SpyHexDecoder::Details& ret) {
//////////////////////////////////////////////////////////
	wxString temp;
	
	std::string helpStr(hexString.c_str());
	size_t byteCount = std::count(helpStr.begin(), helpStr.end(), ' ') + 1;
	wxString restToken(hexString);
	
	ret.clear();
	ret.context.assign(decodeContollerResult(context));
	
	const char cmd = decodeHexValueAsCharacter(restToken);
	ret.cmd.assign(wxString::Format("[%c] %s", cmd, ArduinoCMDs::getCMDLabel((unsigned int)cmd)));
	
	if ( ArduinoCMDs::exists((unsigned int)cmd) == false )
		return;
	
	if ( byteCount <= 1 )
		return;
	
	// reclacTokenCount
	auto recalcTokenCount = [&]() {
		restToken.assign(restToken.SubString(3, restToken.length() - 1));
		restToken.Trim(true).Trim(false);
		
		if ( restToken.IsEmpty() )
			return (size_t)0;
			
		helpStr = restToken.c_str();
		size_t ret = std::count(helpStr.begin(), helpStr.end(), ' ');
		
		return (size_t)( ret == 0 ? 1 : ret + 1 );
	};
	
	// Recalc token count
	byteCount = recalcTokenCount();
	
	// default values processing
	auto decodeValues = [&]() {
		if ( byteCount == 0 )
			return;
		
		ret.more << ": value(s) = ";
		
		if ( byteCount%4 != 0 ) {
			decodeHexStringAsIntegers(restToken, ret.more);
			return;
		}
		
		wxStringTokenizer tokenizer(restToken, " ");
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
								ret.more << ", ";
								
							ret.more << decodeHexValueAsInteger(value);
							value.clear();
							break;
				}
			}
		}
	};
	
	// command specific handling
	switch ( cmd ) {
		case CMD_GETTER:
				ret.more << wxString::Format("PID = [%03d] %s", decodeHexValueAsInteger(restToken), decodeHexValueAsArduinoPID(restToken));
				
				byteCount = recalcTokenCount();
				decodeValues();
				break;
				
		case CMD_SETTER:
				ret.more << wxString::Format("PID = [%03d] %s", decodeHexValueAsInteger(restToken), decodeHexValueAsArduinoPID(restToken));
				ret.more << "; size = ";
				
				byteCount = recalcTokenCount();
				ret.more << decodeHexValueAsInteger(restToken);
				
				byteCount = recalcTokenCount();
				decodeValues();
				break;
				
		case CMD_MOVE_SEQUENCE:
				ret.more << "size = " << decodeHexValueAsInt32(restToken);
				byteCount = recalcTokenCount();
				decodeValues();
				break;
				
		case CMD_RENDER_AND_MOVE_SEQUENCE:
				ret.more << "size = " << decodeHexValueAsInt32(restToken);
				byteCount = recalcTokenCount();
				decodeValues();
				break;
		
		default: decodeValues();
	}
}
//////////////////////////////////////////////////////////
void SpyHexDecoder::decodeInbound(SpyHexDecoder::Details& ret) {
//////////////////////////////////////////////////////////
	wxString temp;
	
	ret.clear();
	ret.context .assign(decodeContollerResult(context));
	ret.index   .assign(wxString::Format("[%08d]", index));
	
	if ( pid != RET_NULL )	ret.pid.assign(wxString::Format("[%03d] %s", pid, ArduinoPIDs::getPIDLabelWithDefault((unsigned int)pid, "???")));
	else					ret.pid.assign("n.a.");
	
	// default values processing
	auto decodeValues = [&]() {
		if ( index > IDX_SIZE && hexString.length() == 2 ) {
			int val = decodeHexValueAsInteger(hexString);
			
			if ( val < 32 ) {
				ret.more.assign(wxString::Format("ret = %s", ArduinoPIDs::getPIDLabelWithDefault((unsigned int)val, "-"))); 
				return;
			}
		}
			
		ret.more.assign(wxString::Format("Value(s) = %s", decodeHexStringAsInt32s(hexString, temp))); 
	};
	
	// context dependent handling
	switch ( context ) {
		case RET_NULL:		ret.more << decodeHexValueAsArduinoPID(hexString);
							break;
							
		case RET_OK:
		case RET_LIMIT:
		case RET_ERROR:
		case RET_INTERRUPT:
		case RET_HALT:
		case RET_QUIT:		ret.more << decodeHexStringAsIntegers(hexString, temp);
							break;
							
		case RET_SOH:		if ( pid == RET_NULL ) {
								ret.more.assign(wxString::Format("PID = %s", decodeHexValueAsArduinoPID(hexString)));
								break;
							}
							
							// PID dependent handling
							switch ( pid ) {
								case PID_TEXT:				ret.more.assign(decodeHexValueAsCharacterString(hexString, temp));
															break;
															
								case PID_MSG:				if ( index == IDX_MSG_TYPE )	ret.more.assign(wxString::Format("Msg Type = %c", decodeHexValueAsCharacter(hexString)));
															else							ret.more.assign(decodeHexValueAsCharacterString(hexString, temp));
															break;
															
								case PID_HEARTBEAT:			if ( index == IDX_SIZE ) 		ret.more.assign(wxString::Format("Size = %s", decodeHexStringAsIntegers(hexString, temp)));
															else					 		decodeValues();
															break;
															
								// more PIDs on demand
								default:					decodeValues();
														
							}
							
							break;
							
		default:			ret.more << "Unknown context: ";
							ret.more << context;
							ret.more << ", Value(s): ";
							ret.more << decodeHexStringAsIntegers(hexString, temp);
							
							break;
	}
}