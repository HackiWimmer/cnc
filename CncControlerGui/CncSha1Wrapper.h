#ifndef CNC_SHA1_WARAPPER_H
#define CNC_SHA1_WARAPPER_H

#include <iostream>
#include <wx/string.h>
#include <wx/file.h>
#include "sha1.h"

struct CncStringSha1  {
	
	static const wxString& checksum(const wxString& string, wxString& ret) {
		
		SHA1 cs;
		cs.update(std::string(string));
		ret.assign(cs.final().c_str());
		
		return ret;
	}
};

struct CncFileSha1  {
	
	static const wxString& checksum(const wxString& fileName, wxString& ret) {
		
		if ( wxFile::Exists(fileName) == false ) {
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