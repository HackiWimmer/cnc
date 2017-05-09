#include "SVGElementConverter.h"

wxXmlNode* SVGElementConverter::_xmlNode = NULL;

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