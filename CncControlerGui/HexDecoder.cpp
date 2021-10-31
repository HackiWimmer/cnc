#include "HexDecoder.h"

//////////////////////////////////////////////////////////
SpyHexDecoder::SpyHexDecoder(const wxString& contextInfo, const wxString& hs) 
: HexDecoder	()
, hexString		(hs)
, context		(0)
, cmd			(0)
, pid			(0)
, index			(0)
, portion		(0)
//////////////////////////////////////////////////////////
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
SpyHexDecoder::~SpyHexDecoder() {
//////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////
unsigned int SpyHexDecoder::getByteCount(const wxString& hexValues) {
//////////////////////////////////////////////////////////
	const int freq = hexValues.Freq(' ');
	const int len  = hexValues.length();
	
	if ( len == 0 )	
		return 0;
	
	return ( freq == 0 ? 1 : freq + 1 );
}
//////////////////////////////////////////////////////////
bool SpyHexDecoder::readNextHexBytes(wxString& hexValues, unsigned int count, wxString& ret) {
//////////////////////////////////////////////////////////
	const int totalLen  = hexValues.length();
	const int neededLen = count * 2 + (count - 1) * 1;
	
	if ( totalLen == 0 )
		return true;
	
	if ( totalLen < neededLen ) {
		std::cerr << "SpyHexDecoder::readNextHexBytes(): totalLen < neededLen" << std::endl;
		std::cerr << " - totalLen  : " << totalLen  << std::endl;
		std::cerr << " - neededLen : " << neededLen << std::endl;
		return false;
	}

	if ( getByteCount(hexValues) < count ) {
		std::cerr << "SpyHexDecoder::readNextHexBytes(): getByteCount(hexValues) < count" << std::endl;
		std::cerr << " - getByteCount(hexValues) : " << getByteCount(hexValues) << std::endl;
		std::cerr << " - count                   : " << count 					<< std::endl;
		return false;
	}
	
	if ( true ) {
		// reverse byte order
		wxString tmp(hexValues.SubString(0, neededLen));
		wxStringTokenizer t(tmp, " ");
		ret.clear();
		while ( t.HasMoreTokens() ) {
			ret.Prepend(t.GetNextToken());
		}
	}
	else {
		ret.assign(hexValues.SubString(0, neededLen));
		ret.Replace(" ", "", true);
	}
	
	hexValues.assign(hexValues.SubString(neededLen + 1, hexValues.length() - 1));
	return true;
}
//////////////////////////////////////////////////////////
bool SpyHexDecoder::skipNextHexBytes(wxString& hexValues, unsigned int count) {
//////////////////////////////////////////////////////////
	wxString dummy;
	return readNextHexBytes(hexValues, count, dummy);
}
//////////////////////////////////////////////////////////
void SpyHexDecoder::decodeValuesDefault(SpyHexDecoder::Details& ret, wxString& restToken) {
/////////////////////////////////////////////////////////
	unsigned int byteCount = getByteCount(restToken);
	
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
		const wxString token(tokenizer.GetNextToken());
		
		if ( token.IsEmpty() == false ) {
			count++;
			
			switch ( count%4 ) {
				case 1:	
				case 2:
				case 3: value.Prepend(token); 
						break;
						
				case 0: value.Prepend(token);
						if ( count > 4 )
							ret.more << ", ";
							
						ret.more << decodeHexValueAsInteger(value);
						value.clear();
						break;
			}
		}
	}
}
//////////////////////////////////////////////////////////
void SpyHexDecoder::decodeMoveSeqOutbound(SpyHexDecoder::Details& ret, wxString& restToken) {
//////////////////////////////////////////////////////////
	wxString value;
	
	if ( index <= 1 ) {
		// decoding header 
		if ( readNextHexBytes(restToken, 4, value) == false ) return;
		ret.more.append(wxString::Format("Total Size   = %ld | ", (long)decodeHexValueAsInt32(value)));
		
		if ( readNextHexBytes(restToken, 4, value) == false ) return;
		ret.more.append(wxString::Format("Impulse #    = %ld | ", (long)decodeHexValueAsInt32(value)));
		
		if ( readNextHexBytes(restToken, 4, value) == false ) return;
		const int32_t x = decodeHexValueAsInt32(value);
		
		if ( readNextHexBytes(restToken, 4, value) == false ) return;
		const int32_t y = decodeHexValueAsInt32(value);
		
		if ( readNextHexBytes(restToken, 4, value) == false ) return;
		const int32_t z = decodeHexValueAsInt32(value);
		
		ret.more.append(wxString::Format("Lenght x,y,z = %ld, %ld, %ld | ", x, y, z));
	}
	
	if ( readNextHexBytes(restToken, 1, value) == false ) return;
	const int portionSize = decodeHexValueAsInteger(value);
	ret.more.append(wxString::Format("Portion size = %d | | ", portionSize));
	ret.more.append(wxString::Format(" [TYP][%s]            X           Y           Z             | ", ArdoObj::ValueInfo::getBitDeclaration()));
	ret.more.append(                 " ---------------------------------------------------------- | ");
	
	// ---------------------------------------------------
	auto determineDataStructure = [](unsigned char type, unsigned int& byteCount, unsigned int& valCount) {
		ArdoObj::ValueInfo vi(type);
		
		if ( vi.isValid() == false )
			return false;
		
		byteCount = (unsigned int)(vi.getByteCount()); 
		valCount  = (unsigned int)(vi.getValueCount());
		
		return true ;
	};

	// ---------------------------------------------------
	auto decode_CMD_MOVE_SEQUENCE = [&]() {
		for ( int i = 0 ; i < portionSize; i++ ) {
			
			if ( readNextHexBytes(restToken, 1, value) == false ) break;
			const int v = decodeHexValueAsInteger(value);
			
			const int x = ( v &   1 ?  +1 : v &   2 ? -1 : 0 );
			const int y = ( v &   4 ?  +1 : v &   8 ? -1 : 0 );
			const int z = ( v &  16 ?  +1 : v &  32 ? -1 : 0 );
			
			ret.more.append(wxString::Format(" [---][11110001] = % 10d, % 10d, % 10d | ", x, y, z));
		}
	};
	
	// ---------------------------------------------------
	auto decode_CMD_RENDER_AND_MOVE_SEQUENCE = [&]() {
		int remaining 			= portionSize;
		unsigned int byteCount 	= 0;
		unsigned int valCount	= 0;
		
		while ( remaining > 0 ) {
			// pid 
			if ( readNextHexBytes(restToken, 1, value) == false ) return;
			const int valueType = decodeHexValueAsInteger(value);
			remaining--;
			
			// values
			if ( determineDataStructure(valueType, byteCount, valCount) ) {
				
				if ( false ) 
					std::cout << "determineDataStructure: " << valueType << ", " << byteCount << ", " << valCount << std::endl;
				
				int32_t dx, dy, dz, f = 0;
				
				if ( byteCount > 0 ) {
					int32_t values[ArdoObj::ValueInfo::MaxValueCount];
					wxASSERT( valCount <= ArdoObj::ValueInfo::MaxValueCount );
					
					// reading
					for ( unsigned int i = 0 ; i < valCount; i++ ) {
						if ( readNextHexBytes(restToken, byteCount, value) == false ) 
							return;
						
						switch ( byteCount ) {
							case 1:		values[i] = decodeHexValueAsUInt8(value); break;
							case 2:		values[i] = decodeHexValueAsInt16(value); break;
							case 4:		values[i] = decodeHexValueAsInt32(value); break;
						}
						
						remaining -= byteCount;
					}
					
					ArdoObj::ValueInfo vi(valueType);
					
					if 		( vi.isXYZ() ) { dx = values[0]; dy = values[1]; dz = values[2]; }
					else if ( vi.isXY()  ) { dx = values[0]; dy = values[1]; dz = 0;         }
					else if ( vi.isXZ()  ) { dx = values[0]; dy = 0;         dz = values[1]; }
					else if ( vi.isYZ()  ) { dx = 0;         dy = values[0]; dz = values[1]; }
					else if ( vi.isX()   ) { dx = values[0]; dy = values[1]; dz = 0;         }
					else if ( vi.isY()   ) { dx = 0;         dy = values[0]; dz = 0;         }
					else if ( vi.isZ()   ) { dx = 0;         dy = 0;         dz = values[0]; }
					else				   { return;                                         }
				}
				else {
					// one byte xyz format
					if ( readNextHexBytes(restToken, 1, value) == false ) return;
					const int v = decodeHexValueAsInteger(value);
					
					dx = ( v &  1 ?  +1 : v &   2 ? -1 : 0 );
					dy = ( v &  4 ?  +1 : v &   8 ? -1 : 0 );
					dz = ( v & 16 ?  +1 : v &  32 ? -1 : 0 );
					
					remaining--;
				}
				
				// output
				ArdoObj::ValueInfo vi(valueType);
				std::stringstream ss; ss << vi;
				ret.more.append(wxString::Format(" [%03d][%s] = % 10ld, % 10ld, % 10ld, % 10ld  | ", (int)valueType, ss.str().c_str(), (long)dx, (long)dy, (long)dz, (long)f));
				
				//std::cout << portionSize << ", " << remaining << ", " << restToken << std::endl;
			}
			else {
				std::cerr << "SpyHexDecoder::decodeMoveSeqOutbound(): Invalid Type: " << valueType << std::endl;
				break;
			}
		}
	};
	
	switch ( cmd ) {
		case CMD_MOVE_SEQUENCE: 			decode_CMD_MOVE_SEQUENCE();				break;
		case CMD_RENDER_AND_MOVE_SEQUENCE:	decode_CMD_RENDER_AND_MOVE_SEQUENCE(); 	break;
		
		default:							std::cerr << "SpyHexDecoder::decodeMoveSeqOutbound(): Invalid command: " 
		                                              << (int)cmd << std::endl;
	}
}
//////////////////////////////////////////////////////////
void SpyHexDecoder::decode(SpyHexDecoder::Details& ret) {
//////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////
void SpyHexDecoder::decodeOutbound(SpyHexDecoder::Details& ret) {
//////////////////////////////////////////////////////////
	wxString temp;
	
	unsigned int byteCount = getByteCount(hexString);
	wxString restToken(hexString);
	
	ret.clear();
	
	if ( byteCount <= 0 )
		return;
	
	ret.context .assign(decodeContollerResult(context));
	ret.cmd     .assign(wxString::Format("%s [%c]", ArduinoCMDs::getCMDLabel((unsigned int)cmd), cmd));
	ret.portion .assign(wxString::Format("[%08d]", portion));
	
	if ( ArduinoCMDs::exists((unsigned int)cmd) == false )
		return;
	
	// skip command byte
	if ( index <= 1 )
		if ( skipNextHexBytes(restToken, 1) == false ) 
			return;
	
	// command specific handling
	wxString value;
	switch ( cmd ) {
		case CMD_GETTER:
		{
			if ( readNextHexBytes(restToken, 1, value) == false ) 
				break;
				
			ret.more << wxString::Format("PID = [%03d] %s", decodeHexValueAsInteger(value), decodeHexValueAsArduinoPID(value));
			decodeValuesDefault(ret, restToken);
			break;
		}
		case CMD_SETTER:
		{
			if ( readNextHexBytes(restToken, 1, value) == false ) 
				break;
				
			ret.more << wxString::Format("PID = [%03d] %s", decodeHexValueAsInteger(value), decodeHexValueAsArduinoPID(value));

			if ( readNextHexBytes(restToken, 1, value) == false ) 
				break;
				
			ret.more << "; Size = ";
			ret.more << decodeHexValueAsInteger(value);
			
			decodeValuesDefault(ret, restToken);
			break;
		}
		case CMD_MOVE:
		case CMD_RENDER_AND_MOVE:
		case CMD_MOVE_UNTIL_LIMIT_IS_FREE:
		case CMD_MOVE_PODEST:
		case CMD_MOVE_PODEST_EXACT:
		{
			if ( readNextHexBytes(restToken, 1, value) == false ) 
				break;
			
			const int size = decodeHexValueAsInteger(value);
			ret.more << "Size = ";
			ret.more << size;
			
			if ( size > 0 && size < 4 ) {
				const char* label = (size == 1 ? "Z" : "XYZ");
				
				for ( int i = 0; i < size; i++ ) {
					if ( readNextHexBytes(restToken, 4, value) == false ) return;
						ret.more.append(wxString::Format("; %c=%ld", label[i], (long)decodeHexValueAsInt32(value)));
				}
			}
			
			break;
		}
		case CMD_MOVE_SEQUENCE:
		case CMD_RENDER_AND_MOVE_SEQUENCE:
		{
			decodeMoveSeqOutbound(ret, restToken);
			break;
		}
		case CMD_RESET_CONTROLLER:
		case CMD_PRINT_CONFIG:
		case CMD_PRINT_TIMESTAMP:
		case CMD_PRINT_VERSION:
		case CMD_PRINT_PIN_REPORT:
		case CMD_POP_SERIAL:
		case CMD_IDLE:
		case CMD_HEARTBEAT:
		case CMD_PERFORM_TEST:
		case CMD_MOVE_INTERACTIVE:
		case CMD_ACTIVATE_PODEST_HW:
		case CMD_DEACTIVATE_PODEST_HW:
		{
			ret.more << wxString::Format("<Command without further content>");
			break;
		}
		case SIG_UPDATE:
		{
			if ( readNextHexBytes(restToken, 1, value) == false ) 
				break;
				
			const int len = decodeHexValueAsInteger(value);
			ret.more << "LEN = " << len;
			
			if ( readNextHexBytes(restToken, 1, value) == false ) 
				break;
				
			const int subPid = decodeHexValueAsInteger(value);
			ret.more << wxString::Format("; PID = [%03d] %s", subPid, decodeHexValueAsArduinoPID(value));
			
			// only if further data is announced
			if ( len > 0 )
			{
				switch ( subPid )
				{
					case PID_XYZ_INTERACTIVE_POS:
					{
						if ( readNextHexBytes(restToken, 1, value) == false ) 
							break;
						ret.more << "; X = " << decodeHexValueAsInt8(value); // As Int8 to resolve also negative values
							
						if ( readNextHexBytes(restToken, 1, value) == false ) 
							break;
						ret.more << "; Y = " << decodeHexValueAsInt8(value); // As Int8 to resolve also negative values
						
						if ( readNextHexBytes(restToken, 1, value) == false ) 
							break;
						ret.more << "; Z = " << decodeHexValueAsInt8(value); // As Int8 to resolve also negative values
						
						
						if ( readNextHexBytes(restToken, 1, value) == false ) 
							break;
						ret.more << "; Fx = " << decodeHexValueAsUInt8(value); 
						
						if ( readNextHexBytes(restToken, 1, value) == false ) 
							break;
						ret.more << decodeHexValueAsInt8(value); 
					}
				}
			}
			
			break;
		}
		case SIG_INTERRUPPT:
		case SIG_HALT:
		case SIG_PAUSE:
		case SIG_RESUME:
		case SIG_QUIT_MOVE:
		case SIG_SOFTWARE_RESET:
		{
			ret.more << wxString::Format("<Signal without further content>");
			break;
		}
		default: 
		{
			ret.more << wxString::Format("!Command isn't registered to decode!");
		}
	}
}
//////////////////////////////////////////////////////////
void SpyHexDecoder::decodeInbound(SpyHexDecoder::Details& ret) {
//////////////////////////////////////////////////////////
	wxString temp;
	
	ret.clear();
	ret.context .assign(decodeContollerResult(context));
	ret.cmd     .assign(wxString::Format("%s [%c]", ArduinoCMDs::getCMDLabel((unsigned int)cmd), cmd));
	ret.index   .assign(wxString::Format("[%08d]", index));
	
	const int BYTE_STRING_LEN = 2;
	// ----------------------------------------------------------------------
	wxString tmpRet;
	auto lastInboundBytes = [&](int byteCount) {
		
		if ( (int)ret.inbound.prev.length() >= byteCount * BYTE_STRING_LEN ) {
			tmpRet.assign(ret.inbound.prev.Right(byteCount * BYTE_STRING_LEN));
			
			switch ( byteCount ) {
				case 2: return reorderHexInt16String(tmpRet);
				case 4: return reorderHexInt32String(tmpRet);
			}
		}
		else {
			tmpRet.assign(wxString('0', byteCount * BYTE_STRING_LEN));
		}
		
		return tmpRet;
	};
	
	// ----------------------------------------------------------------------
	auto inboundByteAtIndex = [&](int idx) {
		if ( idx > 0 )	tmpRet.assign(ret.inbound.prev.Mid((idx - 1) * BYTE_STRING_LEN, BYTE_STRING_LEN));
		else			tmpRet.assign("00");
		return tmpRet;
	};
	
	// ----------------------------------------------------------------------
	// context dependent handling
	switch ( context ) {
		
		// Context ----------------------------------------------------------------------------
		case RET_SOH:
		{
			if  	( index == 0x01 ) { ret.more.assign(wxString::Format("%s",     decodeHexValueAsArduinoPID(hexString))); break; } // break RET_SOH:
			else if ( index == 0x02 ) { ret.more.assign(wxString::Format("%s ...", decodeHexValueAsArduinoPID(hexString))); break; } // break RET_SOH:
			
			// PID dependent handling
			switch ( pid ) {
				
				// ...........................................................
				case PID_TEXT:
				{	
					ret.more.assign(decodeHexValueAsCharacterString(hexString, temp));
					break;
				}
				// ...........................................................
				case PID_MSG:
				{	
					if      ( index == 0x03 ) {
						// message type E, W, I ...
						ret.more.assign(wxString::Format("Msg Type = %c", decodeHexValueAsCharacter(hexString))); 
					}
					else if ( index == 0x04 ) {
						
						// message ID ?
						const bool b = decodeHexValueAsUInt8(hexString) == MT_MID_FLAG;
						if ( b )	ret.more.assign("MT_MID_FLAG");
						else		ret.more.assign(decodeHexValueAsCharacterString(hexString, temp)); 
					}
					else if ( index == 0x05 ) {
						
						// standard message
						const bool b = decodeHexValueAsUInt8( inboundByteAtIndex(0x04) ) == MT_MID_FLAG;
						if ( b )	ret.more.assign(wxString::Format("%s", ArduinoErrorCodes::getECLabel(decodeHexValueAsUInt8(hexString))));
						else 		ret.more.assign(decodeHexValueAsCharacterString(hexString, temp)); 
					}
					else if ( index  > 0x05 ) {
						ret.more.assign(decodeHexValueAsCharacterString(hexString, temp)); 
					}
					
					break;
				}
				// ...........................................................
				case PID_GETTER:
				{
					if      ( index == 0x03 ) { ret.more.assign(wxString::Format("Content = %s", decodeHexValueAsArduinoPID( hexString ))); }
					else if ( index == 0x04 ) { ret.more.assign(wxString::Format("Size    = %s", decodeHexStringAsIntegers ( hexString, temp ))); }
					else if ( index  > 0x05 ) {
						
						// every 4 bytes
						if ( ( index - 0x04 ) % 4 == 0 )
							ret.more.assign(wxString::Format("val     = %ld", (long)decodeHexValueAsInt32( lastInboundBytes(4) )));
						
						// last byte
						if ( index == (unsigned int)(0x05 + decodeHexValueAsUInt8( inboundByteAtIndex(0x04) ) * 4) )
							ret.more.assign(wxString::Format("%s", decodeHexValueAsArduinoPID ( hexString )));
					}
					
					break;
				}
				// ...........................................................
				case PID_HEARTBEAT:
				{
					if      ( index == 0x06 ) { ret.more.assign(wxString::Format("Id     = %ld", (long)decodeHexValueAsInt32( lastInboundBytes(4) ))); }
					else if ( index == 0x07 ) { ret.more.assign(wxString::Format("Limit  = %d",   (int)decodeHexValueAsUInt8( lastInboundBytes(1) ))); }
					else if ( index == 0x08 ) { ret.more.assign(wxString::Format("Switch = %d",   (int)decodeHexValueAsUInt8( lastInboundBytes(1) ))); }
					else if ( index == 0x09 ) { ret.more.assign(wxString::Format("Power  = %d",   (int)decodeHexValueAsUInt8( lastInboundBytes(1) ))); }
					else if ( index == 0x0A ) { ret.more.assign(wxString::Format("Const  = %d",   (int)decodeHexValueAsUInt8( lastInboundBytes(1) ))); }
					else if ( index == 0x0B ) { ret.more.assign(wxString::Format("%s", decodeHexValueAsArduinoPID ( hexString ))); }
					
					break;
				}
				// ...........................................................
				case PID_XYZ_POS_MAJOR:
				case PID_XYZ_POS_DETAIL:
				case PID_LIMIT:
				{
					if      ( index == 0x06 ) { ret.more.assign(wxString::Format("X = %ld", (long)decodeHexValueAsInt32( lastInboundBytes(4) ))); }
					else if ( index == 0x0A ) { ret.more.assign(wxString::Format("Y = %ld", (long)decodeHexValueAsInt32( lastInboundBytes(4) ))); }
					else if ( index == 0x0E ) { ret.more.assign(wxString::Format("Z = %ld", (long)decodeHexValueAsInt32( lastInboundBytes(4) ))); }
					else if ( index == 0x12 ) { ret.more.assign(wxString::Format("F = %ld", (long)decodeHexValueAsInt32( lastInboundBytes(4) ) / FLT_FACT )); }
					else if ( index == 0x13 ) { ret.more.assign(wxString::Format("%s", decodeHexValueAsArduinoPID ( hexString ))); }
					
					break;
				}
				
				// more PIDs on demand
				default:	;
			}
			
			break;
		} //RET_SOH:
		
		// Context ----------------------------------------------------------------------------
		case RET_NULL:		ret.more << decodeHexValueAsArduinoPID(hexString);
							break;
		
		// Context ----------------------------------------------------------------------------
		case RET_OK:
		case RET_MORE:
		case RET_LIMIT:
		case RET_ERROR:
		case RET_INTERRUPT:
		case RET_HALT:
		case RET_QUIT:		ret.more << decodeHexStringAsIntegers(hexString, temp);
							break;
							
		// Context ----------------------------------------------------------------------------
		default:			ret.more << "Unknown context: ";
							ret.more << context;
							ret.more << ", Value(s): ";
							ret.more << decodeHexStringAsIntegers(hexString, temp);
							
							break;
	}
}