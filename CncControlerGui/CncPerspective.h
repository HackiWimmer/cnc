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
		
		static const char* configGroupName;
		static const char* paneListSuffix;
		static const char* userPerspectivePrefix;
		static const char  nameEnclose;
		
		CncPerspective(wxConfigBase* cfg, wxMenu* menu);
		~CncPerspective();
		
		static bool isUserPerspective(const wxString& menuLabel);
		static const char getNameEnclose();
		static const char* getConfigGroupName();
		static const char* getUserPerspectivePrefix();
		static const char* getPaneListSuffix();
		
		static const char* formatWxPerspectiveInfo(wxString& info);
		
		void setupUserPerspectives();
		void destroyUserPerspectives();
		
		bool addUserPerspective();
		bool removeUserPerspective();
		bool renameUserPerspective();
		
		bool loadDefaultPerspective() { return loadPerspective("Default"); }

		bool loadPerspective(const wxString& name);
		void savePerspective(const wxString& name, bool withQuestion=true);
		
		void ensureRunPerspectiveMinimal();
		void ensureDebugPerspectiveMinimal();
		void ensureAllPanesFromPerspectiveAreShown(const wxString& name);
		
		void logCurrentPerspective();
		bool isAPerspectiveLogged() 	{ return currentPerspectiveBuffer.layoutInfo.IsEmpty() == false; }
		bool restoreLoggedPerspective();
	
	private:
		
		typedef std::map<wxString, bool> ActiveAuiPaneList;
		struct PerspectiveBuffer {
			wxString 			layoutInfo;
			ActiveAuiPaneList	paneList;
			
			///////////////////////////////////////////////////////////
			PerspectiveBuffer ()
			: layoutInfo()
			, paneList()
			{
			}
			
			///////////////////////////////////////////////////////////
			void clear() {
				layoutInfo.clear();
				paneList.clear();
			}
			
			///////////////////////////////////////////////////////////
			const char* trace(wxString& ret) {
				wxString fli(layoutInfo);
				formatWxPerspectiveInfo(fli);
				ret.assign(fli);
				ret.append("\n\n");
				
				for( auto it = paneList.begin(); it != paneList.end(); ++it )
					ret.append(wxString::Format("%s = %d\n", it->first, (int)it->second));
					
				return ret;
			}
		};
		
		PerspectiveBuffer currentPerspectiveBuffer;
};

#endif