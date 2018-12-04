#include <wx/tokenzr.h>
#include "CncOSDConfigList.h"

#ifdef __WXMSW__
	unsigned short OSD_INDEX = 0;
#else
	unsigned short OSD_INDEX = 1;
#endif

const char CncOSDConfigList::OSDEntry::groupKeySeparator	= '/';

///////////////////////////////////////////////////////////////////
const wxString& CncOSDConfigList::OSDEntry::makeId(const wxString& group, const wxString& key) {
///////////////////////////////////////////////////////////////////
	static wxString ret;

	ret.assign(wxString::Format("%s%c%s", group, OSDEntry::groupKeySeparator, key));
	return ret;
}

///////////////////////////////////////////////////////////////////
CncOSDConfigList::OSDEntry::OSDEntry()
: delimiter('\0')
, id("","")
, values()
///////////////////////////////////////////////////////////////////
{
}
///////////////////////////////////////////////////////////////////
CncOSDConfigList::OSDEntry::OSDEntry(const wxString& id, const wxString& value)
: delimiter('\0')
, id("","")
, values()
///////////////////////////////////////////////////////////////////
{
	wxString group(id.BeforeFirst(OSDEntry::groupKeySeparator));
	wxString key(id.AfterFirst(OSDEntry::groupKeySeparator));
	OSDEntry(group, key, value);
}
///////////////////////////////////////////////////////////////////
CncOSDConfigList::OSDEntry::OSDEntry(const wxString& group,
		                             const wxString& key,
									 const wxString& value)
