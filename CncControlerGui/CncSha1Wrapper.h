#ifndef CNC_SHA1_WARAPPER_H
#define CNC_SHA1_WARAPPER_H

#include <iostream>
#include <wx/string.h>
#include <wx/file.h>
#include "sha1.h"

typedef wxString SHA1SessionKey;

struct CncStringSha1  {
	
	static const wxString& checksum(const wxString& underlying, wxString& ret) 
	{
		SHA1 cs;
		cs.update(std::string(underlying));
		ret.assign(cs.final().c_str());
		
		return ret;
	}
	
	static const SHA1SessionKey& createSessionKey(const wxString& underlying, SHA1SessionKey& ret) 
	{
		return checksum(underlying, ret) ;
	}
	
};

struct CncFileSha1  {
	
	static const wxString& checksum(const wxString& fileName, wxString& ret)
	{
		if ( wxFile::Exists(fileName) == false )
		{
			std::cerr << "CncFileSha1::checksum: Can't open file: '"
					  << fileName 
					  << "'" << std::endl;
					  
			ret.clear();
			return ret; 
		}
		
		ret.assign(SHA1::from_file(std::string(fileName)));
		return ret;
	}
};


#endif