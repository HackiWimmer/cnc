#ifndef CNC_PERSPECTIVE
#define CNC_PERSPECTIVE

#include <wx/config.h>
#include <wx/menu.h>

class CncPerspective {
	
	private:
		wxConfigBase* config;
		wxMenu*	menu;
		
		int getNextUserPerspectiveInsertIndex(unsigned int sepIndex);
		bool getAllUserPerspectiveNamesFromMenuLabels(wxArrayString& items);
		bool checkIfPerspectiveAlreadyExists(const wxString& name);
		bool insertNextUserPerspective(const wxString& newLabel);
		bool renameUserPerspective(const wxString& from, const wxString& to);
		
	public:
		CncPerspective(wxConfigBase* cfg, wxMenu* menu);
		~CncPerspective();
		
		static bool isUserPerspective(const wxString& menuLabel);
		static const char getNameEnclose();
		static const char* getConfigGroupName();
		static const char* getUserPerspectivePrefix();
		static const char* getPaneListSuffix();
		
		void setupUserPerspectives();
		void destroyUserPerspectives();
		
		bool addUserPerspective();
		bool removeUserPerspective();
		bool renameUserPerspective();
		
		bool loadPerspective(const wxString& name);
		void savePerspective(const wxString& name, bool withQuestion=true);
		
		void ensureRunPerspectiveMinimal();
		void ensureDebugPerspectiveMinimal();
		void ensureAllPanesFromPerspectiveAreShown(const wxString& name);
	
};

#endif