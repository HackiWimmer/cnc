#include <iostream>
#include "CncCommon.h"
#include "SVGElementConverter.h"
#include "SVGParserBase.h"

//////////////////////////////////////////////////////////////////
SVGParserBase::SVGParserBase(SVGPathHandlerBase* ph)
: pathHandler(ph)
//////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////
SVGParserBase::~SVGParserBase() {
//////////////////////////////////////////////////////////////////
	wxDELETE(pathHandler);
}
///////////////////////////////////////////////////////////////////////
inline int SVGParserBase::getCommandParaCount(char c) {
///////////////////////////////////////////////////////////////////////
	int ret = -1;
	
	switch ( c ) {
		case 'z':
		case 'Z': 	ret = 0; break;
		case 'h':
		case 'H':
		case 'v':
		case 'V': 	ret = 1; break;
		case 'm':
		case 'M':
		case 'l':
		case 'L':
		case 't': 
		case 'T': 	ret = 2; break;
		case 'q':
		case 'Q':
		case 's':
		case 'S': 	ret = 4; break;
		case 'c':
		case 'C': 	ret = 6; break;
		case 'a':
		case 'A': 	ret = 7; break;
		
		default: 	ret = -1;
	}
	
	return ret;
}
//////////////////////////////////////////////////////////////////
bool SVGParserBase::evaluatePath(const wxString& data) {
//////////////////////////////////////////////////////////////////
	// data contains the content of the d="" attribute, this is a multi command context
	// there's an example:
	// d = "
	//      M 0.68598735,110.12961 21.583664,93.092966 41.867069,42.606192 83.739072,47.632386 94.14101,38.983749 97.47977,6.2701413 
	//      l -1.01348,-3.944304 4.18955,-2.06530296 27.86445,6.36728696 -1.82864,2.999917 -0.57888,4.9551207 37.64267,8.153743 33.85591
	//      z
	// "
	// Main cause of this method is to token down data into single commands
	
	initNextPath(data);
	
	int sPos = -1;
	wxString token;
	for (unsigned int i=0; i<data.Length(); i++ )
	{
		
		if ( data[i] == '-' || data[i] == '+' || data[i] == 'e' || data[i] == 'E' ) 
			continue;
			
		if ( isalpha(data[i]) && sPos < 0 )
		{
			sPos = i;
		}
		else if ( isalpha(data[i]) )
		{
			token.assign(data.SubString(sPos, i - 1));
			token.Trim(true).Trim(false);
			
			if ( processPathCommand(token) == false )
				return false;
				
			sPos = i;
		} 
		
		if ( i == data.Length() - 1 )
		{
			token.assign(data.SubString(sPos, i));
			token.Trim(true).Trim(false);
			
			if ( processPathCommand(token) == false )
				return false;
		}
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////
bool SVGParserBase::processPathCommand(const wxString& para) {
//////////////////////////////////////////////////////////////////
	// para contains one command context only
	// there's an example:
	// M 0.68598735,110.12961 21.583664,93.092966 41.867069,42.606192 83.739072,47.632386 94.14101,38.983749 97.47977,6.2701413
	//
	// Main cause of this method is to token down the values into a list of path elements 
	// to finally call the virtual method addPathElement(...)
	
	if ( para.Length() == 0 )
		return true;
	
	if ( false )
		std::cout << para << std::endl;

	double values[MAX_PARAMETER_VALUES];
	
	wxString token;
	char c						= '\0';
	unsigned int sPos			=  0;
	unsigned int valueCounter	=  0;
	unsigned int commandCounter	=  0;
	int parameterCount			= -1;

	for (unsigned int i=sPos; i<para.Length(); i++)
	{
		
		if ( i == 0 )
		{
			c = para[0].GetValue();
			sPos++;
			
			if ( (parameterCount = getCommandParaCount(c) ) < 0 )
			{
				std::cerr	<< CNC_LOG_FUNCT 
							<< "Not known command: "
							<< c << std::endl
							;
				return false;
			}
		}
		else
		{
			// list of possible separators
			if ( para[i] == ' ' || para[i] == ',' || para[i] == '-' || para[i] == '+' )
			{
				
				// handle exponential presentation - i is always > 0
				if ( (para[i] == '-' || para[i] == '+') && ( para[i-1] == 'e' || para[i-1] == 'E') ) 
					continue;
				
				if ( i != sPos )
				{
					token.assign(para.SubString(sPos, i - 1));
					token.ToDouble(&values[valueCounter++]);

					if ( valueCounter == MAX_PARAMETER_VALUES )
					{
						std::cerr	<< CNC_LOG_FUNCT 
									<< ": Max parameters count reached for: " 
									<< para.c_str() 
									<< std::endl
									;
						return false;
					}
				}
				
				if ( para[i] == '-'|| para[i] == '+' )	sPos = i;
				else									sPos = i + 1;
				
			}
			else if ( i == para.Length() - 1 )
			{
				token.assign(para.SubString(sPos, i));
				token.ToDouble(&values[valueCounter++]);
			}
		}
		
		if ( (int)valueCounter ==  parameterCount )
		{
			commandCounter++;
			if ( commandCounter == 2 && ( c == 'm' || c == 'M' ) ) {
				// M - 1 = L or m - 1 = l
				c = c - 1;
			}
			
			const bool ret = addPathElement(c, valueCounter, values);
			if ( ret == false )
				return false;
			
			valueCounter = 0;
		} 
		
		if ( evaluateProcessingCallback() == false )
			return false;
	}
	
	const bool ret = (valueCounter == 0);
	if ( ret == false )
	{
		std::cerr	<< CNC_LOG_FUNCT_A(":") 								<< std::endl
					<< "Parameters count error in: "	<< para.c_str()		<< std::endl
					<< "Defined parameter count: "		<< parameterCount 
					<< "; Current value count: "		<< valueCounter		<< std::endl
					<< "Stored value list: "								<< std::endl
					;
					
		for (unsigned int i=0; i<valueCounter; i++)
			std::cerr << "[" << i << "]=" << values[i] << "\t";
			
		std::cerr << std::endl;
	}
	
	return ret;
}
