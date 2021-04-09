#ifndef VERSION_H
#define VERSION_H

	#define STR_HELPER(x) #x
	#define STR(x) STR_HELPER(x)

	#define CNC_VERSION_MAJOR_NUM           0
	#define CNC_VERSION_MINOR_NUM           8
	#define CNC_VERSION_INDEX_NUM           9
	
	#define CNC_VERSION_MAJOR_STR           "" STR(CNC_VERSION_MAJOR_NUM)
	#define CNC_VERSION_MINOR_STR           "" STR(CNC_VERSION_MINOR_NUM)
	#define CNC_VERSION_INDEX_STR           "" STR(CNC_VERSION_INDEX_NUM)

	#define CNC_COMPANY_STR                 "Hacki Wimmer"
	#define CNC_DESCRIPTION_STR             "Woodworking CNC Controller"
	#define CNC_COPYRIGHT_STR               "invented by Hacki Wimmer 2016 - 2021"
	#define CNC_PROGNAME_STR                "CncControllerGui.exe"
	#define CNC_PRODUCTNAME_STR             CNC_DESCRIPTION_STR
	
	#define CNC_VERSION_STR                 "" STR(CNC_VERSION_MAJOR_NUM) "." STR(CNC_VERSION_MINOR_NUM) "." STR(CNC_VERSION_INDEX_NUM) "\0"
	

	#define RC_COMPANY_STR                  CNC_COMPANY_STR "\0"
	#define RC_DESCRIPTION_STR              CNC_DESCRIPTION_STR "\0"
	#define RC_COPYRIGHT_STR                CNC_COPYRIGHT_STR "\0"
	#define RC_PROGNAME_STR                 CNC_PROGNAME_STR "\0"
	#define RC_PRODUCTNAME_STR              CNC_PRODUCTNAME_STR "\0"

	#define RC_FILEVERSION                  CNC_VERSION_MAJOR_NUM,CNC_VERSION_MINOR_NUM,CNC_VERSION_INDEX_NUM,0
	#define RC_FILEVERSION_STR              CNC_VERSION_STR 
	
	#define RC_PRODUCTVERSION               CNC_VERSION_MAJOR_NUM,CNC_VERSION_MINOR_NUM,CNC_VERSION_INDEX_NUM,0
	#define RC_PRODUCTVERSION_STR           CNC_VERSION_STR

#endif // VERSION_H