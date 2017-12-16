
#include <iostream>
#include <chrono>
#include <sys/time.h>

#include <wx/init.h>

#include <wx/string.h>





///////////////////////////////////////////////////////////////////
int main( int argc, char** argv ) {
///////////////////////////////////////////////////////////////////
	// initialize wxWidgets
	wxInitializer init;

	unsigned char p1[8];
	unsigned char p2[8];

	int32_t leLong = 123456;
	memcpy(p1, &leLong, 4);
	
	p1[4] = '\0';
	
	
	int32_t v  = htonl(leLong);
	memcpy(p2, &v, 4);

	p2[4] = '\0';
	
	
	for ( int i=0;i<4;i++) {
		std::clog << wxString::Format("%2X", (int)p1[i]) << " ";
	} 
	std::clog << std::endl;
	
	for ( int i=0;i<4;i++) {
		std::clog << wxString::Format("%2X", (int)p2[i]) << " ";
	} 
	std::clog << std::endl;
	
	

	wxString hexToken("FAF4FFFF");
	//wxString hexToken("FFFFF4FA");
	wxString unordered(hexToken);
	unordered.append(wxString('0', 8 - hexToken.length()));
	
	char hex[9];
	
	hexToken.clear();
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
	
	std::clog << hexToken << std::endl;
	
	//hexToken.c_str();
	
	int32_t ret;
	sscanf(hexToken.c_str(), "%8X", &ret);
	std::clog << ret << std::endl;

	return 0;
}