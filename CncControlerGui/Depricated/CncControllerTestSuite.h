#ifndef CNC_TEST_SUITE
#define CNC_TEST_SUITE

#include <map>
#include <wx/listbox.h>
#include "CncArduino.h"

////////////////////////////////////////////////////////////////
struct CncTestCaseConfig {
	unsigned int 	id;
	const char* 	name;
	const char*		description;
	unsigned int 	parameterCount;
	
	int32_t			defaultValue1;
	int32_t			defaultValue2;
	int32_t			defaultValue3;
	int32_t			defaultValue4;
	int32_t			defaultValue5;
	
	///////////////////////////////////////////////////////////
	CncTestCaseConfig() {
		init();
	}
	
	///////////////////////////////////////////////////////////
	void init() {
		id				= TID_INVALID;
		name 			= "";
		description		= "Not available";
		parameterCount 	= 0;
		initDefaultValues();
	}
	
	///////////////////////////////////////////////////////////
	void initDefaultValues(int32_t v1=0, int32_t v2=0, int32_t v3=0, int32_t v4=0, int32_t v5=0) {
		defaultValue1	= v1;
		defaultValue2	= v2;
		defaultValue3	= v3;
		defaultValue4	= v4;
		defaultValue5	= v5;
	}
};

typedef std::map<unsigned int, CncTestCaseConfig> CncTestCaseMap;

////////////////////////////////////////////////////////////////
class CncControllerTestSuite {

	private:
		
		static CncTestCaseMap testCaseList;
		
		////////////////////////////////////////////////////////
		static void registerTestCase(const CncTestCaseConfig& tcc) {
			if ( tcc.id != TID_INVALID)
				testCaseList[tcc.id] = tcc;
		}
		
		////////////////////////////////////////////////////////
		static int32_t getDefaultValue(unsigned int testCaseId, unsigned int parameter) {
			CncTestCaseMap::iterator it;
			it = testCaseList.find(testCaseId);
			if ( it != testCaseList.end() ) {
				switch (parameter) {
					case 1: return it->second.defaultValue1;
					case 2: return it->second.defaultValue2;
					case 3: return it->second.defaultValue3;
					case 4: return it->second.defaultValue4;
					case 5: return it->second.defaultValue5;
				}
			}

			return 0;
		}
		
	public:
		
		////////////////////////////////////////////////////////
		static void init();
		
		////////////////////////////////////////////////////////
		static const CncTestCaseMap& getTestCases() {
			return testCaseList;
		}
		
		////////////////////////////////////////////////////////
		static int getParameterCount(unsigned int testCaseId) {
			CncTestCaseMap::iterator it;
			it = testCaseList.find(testCaseId);
			if ( it != testCaseList.end() ) 
				return it->second.parameterCount;
			
			return -1;
		}
		
		////////////////////////////////////////////////////////
		static const char* getTestCaseName(unsigned int testCaseId) {
			CncTestCaseMap::iterator it;
			it = testCaseList.find(testCaseId);
			if ( it != testCaseList.end() ) 
				return it->second.name;
			
			return "Unknown Test Case";
		}
		
		////////////////////////////////////////////////////////
		static const char* getTestCaseDescrption(unsigned int testCaseId) {
			CncTestCaseMap::iterator it;
			it = testCaseList.find(testCaseId);
			if ( it != testCaseList.end() ) 
				return it->second.description;
			
			return "Unknown Test Case";
		}
		
		////////////////////////////////////////////////////////
		static int32_t getDefaultValue1(unsigned int testCaseId) { return getDefaultValue(testCaseId, 1); }
		static int32_t getDefaultValue2(unsigned int testCaseId) { return getDefaultValue(testCaseId, 2); }
		static int32_t getDefaultValue3(unsigned int testCaseId) { return getDefaultValue(testCaseId, 3); }
		static int32_t getDefaultValue4(unsigned int testCaseId) { return getDefaultValue(testCaseId, 4); }
		static int32_t getDefaultValue5(unsigned int testCaseId) { return getDefaultValue(testCaseId, 5); }
		
		////////////////////////////////////////////////////////
		static void fillTestCases(wxListBox* lb) {
			if ( lb == NULL)
				return;
			
			lb->Clear();
			for ( CncTestCaseMap::iterator it=testCaseList.begin(); it!=testCaseList.end(); ++it) {
				CncTestCaseConfig tcc = (*it).second;
				
				wxString item;
				item += wxString::Format("%03d:   ", tcc.id);
				item += tcc.name;
				
				lb->Append(item);
			}
			if ( lb->GetCount() > 0 )
				lb->Select(0);
		}
};

#endif