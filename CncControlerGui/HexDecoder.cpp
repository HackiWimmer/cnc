#include "HexDecoder.h"

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
	
	ret.assign(hexValues.SubString(0, neededLen));
	ret.Replace(" ", "", true);
	
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
	
	// ---------------------------------------------------
	auto determineDataStructure = [](unsigned char pid, unsigned int& byteCount, unsigned int& valCount) {
		bool ret = true;
		switch ( pid ) {
			case PID_MV_SEQ_0_XYZ:  byteCount = 0; valCount = 3; break;
			case PID_MV_SEQ_0_XY:   byteCount = 0; valCount = 2; break;
			case PID_MV_SEQ_0_X:    byteCount = 0; valCount = 1; break;
			case PID_MV_SEQ_0_Y:    byteCount = 0; valCount = 1; break;
			case PID_MV_SEQ_0_Z:    byteCount = 0; valCount = 1; break;

			case PID_MV_SEQ_1_XYZ:  byteCount = 1; valCount = 3; break;
			case PID_MV_SEQ_1_XY:   byteCount = 1; valCount = 2; break;
			case PID_MV_SEQ_1_X:    byteCount = 1; valCount = 1; break;
			case PID_MV_SEQ_1_Y:    byteCount = 1; valCount = 1; break;
			case PID_MV_SEQ_1_Z:    byteCount = 1; valCount = 1; break;

			case PID_MV_SEQ_2_XYZ:  byteCount = 2; valCount = 3; break;
			case PID_MV_SEQ_2_XY:   byteCount = 2; valCount = 2; break;
			case PID_MV_SEQ_2_X:    byteCount = 2; valCount = 1; break;
			case PID_MV_SEQ_2_Y:    byteCount = 2; valCount = 1; break;
			case PID_MV_SEQ_2_Z:    byteCount = 2; valCount = 1; break;

			case PID_MV_SEQ_4_XYZ:  byteCount = 4; valCount = 3; break;
			case PID_MV_SEQ_4_XY:   byteCount = 4; valCount = 2; break;
			case PID_MV_SEQ_4_X:    byteCount = 4; valCount = 1; break;
			case PID_MV_SEQ_4_Y:    byteCount = 4; valCount = 1; break;
			case PID_MV_SEQ_4_Z:    byteCount = 4; valCount = 1; break;

			default:				ret = false;
		}
		return ret;
	};

	// ---------------------------------------------------
	auto decode_CMD_MOVE_SEQUENCE = [&]() {
		for ( int i = 0 ; i < portionSize; i++ ) {
			
			if ( readNextHexBytes(restToken, 1, value) == false ) break;
			const int v = decodeHexValueAsInteger(value);
			
			const int x = ( v &  1 ?  +1 : v &  2 ? -1 : 0 );
			const int y = ( v &  4 ?  +1 : v &  8 ? -1 : 0 );
			const int z = ( v & 16 ?  +1 : v & 32 ? -1 : 0 );
			
			ret.more.append(wxString::Format("%d, %d, %d | ", x, y, z));
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
			const int pid = decodeHexValueAsInteger(value);
			remaining--;
			
			// values
			if ( determineDataStructure(pid, byteCount, valCount) ) {
				
				if ( false ) 
					std::cout << "determineDataStructure: " << pid << ", " << byteCount << ", " << valCount << std::endl;
				
				int32_t dx, dy, dz = 0;
				
				if ( byteCount > 0 ) {
					int32_t values[3];
					wxASSERT( valCount <= 3 );
					
					// reading
					for ( unsigned int i = 0 ; i < valCount; i++ ) {
						if ( readNextHexBytes(restToken, byteCount, value) == false ) 
							return;
						
						switch ( byteCount ) {
							case 1:		values[i] = decodeHexValueAsInt8(value);  break;
							case 2:		values[i] = decodeHexValueAsInt16(value); break;
							case 4:		values[i] = decodeHexValueAsInt32(value); break;
						}
						
						remaining -= byteCount;
					}
					
					// determine values
					switch ( pid ) {
						case PID_MV_SEQ_0_XYZ:   
						case PID_MV_SEQ_1_XYZ:   
						case PID_MV_SEQ_2_XYZ:   
						case PID_MV_SEQ_4_XYZ:  dx = values[0]; dy = values[1]; dz= values[2];  break;
						
						case PID_MV_SEQ_0_XY:
						case PID_MV_SEQ_1_XY:
						case PID_MV_SEQ_2_XY:
						case PID_MV_SEQ_4_XY:   dx = values[0]; dy = values[1]; dz = 0;         break;
						
						case PID_MV_SEQ_0_X:
						case PID_MV_SEQ_1_X:
						case PID_MV_SEQ_2_X:
						case PID_MV_SEQ_4_X:    dx = values[0]; dy = 0;         dz = 0;         break;
						
						case PID_MV_SEQ_0_Y:
						case PID_MV_SEQ_1_Y:
						case PID_MV_SEQ_2_Y:
						case PID_MV_SEQ_4_Y:    dx = 0;         dy = values[0]; dz = 0;         break;
						
						case PID_MV_SEQ_0_Z:
						case PID_MV_SEQ_1_Z:
						case PID_MV_SEQ_2_Z:
						case PID_MV_SEQ_4_Z:    dx = 0;         dy = 0;         dz = values[0]; break;
						
						default:                return;
					}
				}
				else {
					// one byte xyz format
					if ( readNextHexBytes(restToken, 1, value) == false ) return;
					const int v = decodeHexValueAsInteger(value);
					
					dx = ( v &  1 ?  +1 : v &  2 ? -1 : 0 );
					dy = ( v &  4 ?  +1 : v &  8 ? -1 : 0 );
					dz = ( v & 16 ?  +1 : v & 32 ? -1 : 0 );
					
					remaining--;
				}
				
				// output
				ret.more.append(wxString::Format(" x, y, z = %ld, %ld, %ld  | ", (long)dx, (long)dy, (long)dz));
				
				if ( false )
					std::cout << portionSize << ", " << remaining << ", " << restToken << std::endl;
			}
			else {
				std::cerr << "SpyHexDecoder::decodeMoveSeqOutbound(): Invalid PID: " << pid << std::endl;
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
				if ( readNextHexBytes(restToken, 1, value) == false ) break;
				ret.more << wxString::Format("PID = [%03d] %s", decodeHexValueAsInteger(value), decodeHexValueAsArduinoPID(value));
				
				decodeValuesDefault(ret, restToken);
				break;
				
		case CMD_SETTER:
				if ( readNextHexBytes(restToken, 1, value) == false ) break;
				ret.more << wxString::Format("PID = [%03d] %s", decodeHexValueAsInteger(value), decodeHexValueAsArduinoPID(value));

				if ( readNextHexBytes(restToken, 1, value) == false ) break;
				ret.more << "; size = ";
				ret.more << decodeHexValueAsInteger(value);
				
				decodeValuesDefault(ret, restToken);
				break;
				
		case CMD_MOVE_SEQUENCE:
		case CMD_RENDER_AND_MOVE_SEQUENCE:
				decodeMoveSeqOutbound(ret, restToken);
				break;
		
		default: decodeValuesDefault(ret, restToken);
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
	
	if ( pid != RET_NULL )	ret.pid.assign(wxString::Format("%s [%03d]", ArduinoPIDs::getPIDLabelWithDefault((unsigned int)pid, "???"), pid));
	else					ret.pid.assign("n.a.");
	
	// default values processing
	auto decodeValues = [&]() {
		if ( index > IDX_DEF_SIZE && hexString.length() == 2 ) {
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
		case RET_MORE:
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
															
								case PID_HEARTBEAT:			if ( index == IDX_HB_SIZE ) 	ret.more.assign(wxString::Format("Size = %s", decodeHexStringAsIntegers(hexString, temp)));
															else					 		decodeValues();
															break;
															
								case PID_GETTER:			switch ( index ) {
																case IDX_GETTER_PID: 		ret.more.assign(wxString::Format("PID = %s", decodeHexValueAsArduinoPID(hexString)));
																							break;
																							
																case IDX_GETTER_SIZE: 		ret.more.assign(wxString::Format("Size = %s", decodeHexStringAsIntegers(hexString, temp)));
																							break;
																							
																default:					decodeValues();
															}
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