: delimiter('\0')
, id("","")
, values()
///////////////////////////////////////////////////////////////////
{
	if ( key.IsEmpty() == true )
		return;
		
	// example:
	// OSD_XYZ=;abc;xyz
	
	id = std::make_pair(group, key);
	
	if ( value.IsEmpty() == true ) {
		values.push_back("");
	
	} else {
		delimiter = value[0];
		
		if ( delimiter == OSDEntry::groupKeySeparator ) {
			std::cerr << "CncOSDConfigList::OSDEntry::OSDEntry(): " << OSDEntry::groupKeySeparator << " isn't allowed eas delimiter!"<< std::endl;
			values.push_back("");
			
		} else {
			// tokennize values . . . 
			wxStringTokenizer tokenizer(value, wxString::Format("%c", delimiter));
			while ( tokenizer.HasMoreTokens() ) {
				wxString token = tokenizer.GetNextToken();
				if ( token.IsEmpty() )
					continue;
					
				values.push_back(token);
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
CncOSDConfigList::OSDEntry::~OSDEntry() {
///////////////////////////////////////////////////////////////////
	values.clear();
}
///////////////////////////////////////////////////////////////////
unsigned int CncOSDConfigList::OSDEntry::getCount() const {
///////////////////////////////////////////////////////////////////
	return values.size();
}
///////////////////////////////////////////////////////////////////
const wxString&	CncOSDConfigList::OSDEntry::getId(wxString& ret) const {
///////////////////////////////////////////////////////////////////
	ret.assign(OSDEntry::makeId(id.first, id.second));
	return ret;
}
///////////////////////////////////////////////////////////////////
const wxString&	CncOSDConfigList::OSDEntry::getGroup(wxString& ret) const {
///////////////////////////////////////////////////////////////////
	ret.assign(wxString::Format("%s", id.first));
	return ret;
}
///////////////////////////////////////////////////////////////////
const wxString&	CncOSDConfigList::OSDEntry::getKey(wxString& ret) const {
///////////////////////////////////////////////////////////////////
	ret.assign(wxString::Format("%s", id.second));
	return ret;
}
///////////////////////////////////////////////////////////////////
const wxString&	CncOSDConfigList::OSDEntry::getValue(unsigned short idx, wxString& ret) const {
///////////////////////////////////////////////////////////////////
	ret.clear();
	
	if ( values.size() == 0 )
		return ret;
	
	if ( idx > values.size() - 1 )
		return ret;
		
	ret.assign(values[idx]);
	
	return ret;
}
///////////////////////////////////////////////////////////////////
const wxString&	CncOSDConfigList::OSDEntry::getValues(wxString& ret) const {
///////////////////////////////////////////////////////////////////
	ret.clear();
	
	for (auto it = values.begin(); it != values.end(); ++it) {
		if ( delimiter != '\0' )
			ret.append(delimiter);
	
		ret.append(*it);
	}
	
	return ret;
}




///////////////////////////////////////////////////////////////////
CncOSDConfigList::CncOSDConfigList() {
///////////////////////////////////////////////////////////////////

}
///////////////////////////////////////////////////////////////////
CncOSDConfigList::~CncOSDConfigList() {
///////////////////////////////////////////////////////////////////
	configMap.clear();
}
///////////////////////////////////////////////////////////////////
bool CncOSDConfigList::add(const wxString& group, const wxString& key, const wxString& value) {
///////////////////////////////////////////////////////////////////
	wxString g(group);
	g.Trim().Trim(true);

	wxString k(key);
	k.Trim().Trim(true);

	if ( k.StartsWith("OSD_") == false )
		return false;

	OSDEntry e(g, k, value);

	wxString id;
	configMap[e.getId(id)] = e;

	return true;
}
///////////////////////////////////////////////////////////////////
bool CncOSDConfigList::update(const wxString& group, const wxString& key, const wxString& value) {
///////////////////////////////////////////////////////////////////
	return update(OSDEntry::makeId(group, key), value);;
}
///////////////////////////////////////////////////////////////////
const wxString& CncOSDConfigList::get(const wxString& group, const wxString& key, wxString& value) const {
///////////////////////////////////////////////////////////////////
	getFromIndex(group, key, OSD_INDEX, value);
	return value;
}
///////////////////////////////////////////////////////////////////
const wxString& CncOSDConfigList::getFromIndex(const wxString& group, const wxString& key, unsigned short idx, wxString& value) const {
///////////////////////////////////////////////////////////////////
	return getFromIndex(OSDEntry::makeId(group, key), idx, value);
}
///////////////////////////////////////////////////////////////////
const wxString& CncOSDConfigList::getList(const wxString& group, const wxString& key, wxString& values) const {
///////////////////////////////////////////////////////////////////
	return getList(OSDEntry::makeId(group, key), values);
}
///////////////////////////////////////////////////////////////////
bool CncOSDConfigList::add(const wxString& id, const wxString& value) {
///////////////////////////////////////////////////////////////////
	const wxString group(id.BeforeFirst(OSDEntry::groupKeySeparator));
	const wxString key(id.BeforeFirst(OSDEntry::groupKeySeparator));
	
	return add(group, key, value);
}
///////////////////////////////////////////////////////////////////
bool CncOSDConfigList::update(const wxString& id, const wxString& value) {
///////////////////////////////////////////////////////////////////
	auto it = configMap.find(id);
	if ( it == configMap.end() )
		return false;
	
	OSDEntry& e = it->second;
	if ( OSD_INDEX < e.getCount() ) {
		e.values[OSD_INDEX] = value;
	
		wxString id;
		configMap[e.getId(id)] = e;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////
const wxString& CncOSDConfigList::get(const wxString& id, wxString& value) const {
///////////////////////////////////////////////////////////////////
	return getFromIndex(id, OSD_INDEX, value);
}
///////////////////////////////////////////////////////////////////
const wxString& CncOSDConfigList::getFromIndex(const wxString& id, unsigned short idx, wxString& value) const {
///////////////////////////////////////////////////////////////////
	value.clear();
	auto it = configMap.find(id);
	if ( it == configMap.end() )
		return value;
	
	const OSDEntry e = it->second;
	
	e.getValue(idx, value);
	return value;
}
///////////////////////////////////////////////////////////////////
const wxString& CncOSDConfigList::getList(const wxString& id, wxString& values) const {
///////////////////////////////////////////////////////////////////
	values.clear();
	
	wxString value;
	auto it = configMap.find(id);
	if ( it == configMap.end() )
		return values;
	
	const OSDEntry e = it->second;
	for (unsigned short i = 0; i< e.getCount(); i++ )
		values.append(wxString::Format("%c%s",e.delimiter, e.getValue(i, value)));
	
	return values;
}



///////////////////////////////////////////////////////////////////
void CncOSDConfigList::test() {
///////////////////////////////////////////////////////////////////

	std::cout << "CncOSDConfigList::test()" << std::endl;

	CncOSDConfigList l;
	l.add("Group", "OSD_Name", ";Name1;Name2");
	l.add("Group", "OSD_Twins", ";Name4;Name5");
	l.add("Group", "OSD_XXX", ";a");
	l.add("Group", "OSD_YYY", "a;b");
	l.add("Group", "OSD_ZZZ", "");
	l.add("Group", "ZZZ", "");

	std::cout << l << std::endl;

	wxString ret, x;
	x.assign("OSD_Name"); 	std::cout << x << "=" << l.get("Group", x, ret) << std::endl;
	x.assign("OSD_XXX"); 	std::cout << x << "=" << l.get("Group", x, ret) << std::endl;
	x.assign("OSD_YYY"); 	std::cout << x << "=" << l.get("Group", x, ret) << std::endl;
	x.assign("OSD_ZZZ"); 	std::cout << x << "=" << l.get("Group", x, ret) << std::endl;
	x.assign("ZZZ"); 		std::cout << x << "=" << l.get("Group", x, ret) << std::endl;
	x.assign("OSD_Twins");	std::cout << x << "=" << l.get("Group", x, ret) << std::endl;


	l.update("Group", "OSD_Name", "Name3");
	x.assign("OSD_Name");	std::cout << x << "=" << l.get("Group", x, ret) << std::endl;
	x.assign("OSD_Name");	std::cout << x << "=" << l.getList("Group", x, ret) << std::endl;


}
