#include "SVGElementConverter.h"

wxXmlNode* SVGElementConverter::_xmlNode 	= NULL;
wxString SVGElementConverter::_errorInfo 	= _T("");

/////////////////////////////////////////////////////////////////////////
class SVGElementConverterInitializer {
	public:
		SVGElementConverterInitializer() {
			SVGElementConverter::init();
		}
		~SVGElementConverterInitializer() {
			SVGElementConverter::cleanUp();
		}
		
}; SVGElementConverterInitializer seci;