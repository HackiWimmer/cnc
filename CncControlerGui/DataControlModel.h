#ifndef DATA_CONTROL_MODEL_H
#define DATA_CONTROL_MODEL_H

#include <wx/variant.h>
#include <wx/vector.h>
#include "CncArduino.h"

typedef wxVector<wxVector<wxVariant>> DcmItemList;
typedef wxVector<wxVariant> DcmRow;

class DataControlModel {

	private:
		static DcmRow _row;
			
	public:
		DataControlModel() {}
		~DataControlModel() {}
		
		////////////////////////////////////////////////////////////////////////
		static void addKeyCheckValueRow(DcmItemList& ret, int key, wxVariant value) {
			_row.clear();
			_row.push_back(wxString::Format(wxT("%i"),key));
			_row.push_back(true);
			_row.push_back(value.GetString());
			ret.push_back(_row);
		}
		
		////////////////////////////////////////////////////////////////////////
		static void addKeyValueRow(DcmItemList& ret, const char* key, wxVariant value) {
			_row.clear();
			_row.push_back(key);
			_row.push_back(value.GetString());
			ret.push_back(_row);
		}
		
		////////////////////////////////////////////////////////////////////////
		static void addNumKeyValueRow(DcmItemList& ret, const char* key, wxVariant value) {
			_row.clear();
			_row.push_back(wxString::Format(wxT("%i"),(int)(ret.size() + 1)));
			_row.push_back(key);
			_row.push_back(value.GetString());
			ret.push_back(_row);
		}
		////////////////////////////////////////////////////////////////////////
		static void addNumParameterValueUnitRow(DcmItemList& ret, const char* parameter, wxVariant value, const char* unit ) {
			_row.clear();
			_row.push_back(parameter);
			_row.push_back(value.GetString());
			_row.push_back(unit);
			ret.push_back(_row);
		}
		////////////////////////////////////////////////////////////////////////
		static void addNumKeyValueRow(DcmItemList& ret, unsigned int count, const char* key, wxVariant value) {
			_row.clear();
			_row.push_back(wxString::Format(wxT("%i"),(int)count));
			_row.push_back(key);
			_row.push_back(value.GetString());
			ret.push_back(_row);
		}
		////////////////////////////////////////////////////////////////////////
		static void addNumKeyValueRow(DcmItemList& ret, unsigned int count, unsigned int code, const char* key, wxVariant value) {
			_row.clear();
			_row.push_back(wxString::Format(wxT("%i"),(int)count));
			_row.push_back(wxString::Format(wxT("%i"),(int)code));
			_row.push_back(key);
			_row.push_back(value.GetString());
			ret.push_back(_row);
		}
		////////////////////////////////////////////////////////////////////////
		static void addPinReportRow(DcmItemList& ret, const char* desc, int pin, int type, int mode, int value) {
			_row.clear();
			
			if ( pin >= 0 && pin < MAX_PINS ) {
				_row.push_back(wxString::Format(wxT("%i"),(int)pin));
				_row.push_back(desc);
				_row.push_back((type == (int)'D' ? "Digital" : "Analog"));
				_row.push_back((mode == (int)'O' ? "Output" : "Input"));
				_row.push_back(wxString::Format(wxT("%i"),(char)value));
				ret.push_back(_row);
			} else {
				_row.push_back("-");
				_row.push_back("Not available, because there's no controller connection");
				_row.push_back("-");
				_row.push_back("-");
				_row.push_back("-");
				ret.push_back(_row);
			}
		}
};

#endif