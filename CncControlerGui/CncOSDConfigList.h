#ifndef CNC_OSD_CONFIG_LIST_H
#define CNC_OSD_CONFIG_LIST_H

#include <iostream>
#include <vector>
#include <map>
#include "wx/string.h"


class CncOSDConfigList {

	private:

		struct OSDEntry {
			typedef std::pair<wxString, wxString> Key;
			typedef std::vector<wxString> Values;
			
			static const char groupKeySeparator;
			
			char 	delimiter 				= '\0';
			Key 	id;
			Values 	values;

			OSDEntry();
			OSDEntry(const wxString& id, const wxString& value);
			OSDEntry(const wxString& group, const wxString& key, const wxString& value);
			~OSDEntry();

			unsigned int 	getCount() const;

			const wxString&	getId(wxString& ret) const;
			const wxString&	getGroup(wxString& ret) const;
			const wxString&	getKey(wxString& ret) const;

			const wxString&	getValue(unsigned short idx, wxString& ret) const;
			const wxString&	getValues(wxString& ret) const;

			friend std::ostream &operator<< (std::ostream &o, const OSDEntry &a) {
				o << a.id.first << OSDEntry::groupKeySeparator << a.id.second << "=";

				for (auto it = a.values.begin(); it != a.values.end(); ++it) {
					if ( a.delimiter != '\0' )
						o << a.delimiter;

					o << *it;
				}

				o << std::endl;
				return o;
			}

			static const wxString makeId(const wxString& group, const wxString& key);

		};

		typedef std::map<wxString, OSDEntry> OSDConfigMap;
		OSDConfigMap configMap;

	public:
		CncOSDConfigList();
		~CncOSDConfigList();
		
		bool add(const wxString& id, const wxString& value);
		bool add(const wxString& group, const wxString& key, const wxString& value);
		
		bool update(const wxString& id, const wxString& value);
		bool update(const wxString& group, const wxString& key, const wxString& value);
		
		const wxString& get(const wxString& id, wxString& value) const;
		const wxString& get(const wxString& group, const wxString& key, wxString& value) const;
		
		const wxString& getFromIndex(const wxString& id, unsigned short idx, wxString& value) const;
		const wxString& getFromIndex(const wxString& group, const wxString& key, unsigned short idx, wxString& value) const;
		
		const wxString& getList(const wxString& id, wxString& values) const;
		const wxString& getList(const wxString& group, const wxString& key, wxString& values) const;
		
		friend std::ostream &operator<< (std::ostream &o, const CncOSDConfigList &a) {
			for (auto it = a.configMap.begin(); it != a.configMap.end(); ++it)
				o << it->second;

			return o;
		}
		
		static const wxString getOSDPrefix() { return "OSD_"; }
			
		static const wxString makeId(const wxString& group, const wxString& key) {
			return OSDEntry::makeId(group, key);
		}
		
		static void test();
};

#endif
